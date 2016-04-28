//
// Created by wrede on 25.04.16.
//

#ifndef GBMOT_DEFINITIONS_H
#define GBMOT_DEFINITIONS_H


#include <boost/graph/properties.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include "../core/ObjectData.h"
#include "../core/Tracklet.h"

namespace algo
{
    //TODO read minimal example
    //TODO check types (move to core)
    typedef boost::property<boost::edge_weight_t, double> EdgeProp;
    typedef boost::property<boost::vertex_name_t, core::ObjectData>
            VertexProp;
    typedef boost::adjacency_list<
            boost::listS, boost::vecS, boost::directedS,
            VertexProp, EdgeProp>
            DirectedGraph;
    typedef boost::graph_traits<DirectedGraph>::vertex_descriptor Vertex;
    typedef boost::property_map<DirectedGraph, boost::vertex_index_t>::type
            VertexIndexMap;
    typedef boost::property_map<DirectedGraph, boost::vertex_name_t >::type
            VertexValueMap;
    typedef boost::iterator_property_map<Vertex*, VertexIndexMap, Vertex, Vertex&>
            PredecessorMap;
    typedef boost::iterator_property_map<double*, VertexIndexMap, double, double&>
            DistanceMap;
};

#endif //GBMOT_DEFINITIONS_H
