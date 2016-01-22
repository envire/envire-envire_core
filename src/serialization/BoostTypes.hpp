#pragma once

//HACK to gain access to private members in directed_graph
//Otherwise the directed graph cannot be serialized currently
#define private public
#include <boost/graph/directed_graph.hpp>
#undef private

#include <boost/graph/adj_list_serialize.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/serialization/binary_object.hpp>
#include <boost/serialization/nvp.hpp>

namespace boost { namespace serialization
{

    /** Serializes boost::uuids::uuid */
    template<typename _Archive>
    inline void serialize(
        _Archive & ar,
        boost::uuids::uuid& uuid,
        const unsigned int version)
    {
        ar & boost::serialization::make_nvp("uuid", boost::serialization::make_binary_object(uuid.data, uuid.size()));
    }

    /** Serializes boost::directed_graph */
    template<class Archive, typename VertexProp, typename EdgeProp, typename GraphProp>
    inline void serialize(Archive & ar,
                   boost::directed_graph<VertexProp, EdgeProp, GraphProp>& graph,
                   const unsigned int version)
    {
        ar & boost::serialization::make_nvp("adjacency_list", graph.impl());
        ar & boost::serialization::make_nvp("m_num_vertices", graph.m_num_vertices);
        ar & boost::serialization::make_nvp("m_num_edges", graph.m_num_edges);
        ar & boost::serialization::make_nvp("m_max_vertex_index", graph.m_max_vertex_index);
        ar & boost::serialization::make_nvp("m_max_edge_index", graph.m_max_edge_index);
    }

}}