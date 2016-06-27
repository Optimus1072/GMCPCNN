//
// Created by wrede on 19.04.16.
//
#include "../core/DetectionSequence.h"
#include "../util/FileIO.h"
#include "../util/Parser.h"
#include "../algo/NStage.h"
#include "../algo/KShortestPaths.h"
#include "../util/Visualizer.h"
#include "../util/Logger.h"
#include "../core/ObjectDataAngular.h"
#include "../algo/Berclaz.h"
#include "../algo/KShortestPaths2.h"
#include "../algo/KShortestPaths3.h"
#include "../algo/KShortestPaths4.h"
#include <boost/program_options.hpp>
#include <boost/graph/named_function_params.hpp>
#include <boost/graph/bellman_ford_shortest_paths.hpp>

struct
{
    size_t max_frame_skip;
    std::string max_tracklet_count;
    std::string penalty_value;
} n_stage_params;

void RunNStage(core::DetectionSequence& sequence,
               std::vector<core::TrackletPtr>& tracks)
{
    util::Logger::LogInfo("Running n-stage");

    std::vector<double> penalty_values;
    std::vector<size_t> max_tracklet_counts;

    // Parse strings to vectors
    size_t d_index;
    std::string str, part;
    str = n_stage_params.max_tracklet_count;
    do
    {
        d_index = str.find(",");

        part = str.substr(0, d_index);

        if (part.size() > 0)
        {
            max_tracklet_counts.push_back((unsigned long&&) atoi(part.c_str()));
        }

        str = str.substr(d_index + 1);
    }
    while (d_index != std::string::npos);
    str = n_stage_params.penalty_value;
    do
    {
        d_index = str.find(",");

        part = str.substr(0, d_index);

        if (part.size() > 0)
        {
            penalty_values.push_back(std::atof(part.c_str()));
        }

        str = str.substr(d_index + 1);
    }
    while (d_index != std::string::npos);

    // Init n stage
    algo::NStage n_stage(n_stage_params.max_frame_skip,
                         penalty_values, max_tracklet_counts);

    n_stage.Run(sequence, tracks);

    // Interpolate tracks
    for (auto track : tracks)
    {
        track->InterpolateMissingFrames();
    }

    util::Logger::LogInfo("Finished");
}

struct
{
    int h_res;
    int v_res;
    int vicinity_size;
    size_t batch_size;
    size_t max_track_count;
} berclaz_params;


void RunBerclaz(core::DetectionSequence& sequence,
                std::vector<core::TrackletPtr>& tracks)
{
    util::Logger::LogInfo("Running berclaz");

    // Init berclaz
    algo::Berclaz berclaz(berclaz_params.h_res,
                          berclaz_params.v_res,
                          berclaz_params.vicinity_size);
    berclaz.Run(sequence, berclaz_params.batch_size,
                berclaz_params.max_track_count, tracks);

    util::Logger::LogInfo("Interpolate tracks");

    // Interpolate tracks
    for (auto track : tracks)
    {
        track->InterpolateMissingFrames();
    }

    util::Logger::LogInfo("Finished");
}

void Run(int argc, char** argv)
{
    // Algorithm independent values
    std::string input_file, output_file, images_folder, algorithm, config_path, header, input_format;
    bool info, debug, display;
    char input_delimiter, output_delimiter;
    double temporal_weight, spatial_weight, angular_weight, image_width, image_height;

    boost::program_options::options_description opts("Allowed options");
    opts.add_options()
            ("help",
             "produce help message")
            ("info",
             boost::program_options::value<bool>(&info)
                    ->default_value(false),
             "if the program should show progress information")
            ("debug",
             boost::program_options::value<bool>(&debug)
                     ->default_value(false),
             "if the program should show debug messages")
            ("display",
             boost::program_options::value<bool>(&display)
                     ->default_value(false),
             "if a window with the images and the detected tracks should be opened")
            ("config",
             boost::program_options::value<std::string>(&config_path),
             "the path to the config file, if no path is given the command line arguments are read")
            ("input-file",
             boost::program_options::value<std::string>(&input_file),
             "set detections file path")
            ("output-file",
             boost::program_options::value<std::string>(&output_file),
             "set the output file path")
            ("output-delimiter",
             boost::program_options::value<char>(&output_delimiter)
                    ->default_value(';'),
             "the delimiter used to separate values in the specified output file")
            ("images-folder",
             boost::program_options::value<std::string>(&images_folder),
             "set images folder path")
            ("input-header",
             boost::program_options::value<std::string>(&header),
             "sets the input header, this value is optional if the input file has a header labeling the values,"
                     "the delimiter used for the header needs to be the same as for the rest of the file")
            ("input-format",
             boost::program_options::value<std::string>(&input_format)
                     ->default_value("ObjectData"),
             "the format the input should be parsed into, valid formats are: "
                     "2D, Box, Angular")
            ("input-delimiter",
             boost::program_options::value<char>(&input_delimiter)
                     ->default_value(';'),
             "the delimiter used to separate values in the specified input file")
            ("image-width",
             boost::program_options::value<double>(&image_width)
                     ->default_value(1),
             "the width of the image")
            ("image-height",
             boost::program_options::value<double>(&image_height)
                     ->default_value(1),
             "the height of the image")
            ("algorithm",
             boost::program_options::value<std::string>(&algorithm),
             "set the algorithm to use, current viable options: n-stage berclaz")
            ("max-frame-skip",
             boost::program_options::value<size_t>(&n_stage_params.max_frame_skip)
                     ->default_value(1),
             "(n stage) set the maximum number of frames a track can skip between two detections,"
                     " if set to less or equal than zero all frames are linked")
            ("max-tracklet-count",
             boost::program_options::value<std::string>(&n_stage_params.max_tracklet_count)
                     ->default_value("-1,1"),
             "(n stage) set the maximum number of tracklets to be extracted")
            ("penalty-value",
             boost::program_options::value<std::string>(&n_stage_params.penalty_value)
                     ->default_value("0,0"),
             "(n stage) set the penalty value for edges from and to source and sink")
            ("temporal-weight",
             boost::program_options::value<double>(&temporal_weight)
                     ->default_value(1.0),
             "(n stage) temporal weight for difference calculations between two detections")
            ("spatial-weight",
             boost::program_options::value<double>(&spatial_weight)
                     ->default_value(1.0),
             "(n stage) spatial weight for difference calculations between two detections")
            ("angular-weight",
             boost::program_options::value<double>(&angular_weight)
                     ->default_value(1.0),
             "(n stage) angular weight for difference calculations between two detections")
            ("horizontal-resolution",
             boost::program_options::value<int>(&berclaz_params.h_res)
                     ->default_value(10),
             "(berclaz) the number of horizontal grid cells")
            ("vertical-resolution",
             boost::program_options::value<int>(&berclaz_params.v_res)
                     ->default_value(10),
             "(berclaz) the number of vertical grid cells")
            ("vicinity-size",
             boost::program_options::value<int>(&berclaz_params.vicinity_size)
                     ->default_value(1),
             "(berclaz) the vicinity size, the number of cells a detection can travel between two frames")
            ("max-track-count",
             boost::program_options::value<size_t>(&berclaz_params.max_track_count)
                     ->default_value(1),
             "(berclaz) the maximal number of tracks to extract")
            ("batch-size",
             boost::program_options::value<size_t>(&berclaz_params.batch_size)
                     ->default_value(100),
             "(berclaz) the size of one processing batch");

    boost::program_options::variables_map opt_var_map;
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wincompatible-pointer-types-discards-qualifiers"
    boost::program_options::store(
            boost::program_options::parse_command_line(argc, argv, opts),
            opt_var_map);
#pragma clang diagnostic pop
    boost::program_options::notify(opt_var_map);

    // Display help
    if (opt_var_map.count("help") != 0)
    {
        std::cout << opts << std::endl;
        exit(0);
    }

    // Read config
    if (opt_var_map.count("config") != 0)
    {
        std::ifstream config_file(config_path, std::ifstream::in);

        if (config_file.is_open())
        {
            boost::program_options::store(
                    boost::program_options::parse_config_file(config_file,
                                                              opts),
                    opt_var_map);
            config_file.close();
            boost::program_options::notify(opt_var_map);
        }
        else
        {
            util::Logger::LogError("Unable to open config file!");
            exit(0);
        }
    }
    else if (opt_var_map.count("input-file") == 0 ||
             opt_var_map.count("input-format") == 0 ||
             opt_var_map.count("output-file") == 0)
    {
        std::cout << opts << std::endl;
        exit(0);
    }

    // Enable info logging
    if (info != 0)
    {
        util::Logger::SetInfo(true);
        util::Logger::LogInfo("Enabled");
    }

    // Enable debug logging
    if (debug != 0)
    {
        util::Logger::SetDebug(true);
        util::Logger::LogDebug("Enabled");
    }

    // Reading the input file
    util::Logger::LogInfo("Reading input");
    util::ValueMapVector values;
    try
    {
        if (header.size() > 0)
        {
            util::FileIO::ReadCSV(values, header, input_file, input_delimiter);
        }
        else
        {
            util::FileIO::ReadCSV(values, input_file, input_delimiter);
        }
    }
    catch (std::exception& e)
    {
        util::Logger::LogError("Failed to read input file!");
        util::Logger::LogError(e.what());
        exit(0);
    }

    // Parsing the read input
    core::DetectionSequence sequence;
    if (input_format == "2D")
    {
        util::Parser::ParseObjectData2D(values,
                                        sequence,
                                        image_width,
                                        image_height,
                                        temporal_weight,
                                        spatial_weight);
    }
    else if (input_format == "Box")
    {
        util::Parser::ParseObjectDataBox(values,
                                         sequence,
                                         image_width,
                                         image_height,
                                         temporal_weight,
                                         spatial_weight);
    }
    else if (input_format == "Angular")
    {
        util::Parser::ParseObjectDataAngular(values,
                                             sequence,
                                             image_width,
                                             image_height,
                                             temporal_weight,
                                             spatial_weight,
                                             angular_weight);
    }
    else
    {
        // No valid input-format specified
        std::cout << opts << std::endl;
        exit(0);
    }

    // Running the specified algorithm
    std::vector<core::TrackletPtr> tracks;
    time_t begin_time, end_time;
    util::Logger::LogInfo("Start time measurement");
    begin_time = time(0);
    if (algorithm == "n-stage")
    {
        RunNStage(sequence, tracks);
    }
    else if (algorithm == "berclaz")
    {
        RunBerclaz(sequence, tracks);
    }
    else
    {
        // No valid algorithm specified
        std::cout << opts << std::endl;
        exit(0);
    }

    end_time = time(0);
    util::Logger::LogInfo("Time measurement stopped");
    util::Logger::LogInfo("Time passed: "
                          + std::to_string(difftime(end_time, begin_time) / 60.0)
                          + " minutes");

    // Write the output file
    util::FileIO::WriteTracks(tracks, output_file, output_delimiter);

    // Display the tracking data
    if (display)
    {
        util::Visualizer vis;

        if (algorithm == "berclaz")
            vis.Display(tracks, images_folder, "Visualizer",
                        0, 24, berclaz_params.h_res, berclaz_params.v_res);
        else
            vis.Display(tracks, images_folder);
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
//    boost::add_edge(vertices[1], vertices[0], 1.0, graph);
//
//    // AC
//    boost::add_edge(vertices[0], vertices[2], 2.0, graph);
//    boost::add_edge(vertices[2], vertices[0], 2.0, graph);
//
//    // BD
//    boost::add_edge(vertices[1], vertices[3], 1.0, graph);
//    boost::add_edge(vertices[3], vertices[1], 1.0, graph);
//
//    // BE
//    boost::add_edge(vertices[1], vertices[4], 2.0, graph);
//    boost::add_edge(vertices[4], vertices[1], 2.0, graph);
//
//    // CD
//    boost::add_edge(vertices[2], vertices[3], 2.0, graph);
//    boost::add_edge(vertices[3], vertices[2], 2.0, graph);
//
//    // DF
//    boost::add_edge(vertices[3], vertices[5], 1.0, graph);
//    boost::add_edge(vertices[5], vertices[3], 1.0, graph);
//
//    // EF
//    boost::add_edge(vertices[4], vertices[5], 2.0, graph);
//    boost::add_edge(vertices[5], vertices[4], 2.0, graph);
//
//    source = vertices[0];
//    sink = vertices[5];

    // Create test graph
    std::vector<Vertex> vertices;
    for (size_t i = 0; i < 11; ++i)
    {
        vertices.push_back(
                boost::add_vertex(
                        core::ObjectDataPtr(new core::ObjectData(i)),graph));
    }

//    boost::add_edge(vertices[0], vertices[1], 0.0, graph);
//    boost::add_edge(vertices[0], vertices[8], 0.0, graph);
//    boost::add_edge(vertices[0], vertices[4], 0.0, graph);
//    boost::add_edge(vertices[1], vertices[2], -1.0, graph);
//    boost::add_edge(vertices[1], vertices[5], -1.0, graph);
//    boost::add_edge(vertices[2], vertices[3], -1.0, graph);
//    boost::add_edge(vertices[2], vertices[6], -1.0, graph);
//    boost::add_edge(vertices[2], vertices[10], -1.0, graph);
//    boost::add_edge(vertices[3], vertices[7], 4.0, graph);
//    boost::add_edge(vertices[4], vertices[2], 1.0, graph);
//    boost::add_edge(vertices[4], vertices[5], 1.0, graph);
//    boost::add_edge(vertices[4], vertices[9], 1.0, graph);
//    boost::add_edge(vertices[5], vertices[6], 2.0, graph);
//    boost::add_edge(vertices[5], vertices[3], 2.0, graph);
//    boost::add_edge(vertices[6], vertices[7], 4.0, graph);
//    boost::add_edge(vertices[8], vertices[2], -3.0, graph);
//    boost::add_edge(vertices[8], vertices[9], -3.0, graph);
//    boost::add_edge(vertices[9], vertices[3], 3.0, graph);
//    boost::add_edge(vertices[9], vertices[10], 3.0, graph);
//    boost::add_edge(vertices[10], vertices[7], 4.0, graph);

    source = vertices[0];
    sink = vertices[10];

    for (int i = 1; i < vertices.size() - 1; ++i)
    {
        boost::add_edge(source, vertices[i], 0.0, graph);
    }

    boost::add_edge(vertices[1], vertices[4], -1.0, graph);
    boost::add_edge(vertices[1], vertices[5], -1.0, graph);
    boost::add_edge(vertices[1], vertices[10], 0.0, graph);
    boost::add_edge(vertices[4], vertices[7], -1.0, graph);
    boost::add_edge(vertices[4], vertices[8], -1.0, graph);
    boost::add_edge(vertices[4], vertices[10], 0.0, graph);
    boost::add_edge(vertices[7], vertices[10], -1.0, graph);

    boost::add_edge(vertices[2], vertices[4], -2.0, graph);
    boost::add_edge(vertices[2], vertices[5], -2.0, graph);
    boost::add_edge(vertices[2], vertices[6], -2.0, graph);
    boost::add_edge(vertices[2], vertices[10], 0.0, graph);
    boost::add_edge(vertices[5], vertices[7], -2.0, graph);
    boost::add_edge(vertices[5], vertices[8], -2.0, graph);
    boost::add_edge(vertices[5], vertices[9], -2.0, graph);
    boost::add_edge(vertices[5], vertices[10], 0.0, graph);
    boost::add_edge(vertices[8], vertices[10], -2.0, graph);

    boost::add_edge(vertices[3], vertices[5], -3.0, graph);
    boost::add_edge(vertices[3], vertices[6], -3.0, graph);
    boost::add_edge(vertices[3], vertices[10], 0.0, graph);
    boost::add_edge(vertices[6], vertices[8], -3.0, graph);
    boost::add_edge(vertices[6], vertices[9], -3.0, graph);
    boost::add_edge(vertices[6], vertices[10], 0.0, graph);
    boost::add_edge(vertices[9], vertices[10], -3.0, graph);


//     Connect all with source and sink
//    boost::add_edge(vertices[1], sink, 0, graph);
//    boost::add_edge(source, vertices[2], 0, graph);
//    boost::add_edge(vertices[2], sink, 0, graph);
//    boost::add_edge(source, vertices[3], 0, graph);
//    boost::add_edge(vertices[4], sink, 0, graph);
//    boost::add_edge(source, vertices[5], 0, graph);
//    boost::add_edge(vertices[5], sink, 0, graph);
//    boost::add_edge(source, vertices[6], 0, graph);
//    boost::add_edge(vertices[8], sink, 0, graph);
//    boost::add_edge(source, vertices[9], 0, graph);
//    boost::add_edge(vertices[9], sink, 0, graph);
//    boost::add_edge(source, vertices[10], 0, graph);

//    boost::add_edge(vertices[1], vertices[7], 0.0, graph);
//    boost::add_edge(vertices[8], vertices[7], 0.0, graph);
}

void TestKSP(DirectedGraph graph, Vertex source, Vertex sink, size_t n_paths)
{
    util::FileIO::WriteCSVMatlab(graph, "/home/wrede/Dokumente/graph_ksp.csv");

    algo::KShortestPaths2 ksp;
    MultiPredecessorMap paths = ksp.Run(graph, source, sink, n_paths);

    util::FileIO::WriteCSVMatlab(paths, source, sink, "/home/wrede/Dokumente/paths_ksp.csv");
}

void TestKBellmanFord(DirectedGraph graph, Vertex source, Vertex sink, size_t n_paths)
{
    util::FileIO::WriteCSVMatlab(graph, "/home/wrede/Dokumente/graph_kbf.csv");

    MultiPredecessorMap paths;
    for (size_t i = 0; i < n_paths; ++i)
    {
        // Prepare variables for path finding
        size_t graph_size = boost::num_vertices(graph);
        std::vector<Vertex> pred_list(graph_size);
        std::vector<double> dist_list(graph_size);
        VertexIndexMap graph_indices = boost::get(boost::vertex_index, graph);
        EdgeWeightMap weight_map = boost::get(boost::edge_weight, graph);
        PredecessorMap pred_map(&pred_list[0], graph_indices);
        DistanceMap dist_map(&dist_list[0], graph_indices);

        // Find the shortest path
        boost::bellman_ford_shortest_paths(graph, graph_size,
                                           boost::root_vertex(source)
                                                   .weight_map(weight_map)
                                                   .predecessor_map(pred_map)
                                                   .distance_map(dist_map));

        // Add path
        for (Vertex u = sink, v = pred_map[u]; u != v; u = v, v = pred_map[v])
        {
            paths[u].insert(v);

            if (u != sink && u != source)
                boost::clear_out_edges(u, graph);
        }
    }

    util::FileIO::WriteCSVMatlab(paths, source, sink, "/home/wrede/Dokumente/paths_kbf.csv");
}

void TestGrid()
{
    int lower_index = 0;
    int upper_index = 5;
    double lower_bound = 0.0;
    double upper_bound = 50.0;
    util::Grid grid(upper_index, upper_index, upper_index,
                    upper_bound, upper_bound, upper_bound);

    std::uniform_int_distribution<int> unii(lower_index, upper_index - 1);
    std::uniform_real_distribution<double> unif(lower_bound, upper_bound);
    std::default_random_engine re;

    // Fill with empty values
    std::cout << "fill with empty values\n";
    for (int z = lower_index; z < upper_index; ++z)
    {
        for (int y = lower_index; y < upper_index; ++y)
        {
            for (int x = lower_index; y < upper_index; ++y)
            {
                grid.SetValue(nullptr, x, y, z);
            }
        }
    }

    // Randomly add data
    std::cout << "randomly add data\n";
    for (int i = 0; i < 10; ++i)
    {
        int xi = unii(re);
        int yi = unii(re);
        int zi = unii(re);

        core::ObjectDataPtr value(new core::ObjectData((size_t)i));
        grid.SetValue(value, xi, yi, zi);

        std::cout << xi << "," << yi << "," << zi << " = " << *value << std::endl;
    }

    // Randomly get data
    std::cout << "randomly get data\n";
    for (int i = 0; i < 10; ++i)
    {
        double x = unif(re);
        double y = unif(re);
        double z = unif(re);

        std::cout << x << "," << y << "," << z << " = ";
        core::ObjectDataPtr value = grid.GetValue(x, y, z);
        if (value)
        {
            std::cout << *value << std::endl;
        }
        else
        {
            std::cout << "nullptr" << std::endl;
        }
    }
}

void CreateBerclazGraph(DirectedGraph& graph, Vertex& source, Vertex& sink)
{
    util::Logger::SetDebug(true);
    util::Logger::SetInfo(true);
    util::Logger::LogInfo("Test berclaz graph");

    // Init grid with data
    util::Grid grid(3, 3, 3, 9.0, 9.0, 9.0);
    for (int z = 0; z < grid.GetDepthCount(); ++z)
    {
        for (int y = 0; y < grid.GetHeightCount(); ++y)
        {
            for (int x = 0; x < grid.GetWidthCount(); ++x)
            {
                core::ObjectDataPtr value(new core::ObjectData(10));
                grid.SetValue(value, x, y, z);
            }
        }
    }

    // Add path source->0,0,0->0,0,1->0,0,2->sink
    core::ObjectDataPtr value0(new core::ObjectData(1));
    value0->SetDetectionScore(1.0);
    grid.SetValue(value0, 0, 0, 0);
    core::ObjectDataPtr value1(new core::ObjectData(2));
    value1->SetDetectionScore(1.0);
    grid.SetValue(value1, 0, 0, 1);
    core::ObjectDataPtr value2(new core::ObjectData(3));
    value2->SetDetectionScore(1.0);
    grid.SetValue(value2, 0, 0, 2);

    // Add path source->0,1,0->0,1,1->0,1,2->sink
    core::ObjectDataPtr value3(new core::ObjectData(4));
    value3->SetDetectionScore(0.6);
    grid.SetValue(value3, 0, 1, 0);
    core::ObjectDataPtr value4(new core::ObjectData(5));
    value4->SetDetectionScore(0.6);
    grid.SetValue(value4, 0, 1, 1);
    core::ObjectDataPtr value5(new core::ObjectData(6));
    value5->SetDetectionScore(0.6);
    grid.SetValue(value5, 0, 1, 2);

    // Add path source->0,2,0->0,2,1->0,2,2->sink
    core::ObjectDataPtr value6(new core::ObjectData(7));
    value6->SetDetectionScore(0.55);
    grid.SetValue(value6, 0, 2, 0);
    core::ObjectDataPtr value7(new core::ObjectData(8));
    value7->SetDetectionScore(0.55);
    grid.SetValue(value7, 0, 2, 1);
    core::ObjectDataPtr value8(new core::ObjectData(9));
    value8->SetDetectionScore(0.55);
    grid.SetValue(value8, 0, 2, 2);

    util::Logger::LogDebug("add vertices");

    // Add grid vertices
    graph.clear();
    for (int z = 0; z < grid.GetDepthCount(); ++z)
    {
        for (int y = 0; y < grid.GetHeightCount(); ++y)
        {
            for (int x = 0; x < grid.GetWidthCount(); ++x)
            {
                boost::add_vertex(grid.GetValue(x, y, z), graph);
            }
        }
    }

    util::Logger::LogDebug("vertex count " + std::to_string(boost::num_vertices(graph)));
    util::Logger::LogDebug("edge count " + std::to_string(boost::num_edges(graph)));

    // Add source and sink vertex
    source = boost::add_vertex(core::ObjectDataPtr(new core::ObjectData()), graph);
    sink = boost::add_vertex(core::ObjectDataPtr(new core::ObjectData()), graph);

    util::Logger::LogDebug("add edges");

    // Iterate all vertices but source and sink
    VertexIndexMap vertices = boost::get(boost::vertex_index, graph);
    VertexValueMap values = boost::get(boost::vertex_name, graph);
    int vicinity_size = 1;
    int layer_size = grid.GetWidthCount() * grid.GetHeightCount();
    for (int z = 0; z < grid.GetDepthCount(); ++z)
    {
        for (int y = 0; y < grid.GetHeightCount(); ++y)
        {
            for (int x = 0; x < grid.GetWidthCount(); ++x)
            {
                // First vertex index
                int vi = x + y * grid.GetHeightCount() + z * layer_size;

                // Get the score, clamp it, prevent division by zero and
                // logarithm of zero
                double score = values[vi]->GetDetectionScore();
                if (score > 0.999999)
                {
                    score = 0.999999;
                }
                else if (score < 0.000001)
                {
                    score = 0.000001;
                }

                // Calculate the edge weight
                double weight = -std::log(score / (1 - score));

                // Connect with the next frame only if there is a next frame
                if (z < grid.GetDepthCount() - 1)
                {
                    // Iterate all nearby cells in the next frame
                    for (int ny = std::max(0, y - vicinity_size);
                         ny <
                         std::min(grid.GetHeightCount(), y + vicinity_size + 1);
                         ++ny)
                    {
                        for (int nx = std::max(0, x - vicinity_size);
                             nx < std::min(grid.GetWidthCount(),
                                           x + vicinity_size + 1);
                             ++nx)
                        {
                            // Second vertex index
                            int vj = nx + ny * grid.GetHeightCount() +
                                     (z + 1) * layer_size;

                            // Connect to nearby cells
                            boost::add_edge(vertices[vi], vertices[vj],
                                            weight, graph);
                        }
                    }

                    boost::add_edge(vertices[vi], sink, 0.0, graph);
                }
                else
                {
                    boost::add_edge(vertices[vi], sink, weight, graph);
                }

                // Connect with source and sink
                boost::add_edge(source, vertices[vi], 0.0, graph);
            }
        }
    }

    util::Logger::LogDebug("vertex count " + std::to_string(boost::num_vertices(graph)));
    util::Logger::LogDebug("edge count " + std::to_string(boost::num_edges(graph)));
}

void CreatePresentationGraph(DirectedGraph& graph, Vertex& source, Vertex& sink)
{
    std::vector<Vertex> vertices;
    for (size_t i = 0; i < 10; ++i)
    {
        vertices.push_back(boost::add_vertex(core::ObjectDataPtr(new core::ObjectData(i)), graph));
    }

    source = vertices[0];
    sink = vertices[9];

    boost::add_edge(vertices[0], vertices[1], 0.0, graph);
    boost::add_edge(vertices[0], vertices[2], 0.0, graph);
    boost::add_edge(vertices[1], vertices[3], 12.0, graph);
    boost::add_edge(vertices[1], vertices[4], 15.0, graph);
    boost::add_edge(vertices[2], vertices[3], 15.0, graph);
    boost::add_edge(vertices[2], vertices[4], 10.0, graph);
    boost::add_edge(vertices[3], vertices[5], 15.0, graph);
    boost::add_edge(vertices[3], vertices[6], 12.0, graph);
    boost::add_edge(vertices[4], vertices[5], 12.0, graph);
    boost::add_edge(vertices[4], vertices[6], 11.0, graph);
    boost::add_edge(vertices[5], vertices[7], 12.0, graph);
    boost::add_edge(vertices[5], vertices[8], 12.0, graph);
    boost::add_edge(vertices[6], vertices[7], 11.0, graph);
    boost::add_edge(vertices[6], vertices[8], 10.0, graph);
    boost::add_edge(vertices[7], vertices[9], 0.0, graph);
    boost::add_edge(vertices[8], vertices[9], 0.0, graph);

//    boost::add_edge(vertices[0], vertices[1], 0.0, graph);
//    boost::add_edge(vertices[0], vertices[2], 0.0, graph);
//    boost::add_edge(vertices[1], vertices[3], 12.0, graph);
//    boost::add_edge(vertices[1], vertices[4], 15.0, graph);
//    boost::add_edge(vertices[2], vertices[3], 15.0, graph);
//    boost::add_edge(vertices[2], vertices[4], 10.0, graph);
//    boost::add_edge(vertices[3], vertices[5], 12.0, graph);
//    boost::add_edge(vertices[3], vertices[6], 12.0, graph);
//    boost::add_edge(vertices[4], vertices[5], 11.0, graph);
//    boost::add_edge(vertices[4], vertices[6], 10.0, graph);
//    boost::add_edge(vertices[5], vertices[7], 15.0, graph);
//    boost::add_edge(vertices[5], vertices[8], 12.0, graph);
//    boost::add_edge(vertices[6], vertices[7], 12.0, graph);
//    boost::add_edge(vertices[6], vertices[8], 11.0, graph);
//    boost::add_edge(vertices[7], vertices[9], 0.0, graph);
//    boost::add_edge(vertices[8], vertices[9], 0.0, graph);
}

void TestSuurballe()
{
    // Create the graph
    Vertex source, sink;
    DirectedGraph graph;
    std::vector<Vertex> vertices;

    // First example graph
//    for (int i = 0; i < 7; ++i)
//    {
//        vertices.push_back(boost::add_vertex(graph));
//    }
//    source = vertices[0];
//    sink = vertices[6];
//    boost::add_edge(vertices[0], vertices[1], 5.0, graph);
//    boost::add_edge(vertices[0], vertices[4], 2.0, graph);
//    boost::add_edge(vertices[1], vertices[2], 1.0, graph);
//    boost::add_edge(vertices[1], vertices[4], 1.0, graph);
//    boost::add_edge(vertices[2], vertices[6], 1.0, graph);
//    boost::add_edge(vertices[3], vertices[2], 1.0, graph);
//    boost::add_edge(vertices[4], vertices[3], 2.0, graph);
//    boost::add_edge(vertices[4], vertices[5], 1.0, graph);
//    boost::add_edge(vertices[5], vertices[2], 1.0, graph);
//    boost::add_edge(vertices[5], vertices[6], 1.0, graph);

    // Second example graph
    for (int i = 0; i < 8; ++i)
    {
        vertices.push_back(boost::add_vertex(graph));
    }
    source = vertices[0];
    sink = vertices[7];
    boost::add_edge(vertices[0], vertices[1], 1.0, graph);
    boost::add_edge(vertices[0], vertices[4], 8.0, graph);
    boost::add_edge(vertices[0], vertices[5], 1.0, graph);
    boost::add_edge(vertices[1], vertices[2], 1.0, graph);
    boost::add_edge(vertices[1], vertices[7], 8.0, graph);
    boost::add_edge(vertices[2], vertices[3], 1.0, graph);
    boost::add_edge(vertices[3], vertices[4], 1.0, graph);
    boost::add_edge(vertices[3], vertices[6], 2.0, graph);
    boost::add_edge(vertices[4], vertices[7], 1.0, graph);
    boost::add_edge(vertices[5], vertices[2], 2.0, graph);
    boost::add_edge(vertices[5], vertices[6], 6.0, graph);
    boost::add_edge(vertices[6], vertices[7], 1.0, graph);

    algo::KShortestPaths4 suurballe(graph, source, sink, 4);
    suurballe.Run();

    size_t i = 0;
    for (std::vector<Vertex> path : suurballe.GetPaths())
    {
        i++;
        std::cout << "P" << i << "{";
        for (Vertex v : path)
        {
            std::cout << v;
        }
        std::cout << "}\n";
    }

    std::cout << "Total paths length: " << suurballe.GetTotalPathsLength() << std::endl;
}

int main(int argc, char** argv)
{
    //TODO load with frame offset

//    Run(argc, argv);

//    TestTracklet();

//    DirectedGraph graph;
//    Vertex source, sink;
//    CreatePresentationGraph(graph, source, sink);
//
//    algo::KShortestPaths4 suurballe(graph, source, sink, 4);
//    suurballe.Run();
//    size_t i = 0;
//    for (std::vector<Vertex> path : suurballe.GetPaths())
//    {
//        i++;
//        std::cout << "P" << i << "{";
//        for (Vertex v : path)
//        {
//            std::cout << v;
//        }
//        std::cout << "}\n";
//    }
//    std::cout << "Total paths length: " << suurballe.GetTotalPathsLength() << std::endl;
//
//    util::FileIO::WriteCSVMatlab(graph, "/home/wrede/Dokumente/graph.csv");
//
//    TestKBellmanFord(graph, source, sink, 2);
//    TestKSP(graph, source, sink, 3);

//    TestGrid();

    TestSuurballe();

    return 0;
}