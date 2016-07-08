//
// Created by wrede on 25.04.16.
//

#include "NStage.h"
#include "../util/Logger.h"
#include <boost/graph/dijkstra_shortest_paths.hpp>

namespace algo
{
    NStage::NStage(std::vector<size_t> max_frame_skip,
                   std::vector<double> penalty_value,
                   std::vector<size_t> max_tracklet_count)
    {
        max_frame_skips_ = max_frame_skip;
        penalty_values_ = penalty_value;
        max_tracklet_counts_ = max_tracklet_count;
        iterations_ = std::min(max_tracklet_count.size(), penalty_value.size());
    }

    void NStage::CreateObjectGraph(DirectedGraph& graph, const core::DetectionSequence& detections)
    {
        util::Logger::LogInfo("Creating object graph");

        std::vector<std::vector<Vertex>> layers;

        // Add source as the vertex with the lowest index
        Vertex source = boost::add_vertex(core::ObjectDataPtr(new core::ObjectData()), graph);

        // Add vertices from detection sequence to directed graph
        // Save the vertices which are in one frame/layer for later use to
        // link easily between vertices in adjacent frames/layers
        for (size_t i = 0; i < detections.GetFrameCount(); ++i)
        {
            std::vector<Vertex> layer;

            for (size_t j = 0; j < detections.GetObjectCount(i); ++j)
            {
                Vertex v = boost::add_vertex(detections.GetObject(i, j), graph);

                layer.push_back(v);
            }

            layers.push_back(layer);
        }

        // Add sink as the vertex with the highest index
        Vertex sink = boost::add_vertex(core::ObjectDataPtr(new core::ObjectData()), graph);

        // Vertex objects
        VertexValueMap values = boost::get(boost::vertex_name, graph);

        // Create edges
        for (size_t i = 0; i < layers.size(); ++i)
        {
            // For each edge in this frame/layer
            for (size_t j = 0; j < layers[i].size(); ++j)
            {
                Vertex u = layers[i][j];

                // For each next frame/layer until maxFrameSkip or end
                for (size_t k = 1; k <= max_frame_skips_[0] && i + k < layers.size(); ++k)
                {
                    // To every edge in the next frame/layer
                    for (size_t l = 0; l < layers[i + k].size(); ++l)
                    {
                        Vertex v = layers[i + k][l];

                        boost::add_edge(u, v,
                                        values[u]->CompareTo(values[v]),
                                        graph);
                    }
                }

                // From source to vertex and from vertex to sink
                boost::add_edge(source, u,
                                (i + 1) * penalty_values_[0],
                                graph);

                boost::add_edge(u, sink,
                                (layers.size() - i) * penalty_values_[0],
                                graph);
            }
        }

        util::Logger::LogDebug("vertex count " + std::to_string(boost::num_vertices(graph)));
        util::Logger::LogDebug("edge count " + std::to_string(boost::num_edges(graph)));
    }

    void NStage::CreateTrackletGraph(DirectedGraph& obj_graph, DirectedGraph& tlt_graph,
                                     size_t frame_count, size_t iteration)
    {
        util::Logger::LogInfo("Creating tracklet graph");

        // Add source to tracklet graph
        Vertex tlt_src = boost::add_vertex(
                core::ObjectDataPtr(new core::ObjectData()), tlt_graph);

        // Prepare variables for dijkstra
        size_t obj_graph_size = boost::num_vertices(obj_graph);
        std::vector<Vertex> obj_pred_list(obj_graph_size);
        std::vector<double> obj_dist_list(obj_graph_size);
        VertexIndexMap obj_indices = boost::get(boost::vertex_index, obj_graph);
        VertexValueMap obj_values = boost::get(boost::vertex_name, obj_graph);
        PredecessorMap obj_pred_map(&obj_pred_list[0], obj_indices);
        DistanceMap obj_dist_map(&obj_dist_list[0], obj_indices);

        // Source and sink of the object graph
        Vertex obj_src = obj_indices[0];
        Vertex obj_snk = obj_indices[obj_graph_size - 1];

        //TODO experimental
        EdgeWeightMap weight_map = boost::get(boost::edge_weight, obj_graph);

        // Iteratively run dijkstra to extract tracklets
        for (size_t i = 0; i != max_tracklet_counts_[iteration]; ++i)
        {
            util::Logger::LogDebug("tracklet iteration: " + std::to_string(i));

            boost::dijkstra_shortest_paths(obj_graph, obj_src,
                                           boost::predecessor_map(obj_pred_map)
                                                   .distance_map(obj_dist_map));

            // No path from source to sink could be found
            if (obj_dist_map[obj_snk] == std::numeric_limits<double>::max())
            {
                break;
            }

            // Create the tracklet
            core::TrackletPtr tracklet(new core::Tracklet);
            for (Vertex u = obj_pred_map[obj_snk], v = obj_snk; u != v; v = u, u = obj_pred_map[v])
            {
                tracklet->AddPathObject(obj_values[u]);

                // Leave source and sink untouched
                if (!obj_values[u]->IsVirtual())
                {
                    //TODO original
                    // Remove the path by setting all used edges to a weight of
                    // infinity
//                    std::pair<DirectedGraph::out_edge_iterator,
//                              DirectedGraph::out_edge_iterator> edge_iter = boost::out_edges(u, obj_graph);
//
//                    for (DirectedGraph::out_edge_iterator iter = edge_iter.first;
//                         iter != edge_iter.second;
//                         ++iter)
//                    {
//                        boost::get(boost::edge_weight, obj_graph, *iter)
//                                = std::numeric_limits<double>::infinity();
//                    }

                    //TODO experimental
                    OutEdgeIter oei, oei_end;
                    for (boost::tie(oei, oei_end) = boost::out_edges(u, obj_graph);
                         oei != oei_end;
                         ++oei)
                    {
                        weight_map[*oei] = std::numeric_limits<double>::infinity();
                    }
                }
            }
            core::ObjectDataPtr tracklet_base = tracklet;

            // Add tracklet into tracklet graph
            boost::add_vertex(tracklet_base, tlt_graph);
        }

        // Add sink to tracklet graph
        Vertex tlt_snk = boost::add_vertex(core::ObjectDataPtr(new core::ObjectData()),
                                  tlt_graph);

        util::Logger::LogDebug("adding edges");

        // Create edges
        size_t tlt_graph_size = boost::num_vertices(tlt_graph);
        VertexIndexMap tlt_indices = boost::get(boost::vertex_index, tlt_graph);
        VertexValueMap tlt_values = boost::get(boost::vertex_name, tlt_graph);

        // For every tracklet but source and sink
        for (size_t i = 1; i < tlt_graph_size - 1; ++i)
        {
            Vertex u = tlt_indices[i];
            core::TrackletPtr u_ptr = std::static_pointer_cast<core::Tracklet>(tlt_values[u]);
            size_t u_first_frame = u_ptr->GetFirstFrameIndex();
            size_t u_last_frame = u_ptr->GetLastFrameIndex();

            // Create edges between tracklets
            for (size_t j = 1; j < tlt_graph_size - 1; ++j)
            {
                if (i != j)
                {
                    Vertex v = tlt_indices[j];
                    core::TrackletPtr v_ptr =
                            std::static_pointer_cast<core::Tracklet>(tlt_values[v]);
                    size_t v_first_frame = v_ptr->GetFirstFrameIndex();

                    // Link only tracklets that are in temporal order
                    if (u_last_frame < v_first_frame &&
                            (v_first_frame - u_last_frame < max_frame_skips_[iteration]))
                    {
                        boost::add_edge(u, v,
                                        tlt_values[u]->CompareTo(tlt_values[v]),
                                        tlt_graph);
                    }
                }
            }

            // From source
            boost::add_edge(tlt_src, u,
                            (u_first_frame + 1) * penalty_values_[iteration],
                            tlt_graph);

            // To sink
            boost::add_edge(u, tlt_snk,
                            (frame_count - u_last_frame) * penalty_values_[iteration],
                            tlt_graph);
        }

        util::Logger::LogDebug("vertex count " + std::to_string(boost::num_vertices(tlt_graph)));
        util::Logger::LogDebug("edge count " + std::to_string(boost::num_edges(tlt_graph)));
    }

    void NStage::ExtractTracks(DirectedGraph& tlt_graph, size_t depth,
                               std::vector<core::TrackletPtr>& tracks)
    {
        util::Logger::LogInfo("Extracting tracks");

        VertexValueMap tlt_values = boost::get(boost::vertex_name, tlt_graph);
        for (size_t i = 0; i < boost::num_vertices(tlt_graph); ++i)
        {
            core::ObjectDataPtr obj = tlt_values[i];
            if (!obj->IsVirtual())
            {
                core::TrackletPtr tlt = std::static_pointer_cast<core::Tracklet>(obj);

                for (size_t j = 0; j < depth; j++)
                {
                    tlt->Flatten();
                }

                tracks.push_back(tlt);
            }
        }

        util::Logger::LogDebug("track count " + std::to_string(tracks.size()));
    }

    void NStage::Run(const core::DetectionSequence& sequence,
                     std::vector<core::TrackletPtr>& tracks)
    {
        // Running the two stage graph algorithm
        DirectedGraph obj_graph;
        CreateObjectGraph(obj_graph, sequence);

        // Run the tracklet creation at least once
        DirectedGraph tlt_graph_1, tlt_graph_2;
        CreateTrackletGraph(obj_graph, tlt_graph_1, sequence.GetFrameCount(), 0);

        // Run the tracklet creation iteratively
        for (size_t i = 1; i < iterations_; ++i)
        {
            if (i % 2 == 0)
            {
                CreateTrackletGraph(tlt_graph_2, tlt_graph_1, sequence.GetFrameCount(), i);
            }
            else
            {
                CreateTrackletGraph(tlt_graph_1, tlt_graph_2, sequence.GetFrameCount(), i);
            }
        }

        // Extract tracklets and flatten tracklets
        if (iterations_ % 2 == 0)
        {
            ExtractTracks(tlt_graph_2, iterations_ - 1, tracks);
        }
        else
        {
            ExtractTracks(tlt_graph_1, iterations_ - 1, tracks);
        }
    }
}