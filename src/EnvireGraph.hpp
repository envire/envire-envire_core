#ifndef __ENVIRE_CORE_ENVIRE_GRAPH__
#define __ENVIRE_CORE_ENVIRE_GRAPH__

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/properties.hpp>
#include <boost/pending/property.hpp>
#include <boost/property_map/transform_value_property_map.hpp>
#include <boost/type_traits.hpp>
#include <boost/mpl/if.hpp>

namespace boost
{
    struct EnvireGraphTag { };

    /**
     * The EnvireGraph class template is a simplified version of the BGL
     * adjacency list. This class is provided for ease of use, but may not
     * perform as well as custom-defined adjacency list classes. Instances of
     * this template model the BidirectionalGraph, VertexIndexGraph, and
     * EdgeIndexGraph concepts. The graph is also fully mutable, supporting
     * both insertions and removals of vertices and edges.
     *
     * @note Special care must be taken when removing vertices or edges since
     * those operations can invalidate the numbering of vertices.
     */
    template <
        typename VertexProp = no_property,
        typename EdgeProp = no_property,
        typename GraphProp = no_property>
    class EnvireGraph
    {
    public:
        typedef GraphProp graph_property_type;
        typedef VertexProp vertex_property_type;
        typedef EdgeProp edge_property_type;
        typedef typename lookup_one_property<GraphProp, graph_bundle_t>::type graph_bundled;
        typedef typename lookup_one_property<VertexProp, vertex_bundle_t>::type vertex_bundled;
        typedef typename lookup_one_property<EdgeProp, edge_bundle_t>::type edge_bundled;

    public:
        // Embed indices into the vertex type.
        typedef property<vertex_index_t, unsigned, vertex_property_type> internal_vertex_property;
        typedef property<edge_index_t, unsigned, edge_property_type> internal_edge_property;
    public:
        typedef adjacency_list<
            vecS, vecS, bidirectionalS,
            internal_vertex_property, internal_edge_property, GraphProp,
            listS
        > graph_type;

    private:
        // storage selectors
        typedef typename graph_type::vertex_list_selector vertex_list_selector;
        typedef typename graph_type::edge_list_selector edge_list_selector;
        typedef typename graph_type::out_edge_list_selector out_edge_list_selector;
        typedef typename graph_type::directed_selector directed_selector;

    public:
        // more commonly used graph types
        typedef typename graph_type::stored_vertex stored_vertex;
        typedef typename graph_type::vertices_size_type vertices_size_type;
        typedef typename graph_type::edges_size_type edges_size_type;
        typedef typename graph_type::degree_size_type degree_size_type;
        typedef typename graph_type::vertex_descriptor vertex_descriptor;
        typedef typename graph_type::edge_descriptor edge_descriptor;

        // iterator types
        typedef typename graph_type::vertex_iterator vertex_iterator;
        typedef typename graph_type::edge_iterator edge_iterator;
        typedef typename graph_type::out_edge_iterator out_edge_iterator;
        typedef typename graph_type::in_edge_iterator in_edge_iterator;
        typedef typename graph_type::adjacency_iterator adjacency_iterator;

        // miscellaneous types
        typedef EnvireGraphTag graph_tag;
        typedef typename graph_type::directed_category directed_category;
        typedef typename graph_type::edge_parallel_category edge_parallel_category;
        typedef typename graph_type::traversal_category traversal_category;

        typedef std::size_t vertex_index_type;
        typedef std::size_t edge_index_type;

        EnvireGraph(GraphProp const& p = GraphProp())
            : m_graph(p), m_num_vertices(0), m_num_edges(0)
            , m_max_edge_index(0)
        { }

        EnvireGraph(EnvireGraph const& x)
            : m_graph(x), m_num_vertices(x.m_num_vertices), m_num_edges(x.m_num_edges)
            , m_max_edge_index(x.m_max_edge_index)
        { }

        EnvireGraph(vertices_size_type n, GraphProp const& p = GraphProp())
            : m_graph(n, p), m_num_vertices(n), m_num_edges(0)
            , m_max_edge_index(0)
        { renumber_vertex_indices(); }

        template <typename EdgeIterator>
        EnvireGraph(EdgeIterator f,
                       EdgeIterator l,
                       vertices_size_type n,
                       edges_size_type m = 0,
                       GraphProp const& p = GraphProp())
            : m_graph(f, l, n, m, p), m_num_vertices(n), m_num_edges(0)
            , m_max_edge_index(0)
        {
            // Unfortunately, we have to renumber the entire graph.
            renumber_indices();

            // Can't always guarantee that the number of edges is actually
            // m if distance(f, l) != m (or is undefined).
            m_num_edges = m_max_edge_index = boost::num_edges(m_graph);
        }

        EnvireGraph& operator=(EnvireGraph const& g) {
            if(&g != this) {
                m_graph = g.m_graph;
                m_num_vertices = g.m_num_vertices;
                m_num_edges = g.m_num_edges;
                m_max_edge_index = g.m_max_edge_index;
            }
            return *this;
        }

        // The impl_() methods are not part of the public interface.
        graph_type& impl()
        { return m_graph; }

        graph_type const& impl() const
        { return m_graph; }

        // The following methods are not part of the public interface
        vertices_size_type num_vertices() const
        { return m_num_vertices; }


    public:
        vertex_descriptor add_vertex()
        { return make_index(boost::add_vertex(m_graph)); }

        vertex_descriptor add_vertex(vertex_property_type const& p)
        {
            m_num_vertices++;
            return boost::add_vertex(internal_vertex_property(0u, p), m_graph);
        }

        void clear_vertex(vertex_descriptor v)
        {
            m_num_edges -= boost::degree(v, m_graph);
            boost::clear_vertex(v, m_graph);
        }

        void remove_vertex(vertex_descriptor v)
        {
            boost::remove_vertex(v, m_graph);
            --m_num_vertices;
        }

        edges_size_type num_edges() const
        { return m_num_edges; }

    private:
        // A helper fucntion for managing edge index attributes.
        std::pair<edge_descriptor, bool> const&
        make_index(std::pair<edge_descriptor, bool> const& x)
        {
            if(x.second) {
                boost::put(edge_index, m_graph, x.first, m_max_edge_index);
                ++m_num_edges;
                ++m_max_edge_index;
            }
            return x;
        }
    public:
        std::pair<edge_descriptor, bool>
        add_edge(vertex_descriptor u, vertex_descriptor v)
        { return make_index(boost::add_edge(u, v, m_graph)); }

        std::pair<edge_descriptor, bool>
        add_edge(vertex_descriptor u, vertex_descriptor v, edge_property_type const& p)
        { return make_index(boost::add_edge(u, v, internal_edge_property(0u, p), m_graph)); }

        void remove_edge(vertex_descriptor u, vertex_descriptor v)
        {
            // find all edges, (u, v)
            std::vector<edge_descriptor> edges;
            out_edge_iterator i, i_end;
            for(boost::tie(i, i_end) = boost::out_edges(u, m_graph); i != i_end; ++i) {
                if(boost::target(*i, m_graph) == v) {
                    edges.push_back(*i);
                }
            }
            // remove all edges, (u, v)
            typename std::vector<edge_descriptor>::iterator
            j = edges.begin(), j_end = edges.end();
            for( ; j != j_end; ++j) {
                remove_edge(*j);
            }
        }

        void remove_edge(edge_iterator i)
        {
            remove_edge(*i);
        }

        void remove_edge(edge_descriptor e)
        {
            boost::remove_edge(e, m_graph);
            --m_num_edges;
        }

        void
        renumber_vertex_indices()
        {
            vertex_iterator i, end;
            boost::tie(i, end) = vertices(m_graph);
        }

        void
        remove_vertex_and_renumber_indices(vertex_iterator i)
        {
            vertex_iterator j = next(i), end = vertices(m_graph).second;
            vertex_index_type n = get(vertex_index, m_graph, *i);

            // remove the offending vertex and renumber everything after
            remove_vertex(*i);
        }

        edge_index_type
        max_edge_index() const
        { return m_max_edge_index; }

        void
        renumber_edge_indices()
        {
            edge_iterator i, end;
            boost::tie(i, end) = edges(m_graph);
            m_max_edge_index = renumber_edge_indices(i, end, 0);
        }

        void
        remove_edge_and_renumber_indices(edge_iterator i)
        {
            edge_iterator j = next(i), end = edges(m_graph).second;
            edge_index_type n = get(edge_index, m_graph, *i);

            // remove the offending edge and renumber everything after
            remove_edge(*i);
            m_max_edge_index = renumber_edge_indices(j, end, n);
        }

        void
        renumber_indices()
        {
            renumber_vertex_indices();
            renumber_edge_indices();
        }

        // bundled property support
        #ifndef BOOST_GRAPH_NO_BUNDLED_PROPERTIES
        vertex_bundled& operator[](vertex_descriptor v)
        { return m_graph[v]; }

        vertex_bundled const& operator[](vertex_descriptor v) const
        { return m_graph[v]; }

        edge_bundled& operator[](edge_descriptor e)
        { return m_graph[e]; }

        edge_bundled const& operator[](edge_descriptor e) const
        { return m_graph[e]; }

        graph_bundled& operator[](graph_bundle_t)
        { return get_property(*this); }

        graph_bundled const& operator[](graph_bundle_t) const
        { return get_property(*this); }
        #endif

        // Graph concepts
        static vertex_descriptor null_vertex()
        { return graph_type::null_vertex(); }

        void clear()
        {
            m_graph.clear();
            m_num_edges = m_max_edge_index = 0;
        }

        void swap(EnvireGraph& g)
        {
            m_graph.swap(g.m_graph);
            std::swap(m_num_vertices, g.m_num_vertices);
            std::swap(m_num_edges, g.m_num_edges);
            std::swap(m_max_edge_index, g.m_max_edge_index);
        }

    private:
        vertices_size_type
        renumber_vertex_indices(vertex_iterator i,
                                vertex_iterator end,
                                vertices_size_type n)
        {
            typedef typename property_map<graph_type, vertex_index_t>::type IndexMap;
            IndexMap indices = get(vertex_index, m_graph);
            for( ; i != end; ++i) {
                indices[*i] = n++;
            }
            return n;
        }

        vertices_size_type
        renumber_edge_indices(edge_iterator i,
                            edge_iterator end,
                            vertices_size_type n)
        {
            typedef typename property_map<graph_type, edge_index_t>::type IndexMap;
            IndexMap indices = get(edge_index, m_graph);
            for( ; i != end; ++i) {
                indices[*i] = n++;
            }
            return n;
        }

        graph_type m_graph;
        vertices_size_type m_num_vertices;
        edges_size_type m_num_edges;
        edge_index_type m_max_edge_index;
    };

    #define ENVIRE_GRAPH_PARAMS typename VP, typename EP, typename GP
    #define ENVIRE_GRAPH EnvireGraph<VP,EP,GP>

    // IncidenceGraph concepts
    template <ENVIRE_GRAPH_PARAMS>
    inline typename ENVIRE_GRAPH::vertex_descriptor
    source(typename ENVIRE_GRAPH::edge_descriptor e, ENVIRE_GRAPH const& g)
    { return source(e, g.impl()); }

    template <ENVIRE_GRAPH_PARAMS>
    inline typename ENVIRE_GRAPH::vertex_descriptor
    target(typename ENVIRE_GRAPH::edge_descriptor e, ENVIRE_GRAPH const& g)
    { return target(e, g.impl()); }

    template <ENVIRE_GRAPH_PARAMS>
    inline typename ENVIRE_GRAPH::degree_size_type
    out_degree(typename ENVIRE_GRAPH::vertex_descriptor v, ENVIRE_GRAPH const& g)
    { return out_degree(v, g.impl()); }

    template <ENVIRE_GRAPH_PARAMS>
    inline std::pair<
        typename ENVIRE_GRAPH::out_edge_iterator,
        typename ENVIRE_GRAPH::out_edge_iterator
    >
    out_edges(typename ENVIRE_GRAPH::vertex_descriptor v,
            ENVIRE_GRAPH const& g)
    { return out_edges(v, g.impl()); }

    // BidirectionalGraph concepts
    template <ENVIRE_GRAPH_PARAMS>
    inline typename ENVIRE_GRAPH::degree_size_type
    in_degree(typename ENVIRE_GRAPH::vertex_descriptor v, ENVIRE_GRAPH const& g)
    { return in_degree(v, g.impl()); }

    template <ENVIRE_GRAPH_PARAMS>
    inline std::pair<
        typename ENVIRE_GRAPH::in_edge_iterator,
        typename ENVIRE_GRAPH::in_edge_iterator
    >
    in_edges(typename ENVIRE_GRAPH::vertex_descriptor v,
            ENVIRE_GRAPH const& g)
    { return in_edges(v, g.impl()); }


    template <ENVIRE_GRAPH_PARAMS>
    inline typename ENVIRE_GRAPH::degree_size_type
    degree(typename ENVIRE_GRAPH::vertex_descriptor v, ENVIRE_GRAPH const& g)
    { return degree(v, g.impl()); }

    // AdjacencyGraph concepts
    template <ENVIRE_GRAPH_PARAMS>
    inline std::pair<
        typename ENVIRE_GRAPH::adjacency_iterator,
        typename ENVIRE_GRAPH::adjacency_iterator
        >
    adjacent_vertices(typename ENVIRE_GRAPH::vertex_descriptor v,
                      ENVIRE_GRAPH const& g)
    { return adjacent_vertices(v, g.impl()); }

    template <ENVIRE_GRAPH_PARAMS>
    typename ENVIRE_GRAPH::vertex_descriptor
    vertex(typename ENVIRE_GRAPH::vertices_size_type n,
           ENVIRE_GRAPH const& g)
    { return vertex(n, g.impl()); }

    template <ENVIRE_GRAPH_PARAMS>
    std::pair<typename ENVIRE_GRAPH::edge_descriptor, bool>
    edge(typename ENVIRE_GRAPH::vertex_descriptor u,
         typename ENVIRE_GRAPH::vertex_descriptor v,
         ENVIRE_GRAPH const& g)
    { return edge(u, v, g.impl()); }

    // VertexListGraph concepts
    template <ENVIRE_GRAPH_PARAMS>
    inline typename ENVIRE_GRAPH::vertices_size_type
    num_vertices(ENVIRE_GRAPH const& g)
    { return g.num_vertices(); }

    template <ENVIRE_GRAPH_PARAMS>
    inline std::pair<
        typename ENVIRE_GRAPH::vertex_iterator,
        typename ENVIRE_GRAPH::vertex_iterator
    >
    vertices(ENVIRE_GRAPH const& g)
    { return vertices(g.impl()); }

    // EdgeListGraph concepts
    template <ENVIRE_GRAPH_PARAMS>
    inline typename ENVIRE_GRAPH::edges_size_type
    num_edges(ENVIRE_GRAPH const& g)
    { return g.num_edges(); }

    template <ENVIRE_GRAPH_PARAMS>
    inline std::pair<
        typename ENVIRE_GRAPH::edge_iterator,
        typename ENVIRE_GRAPH::edge_iterator
    >
    edges(ENVIRE_GRAPH const& g)
    { return edges(g.impl()); }

    // MutableGraph concepts
    template <ENVIRE_GRAPH_PARAMS>
    inline typename ENVIRE_GRAPH::vertex_descriptor
    add_vertex(ENVIRE_GRAPH& g)
    { return g.add_vertex(); }

    template <ENVIRE_GRAPH_PARAMS>
    inline typename ENVIRE_GRAPH::vertex_descriptor
    add_vertex(typename ENVIRE_GRAPH::vertex_property_type const& p,
               ENVIRE_GRAPH& g)
    { return g.add_vertex(p); }

    template <ENVIRE_GRAPH_PARAMS>
    inline void
    clear_vertex(typename ENVIRE_GRAPH::vertex_descriptor v,
    ENVIRE_GRAPH& g)
    { return g.clear_vertex(v); }

    template <ENVIRE_GRAPH_PARAMS>
    inline void
    remove_vertex(typename ENVIRE_GRAPH::vertex_descriptor v,
                  ENVIRE_GRAPH& g)
    { return g.remove_vertex(v); }

    template <ENVIRE_GRAPH_PARAMS>
    inline std::pair<typename ENVIRE_GRAPH::edge_descriptor, bool>
    add_edge(typename ENVIRE_GRAPH::vertex_descriptor u,
             typename ENVIRE_GRAPH::vertex_descriptor v,
             ENVIRE_GRAPH& g)
    { return g.add_edge(u, v); }

    template <ENVIRE_GRAPH_PARAMS>
    inline std::pair<typename ENVIRE_GRAPH::edge_descriptor, bool>
    add_edge(typename ENVIRE_GRAPH::vertex_descriptor u,
             typename ENVIRE_GRAPH::vertex_descriptor v,
             typename ENVIRE_GRAPH::edge_property_type const& p,
             ENVIRE_GRAPH& g)
    { return g.add_edge(u, v, p); }

    template <ENVIRE_GRAPH_PARAMS>
    inline void remove_edge(typename ENVIRE_GRAPH::vertex_descriptor u,
                            typename ENVIRE_GRAPH::vertex_descriptor v,
                            ENVIRE_GRAPH& g)
    { return g.remove_edge(u, v); }


    template <ENVIRE_GRAPH_PARAMS>
    inline void remove_edge(typename ENVIRE_GRAPH::edge_descriptor e, ENVIRE_GRAPH& g)
    { return g.remove_edge(e); }

    template <ENVIRE_GRAPH_PARAMS>
    inline void remove_edge(typename ENVIRE_GRAPH::edge_iterator i, ENVIRE_GRAPH& g)
    { return g.remove_edge(i); }

    template <ENVIRE_GRAPH_PARAMS, class Predicate>
    inline void remove_edge_if(Predicate pred, ENVIRE_GRAPH& g)
    { return remove_edge_if(pred, g.impl()); }

    template <ENVIRE_GRAPH_PARAMS, class Predicate>
    inline void
    remove_out_edge_if(typename ENVIRE_GRAPH::vertex_descriptor v,
                       Predicate pred,
                       ENVIRE_GRAPH& g)
    { return remove_out_edge_if(v, pred, g.impl()); }

    template <ENVIRE_GRAPH_PARAMS, class Predicate>
    inline void
    remove_in_edge_if(typename ENVIRE_GRAPH::vertex_descriptor v,
                    Predicate pred,
                    ENVIRE_GRAPH& g)
    { return remove_in_edge_if(v, pred, g.impl()); }

    template <ENVIRE_GRAPH_PARAMS, typename Property>
    struct property_map<ENVIRE_GRAPH, Property>: property_map<typename ENVIRE_GRAPH::graph_type, Property> {};

    template <ENVIRE_GRAPH_PARAMS>
    struct property_map<ENVIRE_GRAPH, vertex_all_t> {
      typedef transform_value_property_map<
                detail::remove_first_property,
                typename property_map<typename ENVIRE_GRAPH::graph_type, vertex_all_t>::const_type>
        const_type;
      typedef transform_value_property_map<
                detail::remove_first_property,
                typename property_map<typename ENVIRE_GRAPH::graph_type, vertex_all_t>::type>
        type;
    };

    template <ENVIRE_GRAPH_PARAMS>
    struct property_map<ENVIRE_GRAPH, edge_all_t> {
      typedef transform_value_property_map<
                detail::remove_first_property,
                typename property_map<typename ENVIRE_GRAPH::graph_type, edge_all_t>::const_type>
        const_type;
      typedef transform_value_property_map<
                detail::remove_first_property,
                typename property_map<typename ENVIRE_GRAPH::graph_type, edge_all_t>::type>
        type;
    };

    // PropertyGraph concepts
    template <ENVIRE_GRAPH_PARAMS, typename Property>
    inline typename property_map<ENVIRE_GRAPH, Property>::type
    get(Property p, ENVIRE_GRAPH& g)
    { return get(p, g.impl()); }

    template <ENVIRE_GRAPH_PARAMS, typename Property>
    inline typename property_map<ENVIRE_GRAPH, Property>::const_type
    get(Property p, ENVIRE_GRAPH const& g)
    { return get(p, g.impl()); }

    template <ENVIRE_GRAPH_PARAMS>
    inline typename property_map<ENVIRE_GRAPH, vertex_all_t>::type
    get(vertex_all_t, ENVIRE_GRAPH& g)
    { return typename property_map<ENVIRE_GRAPH, vertex_all_t>::type(detail::remove_first_property(), get(vertex_all, g.impl())); }

    template <ENVIRE_GRAPH_PARAMS>
    inline typename property_map<ENVIRE_GRAPH, vertex_all_t>::const_type
    get(vertex_all_t, ENVIRE_GRAPH const& g)
    { return typename property_map<ENVIRE_GRAPH, vertex_all_t>::const_type(detail::remove_first_property(), get(vertex_all, g.impl())); }

    template <ENVIRE_GRAPH_PARAMS>
    inline typename property_map<ENVIRE_GRAPH, edge_all_t>::type
    get(edge_all_t, ENVIRE_GRAPH& g)
    { return typename property_map<ENVIRE_GRAPH, edge_all_t>::type(detail::remove_first_property(), get(edge_all, g.impl())); }

    template <ENVIRE_GRAPH_PARAMS>
    inline typename property_map<ENVIRE_GRAPH, edge_all_t>::const_type
    get(edge_all_t, ENVIRE_GRAPH const& g)
    { return typename property_map<ENVIRE_GRAPH, edge_all_t>::const_type(detail::remove_first_property(), get(edge_all, g.impl())); }

    template <ENVIRE_GRAPH_PARAMS, typename Property, typename Key>
    inline typename property_traits<
        typename property_map<
            typename ENVIRE_GRAPH::graph_type, Property
        >::const_type
    >::value_type
    get(Property p, ENVIRE_GRAPH const& g, Key const& k)
    { return get(p, g.impl(), k); }

    template <ENVIRE_GRAPH_PARAMS, typename Key>
    inline typename property_traits<
        typename property_map<
            typename ENVIRE_GRAPH::graph_type, vertex_all_t
        >::const_type
    >::value_type
    get(vertex_all_t, ENVIRE_GRAPH const& g, Key const& k)
    { return get(vertex_all, g.impl(), k).m_base; }

    template <ENVIRE_GRAPH_PARAMS, typename Key>
    inline typename property_traits<
        typename property_map<
            typename ENVIRE_GRAPH::graph_type, edge_all_t
        >::const_type
    >::value_type
    get(edge_all_t, ENVIRE_GRAPH const& g, Key const& k)
    { return get(edge_all, g.impl(), k).m_base; }

    template <ENVIRE_GRAPH_PARAMS, typename Property, typename Key, typename Value>
    inline void put(Property p, ENVIRE_GRAPH& g, Key const& k, Value const& v)
    { put(p, g.impl(), k, v); }

    template <ENVIRE_GRAPH_PARAMS, typename Key, typename Value>
    inline void put(vertex_all_t, ENVIRE_GRAPH& g, Key const& k, Value const& v)
    { put(vertex_all, g.impl(), k,
          typename ENVIRE_GRAPH::internal_vertex_property(get(vertex_index, g.impl(), k), v));
    }

    template <ENVIRE_GRAPH_PARAMS, typename Key, typename Value>
    inline void put(edge_all_t, ENVIRE_GRAPH& g, Key const& k, Value const& v)
    { put(edge_all, g.impl(), k,
          typename ENVIRE_GRAPH::internal_vertex_property(get(edge_index, g.impl(), k), v));
    }

    template <ENVIRE_GRAPH_PARAMS, class Property>
    typename graph_property<ENVIRE_GRAPH, Property>::type&
    get_property(ENVIRE_GRAPH& g, Property p)
    { return get_property(g.impl(), p); }

    template <ENVIRE_GRAPH_PARAMS, class Property>
    typename graph_property<ENVIRE_GRAPH, Property>::type const&
    get_property(ENVIRE_GRAPH const& g, Property p)
    { return get_property(g.impl(), p); }

    template <ENVIRE_GRAPH_PARAMS, class Property, class Value>
    void
    set_property(ENVIRE_GRAPH& g, Property p, Value v)
    { return set_property(g.impl(), p, v); }

    // Vertex index management

    template <ENVIRE_GRAPH_PARAMS>
    inline typename ENVIRE_GRAPH::vertex_index_type
    get_vertex_index(typename ENVIRE_GRAPH::vertex_descriptor v,
                    ENVIRE_GRAPH const& g)
    { return get(vertex_index, g, v); }

    template <ENVIRE_GRAPH_PARAMS>
    typename ENVIRE_GRAPH::vertex_index_type
    max_vertex_index(ENVIRE_GRAPH const& g)
    { return g.max_vertex_index(); }

    template <ENVIRE_GRAPH_PARAMS>
    inline void
    renumber_vertex_indices(ENVIRE_GRAPH& g)
    { g.renumber_vertex_indices(); }

    template <ENVIRE_GRAPH_PARAMS>
    inline void
    remove_vertex_and_renumber_indices(typename ENVIRE_GRAPH::vertex_iterator i,
                                    ENVIRE_GRAPH& g)
    { g.remove_vertex_and_renumber_indices(i); }

    // Edge index management
    template <ENVIRE_GRAPH_PARAMS>
    inline typename ENVIRE_GRAPH::edge_index_type
    get_edge_index(typename ENVIRE_GRAPH::edge_descriptor v, ENVIRE_GRAPH const& g)
    { return get(edge_index, g, v); }

    template <ENVIRE_GRAPH_PARAMS>
    typename ENVIRE_GRAPH::edge_index_type
    max_edge_index(ENVIRE_GRAPH const& g)
    { return g.max_edge_index(); }

    template <ENVIRE_GRAPH_PARAMS>
    inline void renumber_edge_indices(ENVIRE_GRAPH& g)
    { g.renumber_edge_indices(); }

    template <ENVIRE_GRAPH_PARAMS>
    inline void
    remove_edge_and_renumber_indices(typename ENVIRE_GRAPH::edge_iterator i,
                                     ENVIRE_GRAPH& g)
    { g.remove_edge_and_renumber_indices(i); }

    // Index management
    template <ENVIRE_GRAPH_PARAMS>
    inline void
    renumber_indices(ENVIRE_GRAPH& g)
    { g.renumber_indices(); }

    // Mutability Traits
    template <ENVIRE_GRAPH_PARAMS>
    struct graph_mutability_traits<ENVIRE_GRAPH> {
        typedef mutable_property_graph_tag category;
    };

    #undef ENVIRE_GRAPH_PARAMS
    #undef ENVIRE_GRAPH

} /* namespace envire */

#endif

