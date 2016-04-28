/*
 * markerAssociationDP.cpp
 *
 *  Created on: Feb 6, 2013
 *      Author: koerner
 */


#include <boost/config.hpp>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/iteration_macros.hpp>
#include <boost/graph/properties.hpp>

#include <boost/property_map/property_map.hpp>

#include <boost/program_options.hpp>
#include <boost/format.hpp>

#include <opencv2/opencv.hpp>

#include <limits>
#include <iostream>
#include <fstream>
#include <utility>
#include <vector>

// command line parameters
struct Params {
    // filename of 3d point filen
    std::string strPointClouds;

    // file in which the results are saved
    std::string outputFilename;

    // max link distance
    double maxLinkDistance;

    // max angular distance
    double maxAngularDistance;

    // number of tracklets to be extracted
    size_t trackletCount;

    // number of allowed frames which can be skipped within one tracklet
    size_t allowedFrameSkip;
};

// each node contains this struct to provide information about the 3d point
struct NodeInfo {
    // creates virtual marker
    NodeInfo() {
        this->position = cv::Point3d(0, 0, 0);
        this->frame = std::numeric_limits<size_t>::max();
    }

    // create normal marker info
    NodeInfo(const cv::Point3d &_position, const size_t &_frame) {
        this->position = _position;
        this->frame = _frame;
    }

    //
    bool isVirtual() {
        return (this->frame == std::numeric_limits<size_t>::max());
    }

    //
    cv::Point3d position;
    size_t frame;
};

// get command line parameters
bool evalCmdLine(int argc, char **argv, Params &p) {
    // define parameters
    boost::program_options::options_description desc("Allowed options");
    desc.add_options()
            ("help", "produce help message")
            ("pointFile,p",
             boost::program_options::value< std::string >(),
             "3d point clouds file containing the detections for each frame")
            ("outputFile,o",
             boost::program_options::value< std::string >(),
             "output file for the generated tracklets")
            ("trackletCount,c",
             boost::program_options::value< size_t >()->default_value(1),
             "number of tracklets to be extracted")
            ("maxLinkDistance,d",
             boost::program_options::value< double >()->default_value(1.0),
             "maximum Euclidean distance (in px) allowed for two detections to get linked")
            ("maxAngularDistance,a",
             boost::program_options::value< double >()->default_value(20.0),
             "maximum angular distance (in deg) allowed for two detections to get linked")
            ("allowedFrameSkip,s",
             boost::program_options::value< size_t >()->default_value(1),
             "number of allowed frames which can be skipped within one tracklet (>= 1)")
            ;

    // parse parameters
    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
    boost::program_options::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        exit(EXIT_SUCCESS);
    }

    // check parameters
    if (vm.count("pointFile")) {
        p.strPointClouds = vm["pointFile"].as< std::string >();
    } else {
        std::cerr << "No points file passed!" << std::endl;
        std::cout << desc << std::endl;
        exit(EXIT_SUCCESS);
    }
    if (vm.count("outputFile")) {
        p.outputFilename = vm["outputFile"].as< std::string >();
    } else {
        std::cerr << "No output file passed!" << std::endl;
        std::cout << desc << std::endl;
        exit(EXIT_SUCCESS);
    }

    p.maxLinkDistance = vm["maxLinkDistance"].as< double >();
    p.maxAngularDistance = vm["maxAngularDistance"].as< double >();
    p.trackletCount = vm["trackletCount"].as< size_t >();
    p.allowedFrameSkip = vm["allowedFrameSkip"].as< size_t >();

    return true;
}

void readPoints3d(const std::string &filename,
                  std::vector< std::vector< cv::Point3d > > &clouds) {
    std::cout << "Reading points from file " << filename << std::endl;
    std::ifstream pointFile(filename.c_str(), std::ios_base::in);
    if (!pointFile.is_open()) {
        std::cerr << "Could not open file " << std::endl;
        return;
    }

    uint frame = 0;
    uint detections = 0;
    std::string line;
    while (std::getline(pointFile, line)) {
        std::cerr << "\rReading points of frame " << frame + 1 << "... ";

        std::vector< cv::Point3d > cloud;

        std::stringstream split(line + " ");
        double tmpValue;
        int tmpDim = 0;
        cv::Point3d tmpPoint;

        while ((split >> tmpValue).good()) {
            switch (tmpDim) {
                case 0:
                    tmpPoint.x = tmpValue;
                    break;
                case 1:
                    tmpPoint.y = tmpValue;
                    break;
                case 2:
                    tmpPoint.z = tmpValue;
                    cloud.push_back(tmpPoint);
                    //std::cout << cv::Mat(tmpPoint) << std::endl;
                    detections++;
                    break;
            }
            tmpDim = (tmpDim + 1) % 3;
        }

        clouds.push_back(cloud);

        std::cout << cloud.size() << std::flush;

        frame++;
    }

    uint nFrames = clouds.size();
    std::cerr << std::endl << "Read " << nFrames << " frames (" << detections << " detections)" << std::endl;
}

void createTracklets(const std::vector< std::vector< cv::Point3d > > &clouds, const Params &params) {
    typedef double Weight;
    typedef boost::property< boost::edge_weight_t, Weight > WeightProperty;
//  typedef boost::property< boost::vertex_name_t, std::string > NameProperty;
    typedef boost::property< boost::vertex_name_t, NodeInfo > NameProperty;

    typedef boost::adjacency_list<
            boost::listS,
            boost::vecS,
            boost::directedS,
            NameProperty,
            WeightProperty > Graph;

    typedef boost::graph_traits< Graph >::vertex_descriptor Vertex;

    typedef boost::property_map< Graph, boost::vertex_index_t >::type IndexMap;
    typedef boost::property_map< Graph, boost::vertex_name_t >::type NameMap;

    typedef boost::iterator_property_map< Vertex*, IndexMap, Vertex, Vertex& > PredecessorMap;
    typedef boost::iterator_property_map< Weight*, IndexMap, Weight, Weight& > DistanceMap;

    // Create a graph
    Graph g;
    std::vector< std::vector< Vertex > > vertices;

    size_t frameStart = 0;
    size_t frameMax = clouds.size();
//  frameMax = 900;

    // adding vertices
    for (size_t frame = frameStart; frame < frameMax; ++frame) {
        std::cout << "\rAdding layer for frame " << frame + 1; // << std::flush;

        std::vector< cv::Point3d > p = clouds[frame];
        std::vector< Vertex > layer;

        for (size_t marker = 0; marker < p.size(); ++marker) {
            Vertex v = boost::add_vertex(NodeInfo(p[marker], frame), g);
            layer.push_back(v);
        }
        vertices.push_back(layer);
        std::cout << ": " << p.size() << ", " << layer.size() << ", " << vertices.rbegin()->size() << std::flush;
    }

    // virtual source and sink
    Vertex vSource = boost::add_vertex(NodeInfo(), g);
    Vertex vSink = boost::add_vertex(NodeInfo(), g);

    NameMap nameMap = boost::get(boost::vertex_name, g);

    std::cout << std::endl;

    // adding edges (layer eq frame)
    for (size_t iLayers = 0; iLayers < vertices.size(); ++iLayers) {
        // all points of frame "iLayers"
        std::vector< Vertex > p = vertices[iLayers];

        // for each point of layer "iLayers"
        for (size_t iP = 0; iP < p.size(); ++iP) {
            Vertex vP = p[iP];

            // create edges to source and sink
            std::cout << "\rAdding edges " << iLayers << "-> vSink" << std::flush;
if (iLayers <= 100)
 boost::add_edge(vSource, vP, (iLayers + 1) * (params.maxLinkDistance + params.maxAngularDistance), g);
if (iLayers + 100 >= vertices.size())
 boost::add_edge(vP, vSink, (vertices.size() - iLayers) * (params.maxLinkDistance + params.maxAngularDistance), g);

            // create shortcuts to next-next layers
            for (size_t iOffset = 1; iOffset <= params.allowedFrameSkip; ++iOffset) {
                if (iLayers + iOffset < vertices.size() - 1) {
                    std::cout << "\rAdding edges for frames " << iLayers << "->" << (iLayers + iOffset) << std::flush;

                    // all points of frame "iLayers" + "iOffset"
                    std::vector< Vertex > q = vertices[iLayers + iOffset];

                    // for each node of neighboring layers (in time)
                    for (size_t iQ = 0; iQ < q.size(); ++iQ) {
                        Vertex vQ = q[iQ];

                        // euclidean distance between iP and iQ
                        cv::Point3d ptP = nameMap[vP].position;
                        cv::Point3d ptQ = nameMap[vQ].position;
                        double dx = ptP.x - ptQ.x;
                        double dy = ptP.y - ptQ.y;
                        double dz = ptP.z - ptQ.z;
                        Weight distPosition = sqrt(dx * dx + dy * dy);
                        Weight distAngular = std::max(0.0, abs(dz) - 5.0);

                        // edge weight
                        //std::cout << " x" << params.maxLinkDistance << "," << params.maxAngularDistance << "; " << distPosition << "," << distAngular << "," << iOffset << " = " << 0.1 * (distPosition + 0.5 * distAngular) * double(iOffset) << std::endl;
                        if (false || (distPosition < params.maxLinkDistance && distAngular < params.maxAngularDistance)) {
                            // if we discard one layer, the edge weight should be higher!
                            boost::add_edge(vP, vQ, 0.1 * (distPosition + 0.5 * distAngular) * double(iOffset), g);
                            //boost::add_edge(vP, vQ, 0.001 * Weight((1.0 + 0.33 * distPosition) * (1.0 + distAngular) * Weight(1.0 + iOffset)), g);
                            //std::cout << "x" << params.maxAngularDistance << "," << distPosition << "," << distAngular << "," << iOffset << std::endl;
                            //boost::add_edge(vP, vQ, Weight(dist * Weight(1 + 0.5 * iOffset)), g);
                            //              boost::add_edge(vP, vQ, Weight(dist), g);
                        }
                    }
                }
            }
        }
    }
    std::cout << std::endl;

    // Create things for Dijkstra
    std::vector< Vertex > predecessors(boost::num_vertices(g));  // To store parents
    std::vector< Weight > distances(boost::num_vertices(g));  // To store distances

    IndexMap indexMap = boost::get(boost::vertex_index, g);
    PredecessorMap predecessorMap(&predecessors[0], indexMap);
    DistanceMap distanceMap(&distances[0], indexMap);

    // output file
    std::ofstream f(params.outputFilename.c_str());

    // Compute shortest paths from input layer vertices to the sink
    for (size_t tracklet = 0; tracklet < params.trackletCount; ++tracklet) {
        boost::dijkstra_shortest_paths(g,         // the graph
                                       vSource,   // the source vertex
                                       boost::distance_map(distanceMap).predecessor_map(predecessorMap));

        // Output results
//    std::cout << "distances and parents:" << std::endl;
//  NameMap nameMap = boost::get(boost::vertex_name, g);

//  BGL_FORALL_VERTICES(v, g, Graph) {
//    for (size_t i = 0; i < vertices.size(); ++i) {
//      for (size_t j = 0; j < vertices[i].size(); ++j) {
//        Vertex *v = &vertices[i][j];
//        std::cout << "distance(" << nameMap[vSink] << ", " << nameMap[*v] << ") = " << distanceMap[*v] << ", ";
//        std::cout << "predecessor(" << nameMap[*v] << ") = " << nameMap[predecessorMap[*v]] << std::endl;
//      }
//    }
//
//    std::cout << std::endl;


        // Extract the shortest path

        typedef std::vector< Graph::edge_descriptor > PathType;

        PathType path;

        Vertex v = vSink;  // We want to start at the sink and work our way back to the source
        for (Vertex u = predecessorMap[v]; u != v; v = u, u = predecessorMap[v]) {
            std::pair< Graph::edge_descriptor, bool > edgePair = boost::edge(u, v, g);
            Graph::edge_descriptor edge = edgePair.first;
            path.push_back(edge);
        }

        // Write shortest path
        //int c = 0;
        bool vFirst = true;
        //std::cout << "Shortest path from vSource to vSink: " << std::endl;
        for (PathType::reverse_iterator pathIterator = path.rbegin(); pathIterator != path.rend(); ++pathIterator) {
            // print all non-virtual nodes of this path
            if (!nameMap[boost::source(*pathIterator, g)].isVirtual()) {
                //std::cout << nameMap[boost::source(*pathIterator, g)].position << " -> " << nameMap[boost::target(*pathIterator, g)].position
                //        << " = "
                //        << boost::get(boost::edge_weight, g, *pathIterator) << std::endl;

                f << nameMap[boost::source(*pathIterator, g)].position.x << " "
                << nameMap[boost::source(*pathIterator, g)].position.y << " "
                << nameMap[boost::source(*pathIterator, g)].position.z << " "
                << nameMap[boost::source(*pathIterator, g)].frame << " ";
            }

            // for each node of the path, set the weights of all outgoing edges to Inf (for Dijkstra equivalent to deletion of the node)
            if (!vFirst) {
                std::pair<Graph::out_edge_iterator, Graph::out_edge_iterator> edgeIts = boost::out_edges(boost::source(*pathIterator, g), g);
                //size_t nEdges = 0;
                for (Graph::out_edge_iterator edgeIt = edgeIts.first; edgeIt != edgeIts.second; edgeIt++) {
                    boost::get(boost::edge_weight, g, *edgeIt) = std::numeric_limits<Weight>::infinity();
                    //std::cout << "w = " << boost::get(boost::edge_weight, g, *edgeIt) << std::endl;
                    //nEdges++;
                }
                //std::cout << "Found " << nEdges << " out edges for this node" << std::endl;
                //getchar();
            }

            vFirst = false;
        }

        f << "\n";

        std::cout << "Found tracklet #" << tracklet + 1 << "/" << params.trackletCount << ": " << path.size() << " nodes, distance: " << distanceMap[vSink] << std::endl;
    }
    f.close();
}

int main(int argc, char** argv) {
    // get command line parameters
    Params p;
    if (!evalCmdLine(argc, argv, p)) {
        std::cerr << "Error while parsing arguments!" << std::endl;
        return 1;
    }

    std::vector< std::vector< cv::Point3d > > points3dLists;
    if (!p.strPointClouds.empty()) {
        readPoints3d(p.strPointClouds, points3dLists);
    }

    createTracklets(points3dLists, p);

    return EXIT_SUCCESS;
}

