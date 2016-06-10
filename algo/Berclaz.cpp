//
// Created by wrede on 02.06.16.
//

#include "Berclaz.h"
#include "KShortestPaths.h"
#include "../util/Parser.h"
#include "../util/Logger.h"
#include "../util/FileIO.h"

namespace algo
{
    Berclaz::Berclaz(int h_res, int v_res, int vicinity_size)
    {
        h_res_ = h_res;
        v_res_ = v_res;
        vicinity_size_ = vicinity_size;
    }

    void Berclaz::CreateGraph(DirectedGraph& graph, Vertex& source, Vertex& sink,
                              util::Grid& grid)
    {
        // Add source vertex
        source = boost::add_vertex(core::ObjectDataPtr(new core::ObjectData()), graph);

        // Add grid vertices
        for (int f = 0; f < grid.GetDepthCount(); ++f)
        {
            for (int y = 0; y < v_res_; ++y)
            {
                for (int x = 0; x < h_res_; ++x)
                {
                    boost::add_vertex(grid.GetValue(x, y, f), graph);
                }
            }
        }

        // Add sink vertex
        sink = boost::add_vertex(core::ObjectDataPtr(new core::ObjectData()), graph);
        
        // Store the vertex indices
        VertexIndexMap vertices = boost::get(boost::vertex_index, graph);

        // Store the vertex values
        VertexValueMap values = boost::get(boost::vertex_name, graph);

        util::Logger::LogDebug("num vertices " + std::to_string(boost::num_vertices(graph)));
        
        // Iterate all vertices but source and sink
        for (int f = 0; f < grid.GetDepthCount(); ++f)
        {
            for (int y = 0; y < v_res_; ++y)
            {
                for (int x = 0; x < h_res_; ++x)
                {
                    // First vertex index
                    int vi = x + y * h_res_ + f * h_res_ * v_res_ + 1;
                    double score = values[vi]->GetDetectionScore();
                    if (score > MAX_SCORE_VALUE)
                    {
                        score = MAX_SCORE_VALUE;
                    }
                    else if (score < MIN_SCORE_VALUE)
                    {
                        score = MIN_SCORE_VALUE;
                    }

                    // Iterate all nearby cells in the next frame
                    for (int nx = std::max(0, x - vicinity_size_);
                         nx < std::min(h_res_, x + vicinity_size_ + 1);
                         ++nx)
                    {
                        for (int ny = std::max(0, y - vicinity_size_);
                             ny < std::min(v_res_, y + vicinity_size_ + 1);
                             ++ny)
                        {
                            // Second vertex index
                            int vj = nx + ny * h_res_ + (f + 1) * h_res_ * v_res_ + 1;

                            // Connect to nearby cells
                            double weight = -std::log(score / (1 - score));
                            boost::add_edge(vertices[vi], vertices[vj],
                                            weight, graph);
                        }
                    }
                    
                    // Connect with source and sink
                    boost::add_edge(source, vertices[vi],
                                    VIRTUAL_EDGE_WEIGHT, graph);

                    boost::add_edge(vertices[vi], sink,
                                    VIRTUAL_EDGE_WEIGHT, graph);
                }
            }
        }

        util::Logger::LogDebug("num edges " + std::to_string(boost::num_edges(graph)));
    }

    void Berclaz::ExtractTracks(DirectedGraph& graph,
                                MultiPredecessorMap& map, Vertex origin,
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
                      size_t max_track_count,
                      std::vector<core::TrackletPtr>& tracks)
    {
        Vertex source, sink;
        DirectedGraph graph;
        util::Grid grid = util::Parser::ParseGrid(sequence,
                                                  MIN_H_VALUE,
                                                  MAX_H_VALUE,
                                                  h_res_,
                                                  MIN_V_VALUE,
                                                  MAX_V_VALUE,
                                                  v_res_);

        util::Logger::LogDebug("create graph");
        CreateGraph(graph, source, sink, grid);

        util::FileIO::WriteCSVMatlab(graph, "/home/wrede/Dokumente/graph.csv");

        util::Logger::LogDebug("init ksp");
        KShortestPaths ksp(graph, source, sink);

        util::Logger::LogDebug("run ksp");
        MultiPredecessorMap ksp_result = ksp.Run(max_track_count);

        util::Logger::LogDebug("extract tracks");
        ExtractTracks(graph, ksp_result, sink, tracks);
    }
}