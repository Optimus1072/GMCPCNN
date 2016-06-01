//
// Created by wrede on 19.04.16.
//
#include "../core/DetectionSequence.h"
#include "../util/FileIO.h"
#include "../util/Parser.h"
#include "../algo/TwoStage.h"
#include "../algo/KShortestPaths.h"
#include "../visual/Visualizer.h"
#include "../util/Logger.h"
#include "../core/ObjectDataAngular.h"
#include <boost/program_options.hpp>

void ReadInput(const std::string& input_file, core::DetectionSequence& sequence,
               double temporal_weight, double spatial_weight, double angular_weight)
{
    util::Logger::LogInfo("Reading input");

    util::Vector3d values;
    util::FileIO::ReadCSV(values, input_file);
    util::Parser::ParseObjectDataAngular(values, sequence,
                                         temporal_weight,
                                         spatial_weight,
                                         angular_weight);

    if (util::Logger::IsDebugEnabled())
    {
        size_t sequence_object_count = 0;
        for (size_t i = 0; i < sequence.GetFrameCount(); i++)
        {
            sequence_object_count += sequence.GetObjectCount(i);
        }
        util::Logger::LogDebug("sequence object count " + std::to_string(sequence_object_count));
    }
}

struct
{
    size_t iterations;
    size_t max_frame_skip;
    size_t max_tracklet_count;
    double penalty_value;
} two_stage_params;

void RunTwoStage(core::DetectionSequence& sequence, const std::string& output_file,
                 const std::string& images_folder, bool display)
{
    util::Logger::LogInfo("Running two-stage");

    algo::TwoStage two_stage(two_stage_params.max_frame_skip,
                             two_stage_params.penalty_value,
                             two_stage_params.max_tracklet_count);

    // Running the two stage graph algorithm
    DirectedGraph obj_graph;
    two_stage.CreateObjectGraph(obj_graph, sequence);

    // Run the tracklet creation at least once
    DirectedGraph tlt_graph_1, tlt_graph_2;
    two_stage.CreateTrackletGraph(obj_graph, tlt_graph_1,
                                  sequence.GetFrameCount());

    // Run the tracklet creation iteratively
    for (size_t i = 1; i < two_stage_params.iterations; ++i)
    {
        if (i % 2 == 0)
        {
            two_stage.CreateTrackletGraph(tlt_graph_2, tlt_graph_1,
                                          sequence.GetFrameCount());
        }
        else
        {
            two_stage.CreateTrackletGraph(tlt_graph_1, tlt_graph_2,
                                          sequence.GetFrameCount());
        }
    }

    // Extract tracklets and flatten tracklets
    std::vector<core::TrackletPtr> tracks;
    if (two_stage_params.iterations % 2 == 0)
    {
        two_stage.ExtractTracks(tlt_graph_2, two_stage_params.iterations - 1, tracks);
    }
    else
    {
        two_stage.ExtractTracks(tlt_graph_1, two_stage_params.iterations - 1, tracks);
    }

    // Interpolate tracks
    for (auto track : tracks)
    {
        track->InterpolateMissingFrames();
    }

    // Display the tracking data
    if (display)
    {
        util::Logger::LogInfo("Displaying data");

        visual::Visualizer vis;
        vis.Display(tracks, images_folder);
    }

    util::Logger::LogInfo("Finished");
}

void Run(int argc, char** argv)
{
    // Algorithm independent values
    std::string input_file, output_file, images_folder, algorithm;
    bool display;

    // Input dependent variables
    double temporal_weight, spatial_weight, angular_weight;

    boost::program_options::options_description opts("Allowed options");
    opts.add_options()
            ("help",
             "produce help message")
            ("info",
             "if the program should show progress information")
            ("debug",
             "if the program should show debug messages")
            ("input-file,i",
             boost::program_options::value<std::string>(&input_file),
             "set detections file path")
            ("output-file,o",
             boost::program_options::value<std::string>(&output_file),
             "set the output file path")
            ("algorithm,a",
             boost::program_options::value<std::string>(&algorithm),
             "set the algorithm to use, current viable options: two-stage")
            ("display",
             "if a window with the images and the detected tracks should be opened")
            ("images-folder,f",
             boost::program_options::value<std::string>(&images_folder),
             "set images folder path")
            ("iterations",
             boost::program_options::value<size_t>(&two_stage_params.iterations)->default_value((2)),
             "(two-stage) number of tracklet extraction iterations")
            ("max-frame-skip",
             boost::program_options::value<size_t>(&two_stage_params.max_frame_skip)->default_value(1),
             "(two stage) set the maximum number of frames a track can skip between two detections,"
                     " if set to less or equal than zero all frames are linked")
            ("max-tracklet-count",
             boost::program_options::value<size_t>(&two_stage_params.max_tracklet_count)->default_value(1),
             "(two stage) set the maximum number of tracklets to be extracted")
            ("penalty-value",
             boost::program_options::value<double>(&two_stage_params.penalty_value)->default_value(0.0),
             "(two stage) set the penalty value for edges from and to source and sink")
            ("temporal-weight",
             boost::program_options::value<double>(&temporal_weight)->default_value(1.0),
             "temporal weight for difference calculations between two detections")
            ("spatial-weight",
             boost::program_options::value<double>(&spatial_weight)->default_value(1.0),
             "spatial weight for difference calculations between two detections")
            ("angular-weight",
             boost::program_options::value<double>(&angular_weight)->default_value(1.0),
             "angular weight for difference calculations between two detections");

    boost::program_options::variables_map opt_var_map;
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wincompatible-pointer-types-discards-qualifiers"
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, opts), opt_var_map);
#pragma clang diagnostic pop
    boost::program_options::notify(opt_var_map);

    if (opt_var_map.count("help") != 0)
    {
        std::cout << opts << std::endl;
        exit(0);
    }

    if (opt_var_map.count("info") != 0)
    {
        util::Logger::SetInfo(true);
        util::Logger::LogInfo("Enabled");
    }

    if (opt_var_map.count("debug") != 0)
    {
        util::Logger::SetDebug(true);
        util::Logger::LogDebug("Enabled");
    }

    display = opt_var_map.count("display") != 0;

    core::DetectionSequence sequence;

    ReadInput(input_file, sequence, temporal_weight, spatial_weight, angular_weight);

    if (algorithm == "two-stage")
    {
        RunTwoStage(sequence, output_file, images_folder, display);
    }
    else
    {
        std::cout << opts << std::endl;
        exit(0);
    }
}

void CreateTestGraph(DirectedGraph& graph, Vertex& source, Vertex& sink)
{
    // Create test graph (suurballe wikipedia example)
//    std::vector<Vertex> vertices;
//    for (size_t i = 0; i < 6; ++i)
//    {
//        vertices.push_back(
//                boost::add_vertex(
//                        core::ObjectDataPtr(new core::ObjectData(i)),graph));
//    }
//
//    // AB
//    boost::add_edge(vertices[0], vertices[1], 1.0, graph);
//
//    // AC
//    boost::add_edge(vertices[0], vertices[2], 2.0, graph);
//
//    // BD
//    boost::add_edge(vertices[1], vertices[3], 1.0, graph);
//
//    // BE
//    boost::add_edge(vertices[1], vertices[4], 2.0, graph);
//
//    // CD
//    boost::add_edge(vertices[2], vertices[3], 2.0, graph);
//
//    // DF
//    boost::add_edge(vertices[3], vertices[5], 1.0, graph);
//
//    // EF
//    boost::add_edge(vertices[4], vertices[5], 2.0, graph);
//
//    source = vertices[0];
//    sink = vertices[5];

    // Create test graph (disjoint path finding example)
    std::vector<Vertex> vertices;
    for (size_t i = 0; i < 8; ++i)
    {
        vertices.push_back(
                boost::add_vertex(
                        core::ObjectDataPtr(new core::ObjectData(i)),graph));
    }

    boost::add_edge(vertices[0], vertices[1], 1.0, graph);
    boost::add_edge(vertices[0], vertices[4], 1.0, graph);
    boost::add_edge(vertices[1], vertices[2], 1.0, graph);
    boost::add_edge(vertices[2], vertices[3], 1.0, graph);
    boost::add_edge(vertices[3], vertices[7], 1.0, graph);
    boost::add_edge(vertices[4], vertices[3], 1.0, graph);
    boost::add_edge(vertices[4], vertices[5], 3.0, graph);
    boost::add_edge(vertices[5], vertices[6], 1.0, graph);
    boost::add_edge(vertices[5], vertices[3], 1.0, graph);
    boost::add_edge(vertices[6], vertices[7], 1.0, graph);

    source = vertices[0];
    sink = vertices[7];
}

int main(int argc, char** argv)
{
    Run(argc, argv);

    //TestTracklet();

//    Vertex source, sink;
//    DirectedGraph graph;
//
//    util::Logger::SetDebug(true);
//    util::Logger::SetInfo(true);
//
//    CreateTestGraph(graph, source, sink);
//
//    algo::KShortestPaths ksp(graph, source, sink);
//    MultiPredecessorMap paths = ksp.Run(3);
//
//    util::Logger::LogDebug("found paths:");
//    for (Vertex first : paths[sink])
//    {
//        std::string path;
//        path += std::to_string(sink) + "->" + std::to_string(first);
//
//        for (Vertex u = first, v = (*paths[u].begin());
//             u != v; u = v, v = (*paths[v].begin()))
//        {
//            path += "->" + std::to_string(v);
//        }
//
//        util::Logger::LogDebug(path);
//    }

    return 0;
}