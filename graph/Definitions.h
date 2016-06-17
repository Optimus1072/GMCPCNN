//
// Created by wrede on 27.05.16.
//

#ifndef GBMOT_DEFINITIONS_H
#define GBMOT_DEFINITIONS_H

#include "../core/ObjectData.h"
#include <boost/graph/properties.hpp>
#include <boost/graph/adjacency_list.hpp>

typedef boost::property<boost::edge_weight_t, double> EdgeProp;
typedef boost::property <boost::vertex_name_t, core::ObjectDataPtr>
        VertexProp;
typedef boost::adjacency_list <
        boost::listS, boost::vecS, boost::directedS, VertexProp, EdgeProp>
        DirectedGraph;
typedef boost::graph_traits<DirectedGraph>::edge_descriptor Edge;
typedef boost::graph_traits<DirectedGraph>::vertex_descriptor Vertex;
typedef boost::property_map<DirectedGraph, boost::edge_weight_t>::type
        EdgeWeightMap;
typedef boost::property_map<DirectedGraph, boost::vertex_index_t>::type
        VertexIndexMap;
typedef boost::property_map<DirectedGraph, boost::vertex_name_t>::type
        VertexValueMap;
typedef boost::iterator_property_map<Vertex*, VertexIndexMap, Vertex, Vertex&>
        PredecessorMap;
typedef std::unordered_map<Vertex, std::set<Vertex>> MultiPredecessorMap;
typedef boost::iterator_property_map<double*, VertexIndexMap, double, double&>
        DistanceMap;
typedef boost::iterator_property_map<
        std::vector<Vertex>::iterator, VertexIndexMap, Vertex, Vertex&>
        VertexVertexMap;

#endif //GBMOT_DEFINITIONS_H
