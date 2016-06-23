//
// Created by wrede on 02.06.16.
//

#include "Berclaz.h"
#include "KShortestPaths.h"
#include "../util/Parser.h"
#include "../util/Logger.h"
#include "../util/FileIO.h"
#include "KShortestPaths2.h"

namespace algo
{
    Berclaz::Berclaz(int h_res, int v_res, int vicinity_size)
    {
        h_res_ = h_res;
        v_res_ = v_res;
        vicinity_size_ = vicinity_size;
    }

    void Berclaz::CreateGraph(DirectedGraph& graph, Vertex& source, Vertex& sink, util::Grid& grid)
    {
        util::Logger::LogDebug("add vertices");

        // Add grid vertices
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

        util::Logger::LogDebug("source index: " + std::to_string(source));
        util::Logger::LogDebug("sink index: " + std::to_string(sink));
        util::Logger::LogDebug("add edges");

        // Iterate all vertices but source and sink
        VertexIndexMap vertices = boost::get(boost::vertex_index, graph);
        VertexValueMap values = boost::get(boost::vertex_name, graph);
        int layer_size = grid.GetWidthCount() * grid.GetHeightCount();
        for (int z = 0; z < grid.GetDepthCount(); ++z)
        {
            for (int y = 0; y < grid.GetHeightCount(); ++y)
            {
                for (int x = 0; x < grid.GetWidthCount(); ++x)
                {
                    // First vertex index
                    int vi = x + y * grid.GetWidthCount() + z * layer_size;

                    // Get the score, clamp it, prevent division by zero and
                    // logarithm of zero
                    double score = values[vi]->GetDetectionScore();
                    if (score > MAX_SCORE_VALUE)
                    {
                        score = MAX_SCORE_VALUE;
                    }
                    else if (score < MIN_SCORE_VALUE)
                    {
                        score = MIN_SCORE_VALUE;
                    }

                    // Calculate the edge weight
                    double weight = -std::log(score / (1 - score));

                    // Connect with the next frame only if there is a next frame
                    if (z < grid.GetDepthCount() - 1)
                    {
                        // Iterate all nearby cells in the next frame
                        for (int ny = std::max(0, y - vicinity_size_);
                             ny < std::min(grid.GetHeightCount(), y + vicinity_size_ + 1);
                             ++ny)
                        {
                            for (int nx = std::max(0, x - vicinity_size_);
                                 nx < std::min(grid.GetWidthCount(), x + vicinity_size_ + 1);
                                 ++nx)
                            {
                                // Second vertex index
                                int vj = nx + ny * grid.GetWidthCount() + (z + 1) * layer_size;

                                // Connect to nearby cells
                                boost::add_edge(vertices[vi], vertices[vj], weight, graph);
                            }
                        }

                        boost::add_edge(vertices[vi], sink, VIRTUAL_EDGE_WEIGHT, graph);
                    }
                    else
                    {
                        boost::add_edge(vertices[vi], sink, weight, graph);
                    }

                    // Connect with source and sink
                    boost::add_edge(source, vertices[vi], VIRTUAL_EDGE_WEIGHT, graph);
                }
            }
        }

        util::Logger::LogDebug("vertex count " + std::to_string(boost::num_vertices(graph)));
        util::Logger::LogDebug("calculated vertex count " + std::to_string(
           grid.GetWidthCount() * grid.GetHeightCount() * grid.GetDepthCount() + 2
        ));
        util::Logger::LogDebug("edge count " + std::to_string(boost::num_edges(graph)));
        util::Logger::LogDebug("width count " + std::to_string(grid.GetWidthCount()));
        util::Logger::LogDebug("height count " + std::to_string(grid.GetHeightCount()));
        util::Logger::LogDebug("depth count " + std::to_string(grid.GetDepthCount()));
        util::Logger::LogDebug("calculated edge count " + std::to_string(
           grid.GetWidthCount() * grid.GetHeightCount() * (grid.GetDepthCount() - 1) * 11 +
           grid.GetWidthCount() * grid.GetHeightCount() * 2 -
           (grid.GetWidthCount() + grid.GetHeightCount()) * 2 * 3 * (grid.GetDepthCount() - 1) +
           4 * (grid.GetDepthCount() - 1)
        ));
    }

    void Berclaz::ExtractTracks(DirectedGraph& graph, MultiPredecessorMap& map, Vertex origin,
                                std::vector<core::TrackletPtr>& tracks)
    {
        VertexValueMap values = boost::get(boost::vertex_name, graph);

        // Move along all paths in reverse, starting at the origin
        for (Vertex first : map[origin])
        {
            core::TrackletPtr tracklet(new core::Tracklet());

            // The paths are node disjoint, so there should always be only one
            // node to proceed to
            for (Vertex u = first, v = (*map[u].begin());
                 u != v; u = v, v = (*map[v].begin()))
            {
                tracklet->AddPathObject(values[u]);
            }

            tracks.push_back(tracklet);
        }
    }

    void Berclaz::Run(core::DetectionSequence& sequence,
                      size_t batch_size, size_t max_track_count,
                      std::vector<core::TrackletPtr>& tracks)
    {
        for (size_t i = 0; i < sequence.GetFrameCount(); i += batch_size)
        {
            util::Logger::LogDebug("batch offset: " + std::to_string(i));

            util::Grid grid = util::Parser::ParseGrid(sequence, i, i + batch_size,
                                                      0.0, 1.0, h_res_, 0.0, 1.0, v_res_);

            util::Logger::LogDebug("create graph");
            DirectedGraph graph;
            Vertex source, sink;
            CreateGraph(graph, source, sink, grid);

            util::Logger::LogDebug("run ksp");
            KShortestPaths2 ksp;
            MultiPredecessorMap ksp_result = ksp.Run(graph, source, sink, max_track_count);

            util::Logger::LogDebug("extract tracks");
            ExtractTracks(graph, ksp_result, sink, tracks);
        }

        // Only connect tracks if the sequence was split
        if (batch_size < sequence.GetFrameCount())
        {
            //TODO find a better way to connect tracks (n-stage)
            util::Logger::LogDebug("connect tracks");
            ConnectTracks(tracks);
        }
    }

    void Berclaz::ConnectTracks(std::vector<core::TrackletPtr>& tracks)
    {
        for (size_t i = 0; i < tracks.size(); ++i)
        {
            // find the best matching tracklet
            double best_value = std::numeric_limits<double>::max();
            size_t best_index = 0;
            for (size_t k = i + 1; k < tracks.size(); ++k)
            {
                if (tracks[i]->GetLastFrameIndex() < tracks[k]->GetFirstFrameIndex())
                {
                    double value = tracks[i]->CompareTo(tracks[k]);
                    if (value < best_value)
                    {
                        best_index = k;
                    }
                }
            }

            // if a match was found
            if (best_index != 0)
            {
                // merge the two tracks
                tracks[i]->Combine(tracks[best_index]);
                tracks.erase(tracks.begin() + best_index);
            }
        }
    }
}