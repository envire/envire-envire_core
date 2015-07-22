/**\file TransformTree.hpp
 *
 * This class provided stores and manages the transformation tree
 *
 * @author Javier Hidalgo Carrio et. al (see THANKS  for the full Author list)
 * See LICENSE for the license information
 *
 */


#ifndef __ENVIRE_CORE_TRANSFORM_TREE__
#define __ENVIRE_CORE_TRANSFORM_TREE__

#include <fstream> // std::ofstream

#include <boost/graph/directed_graph.hpp> /** Boost directed graph **/
#include <boost/graph/labeled_graph.hpp> /** Boost directed graph **/
#include <boost/uuid/uuid.hpp> /** uuid class */

#include <envire_core/Frame.hpp> /** Frames are for the Vertex **/
#include <envire_core/Transform.hpp> /** Transform are the Edges **/

namespace envire { namespace core
{
    /**@brief Environment information
     */
    struct Environment
    {
        boost::uuids::uuid uuid; /** Environment Unique Identifier */
        std::string name; /** Envire Name **/
        /** Perhaps here some environment information
         * like date of creation or UTM grid coordinates **/

        Environment()
        {
            name = "envire::core::noname";
        }
    };

    /**@brief Frame Property Tag
     * boost access tag for the Frame property
     */
    struct FramePropertyTag
    {
        typedef boost::vertex_property_tag kind;
        static  std::size_t const num;
    };

    //std::size_t const FramePropertyTag::num = (std::size_t) &FramePropertyTag::num;
    typedef boost::property <FramePropertyTag, envire::core::Frame>  FrameProperty;

    /**@brief Transform Property Tag
     * boost access tag for the Transform property
     */
    struct TransformPropertyTag
    {
        typedef boost::edge_property_tag kind;
        static  std::size_t const num;
    };

    //std::size_t const TransformPropertyTag::num = (std::size_t) &TransformPropertyTag::num;
    typedef boost::property <TransformPropertyTag, envire::core::Transform>  TransformProperty;

    /**@brief The Envire Graph type 
    *
    * The basic class for the Envire Graph
    *
    */
    typedef boost::directed_graph<
        FrameProperty,
        TransformProperty,
        envire::core::Environment> EnvireGraph;

    /**@brief generic label
     * use to accessing vertex in labeled graphs
     * */
    typedef std::string VertexLabel;

    /**@brief Labeled version of the Envire Graph
     */
    typedef boost::labeled_graph< EnvireGraph, VertexLabel> LabeledEnvireGraph;

    /**@class Transformation Tree
    *
    * Envire Transformation Tree class
    */
    class TransformTree: public LabeledEnvireGraph
    {
    public:
        typedef typename boost::property_map <LabeledEnvireGraph, FramePropertyTag>::type FrameMap;
        typedef typename boost::property_map <LabeledEnvireGraph, TransformPropertyTag>::type TransformMap;

    /** public class methods **/
    public:

        TransformTree(envire::core::Environment const &environment = Environment()):
                        LabeledEnvireGraph (environment)
        {
        }

        /**@brief Add a vertex to the tree
         */
        inline TransformTree::vertex_descriptor addVertex(const VertexLabel &node_label)
        {
            envire::core::Frame node(node_label);
            return this->add_vertex(node_label, node);
        }

        /**@brief Add a vertex to the tree
         */
        inline TransformTree::vertex_descriptor addVertex(const envire::core::Frame &node)
        {
            return this->add_vertex(node.name, node);
        }

        /**@brief Add a vertex to the tree
         */
        inline TransformTree::vertex_descriptor addVertex(const VertexLabel &node_label,
                                                        const envire::core::Frame &node)
        {
            return this->add_vertex(node_label, node);
        }


        /**@brief Remove a vertex to the tree
         *
         * This method remove the vertex searching by label
         * Note: it does not remove the edges, and create artificial vertices/nodes
         */
        inline void removeVertexOnly(const VertexLabel &node_label)
        {
            return this->remove_vertex(node_label);
        }

        /**@brief getVertex
         *
         * Get a vertex descriptor for the vertex/node label
         * */
        inline TransformTree::vertex_descriptor getVertex(const VertexLabel &node_label)
        {
            return boost::vertex_by_label(node_label, *this);
        }

        /**@brief source
         *
         * Get source vertex descriptor for edge descriptor
         * */
        inline TransformTree::vertex_descriptor source(const TransformTree::edge_descriptor it_node)
        {
            return boost::source(it_node, *this);
        }

        /**@brief target
         *
         * Get target vertex descriptor for edge descriptor
         * */
        inline TransformTree::vertex_descriptor target(const TransformTree::edge_descriptor it_node)
        {
            return boost::target(it_node, *this);
        }


        /**@brief Remove a vertex to the tree
         *
         * This method remove the vertex searching by label and
         * its associated edges.
         */
        inline void removeVertex(const VertexLabel &node_label)
        {
            /** First remove the associated edges to the vertex **/
            boost::clear_vertex_by_label(node_label, *this);
            return boost::remove_vertex(node_label, *this);
        }

        inline std::pair<out_edge_iterator,out_edge_iterator>
        outEdges(const TransformTree::vertex_descriptor &node)
        {
            return boost::out_edges(node, *this);
        }

        /**@brief Add an Edge to the Tree
         * Add an edge using the labels
         */
        inline std::pair<TransformTree::edge_descriptor, bool>
        addEdge(const VertexLabel &node_from,
                    const VertexLabel &node_to,
                    const envire::core::Transform &tf)
        {
            /* Don't allow parallel edges **/
            std::pair<envire::core::TransformTree::edge_descriptor, bool> edge_pair =
                boost::edge_by_label(node_from, node_to, *this);

            /** Update the edge in case it already exist **/
            if (edge_pair.second)
            {
                boost::put(TransformPropertyTag(), *this, edge_pair.first, tf);
            }
            else
            {
                edge_pair =  boost::add_edge_by_label(node_from, node_to, tf, *this);
            }
            return edge_pair;
        }

        /**@brief Remove an Edge from the Tree
         * Remove an edge using the labels.
         * In case destructive is true, the associated
         * vertex are also remove ONLY in case they do not
         * have other connexions.
         */
        inline void removeEdge(const VertexLabel &node_from,
                    const VertexLabel &node_to,
                    const bool destructive = false)
        {
            /** Get the vertex descriptor **/
            envire::core::TransformTree::vertex_descriptor v_from =
                boost::vertex_by_label(node_from, *this);

            /** Get the vertex descriptor **/
            envire::core::TransformTree::vertex_descriptor v_to =
                boost::vertex_by_label(node_to, *this);

            return this->removeEdge(v_from, v_to, destructive);
        }

        /**@brief Remove an Edge from the Tree
         * Remove an edge using the labels.
         * In case destructive is true, the associated
         * vertex are also remove ONLY in case they do not
         * have other connexions.
         */
        inline void removeEdge(const TransformTree::vertex_descriptor node_from,
                    const TransformTree::vertex_descriptor node_to,
                    const bool destructive = false)
        {
            boost::remove_edge(node_from, node_to, *this);

            if (destructive == true)
            {
                /** Check in edges of node_from **/
                TransformTree::degree_size_type in_node_from =
                    boost::in_degree(node_from, *this);

                /** Check out edges of node_from **/
                TransformTree::degree_size_type out_node_from =
                    boost::out_degree(node_from, *this);

                if (in_node_from + out_node_from == 0)
                {
                    boost::remove_vertex(node_from, this->graph());
                }

                /** Check in edges of node_to **/
                TransformTree::degree_size_type in_node_to =
                    boost::in_degree(node_to, *this);

                /** Check out edges of node_to **/
                TransformTree::degree_size_type out_node_to =
                    boost::out_degree(node_to, *this);

                if (in_node_to + out_node_to == 0)
                {
                    boost::remove_vertex(node_to, this->graph());
                }
            }
            return;
        }



        /**@brief clear
         *
         * Remove all of the edges and vertices from the graph.
         */
        inline void clear(){ return this->graph().clear(); }
    };
}}
#endif
