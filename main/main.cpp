//
// Created by wrede on 19.04.16.
//
//
#include "../core/Definitions.h"
#include "../core/DetectionSequence.h"
#include "../util/IO.h"
#include "../util/Parser.h"
#include "../algo/TwoStage.h"

int main(void)
{
    std::cout << "Initializing values\n";
    const char delimiter = ';';
    const std::string full_file("/home/wrede/Dokumente/tmt_detections.csv");
    const std::string test_file("/home/wrede/Dokumente/test.csv");
    const std::string sequence_name("TMT_Detections_Raw");
    size_t max_frame_skip = 1;
    double penalty_value = 20.0;
    size_t max_tracklet_count = 1;
    std::vector<std::string> keys;
    core::Vector3d values;

    keys.push_back("angle");
    keys.push_back("score");
    keys.push_back("x");
    keys.push_back("y");

    std::cout << "Initializing sequence\n";
    core::DetectionSequence sequence(sequence_name);

    std::cout << "Reading CSV file\n";
    util::io::ReadCSV(full_file, delimiter, values);

    std::cout << "Parsing data\n";
    util::parser::ParseObjectDataMap(keys, values, sequence);

    std::cout << "Initializing algorithm\n";
    algo::TwoStage two_stage(max_frame_skip, penalty_value, max_tracklet_count);

    std::cout << "Creating object graph\n";
    two_stage.CreateObjectGraph(sequence);

    std::cout << "Creating tracklet graph twice\n";
    two_stage.CreateTrackletGraph(two_stage.CreateTrackletGraph(), sequence.GetFrameCount());

    std::cout << "Extracting final paths\n";
    two_stage.ExtractTracks();

    std::cout << "Finished successfully\n" << std::flush;

    return 0;
}