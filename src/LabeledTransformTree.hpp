/**\file LabeledTransformTree.hpp
 *
 * This class provided stores and manages the labeled transformation tree
 *
 * @author Javier Hidalgo Carrio et. al (see THANKS  for the full Author list)
 * See LICENSE for the license information
 *
 */


#ifndef __ENVIRE_CORE_LABELED_TRANSFORM_TREE__
#define __ENVIRE_CORE_LABELED_TRANSFORM_TREE__

#include <envire_core/TransformTree.hpp> /** The Transformation Tree **/
#include <boost/graph/labeled_graph.hpp> /** Boost wrapper for labeled graph **/

namespace envire { namespace core
{
    /**@brief generic label
     * use to accessing vertex in labeled graphs
     * */
    typedef std::string VertexLabel;

    /**@brief Labeled version of the Transformation Graph
     */
    typedef boost::labeled_graph< TransformGraph, VertexLabel> LabeledTransformGraph;

    /**@class Labeled Transformation Tree
    *
    * Labeled Transformation Tree class
    */
    class LabeledTransformTree: public LabeledTransformGraph
    {
    public:
        typedef typename boost::property_map <LabeledTransformGraph, FramePropertyTag>::type FrameMap;
        typedef typename boost::property_map <LabeledTransformGraph, TransformPropertyTag>::type TransformMap;

    public:

        LabeledTransformTree(envire::core::Environment const &environment = Environment()):
                        LabeledTransformGraph (environment)
        {
        }


        /***************************************************
         * Methods Naming convention
         * Overloading boost methods uses delimited separated
         * words, new methods use Camel Case separated words
         ***************************************************/

        /** ADD VERTEX METHODS **/

        /**@brief Add a vertex to the tree
         */
        inline LabeledTransformTree::vertex_descriptor addVertex(const VertexLabel &node_label)
        {
            envire::core::Frame node(node_label);
            return this->add_vertex(node_label, node);
        }

        /**@brief Add a vertex to the tree
         */
        inline LabeledTransformTree::vertex_descriptor addVertex(const envire::core::Frame &node)
        {
            return this->add_vertex(node.name, node);
        }

        /**@brief Add a vertex to the tree
        */
        inline envire::core::Frame getProperty(const LabeledTransformTree::vertex_descriptor node)
        {
            return boost::get(FramePropertyTag(), *this, node);
        }

        /**@brief Add a vertex to the tree
        */
        inline envire::core::Transform getProperty(const LabeledTransformTree::edge_descriptor edge)
        {
            return boost::get(TransformPropertyTag(), *this, edge);
        }


        /**@brief getVertex
         *
         * Get a vertex descriptor for the vertex/node label
         * */
        inline LabeledTransformTree::vertex_descriptor getVertex(const VertexLabel &node_label)
        {
            return boost::vertex_by_label(node_label, *this);
        }

        /**@brief source
         *
         * Get source vertex descriptor for edge descriptor
         * */
        inline LabeledTransformTree::vertex_descriptor source(const LabeledTransformTree::edge_descriptor it_node)
        {
            return boost::source(it_node, *this);
        }

        /**@brief target
         *
         * Get target vertex descriptor for edge descriptor
         * */
        inline LabeledTransformTree::vertex_descriptor target(const LabeledTransformTree::edge_descriptor it_node)
        {
            return boost::target(it_node, *this);
        }


        /**@brief Remove a vertex to the tree
         *
         * This method remove the vertex searching by label and
         * its associated edges. If you want to only remove
         * the vertex use the boost remove_vertex method
         */
        inline void removeVertex(const VertexLabel &node_label)
        {
            /** First remove the associated edges to the vertex **/
            boost::clear_vertex_by_label(node_label, *this);
            return this->remove_vertex(node_label);
        }

        inline std::pair<out_edge_iterator,out_edge_iterator>
        out_edges(const LabeledTransformTree::vertex_descriptor &node)
        {
            return boost::out_edges(node, *this);
        }

        /**@brief Add an Edge to the Tree
         * Add an edge using the labels
         */
        inline std::pair<LabeledTransformTree::edge_descriptor, bool>
        addEdge(const VertexLabel &node_from,
                    const VertexLabel &node_to,
                    const envire::core::Transform &tf)
        {
            /* Don't allow parallel edges **/
            std::pair<envire::core::LabeledTransformTree::edge_descriptor, bool> edge_pair =
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
            envire::core::LabeledTransformTree::vertex_descriptor v_from =
                boost::vertex_by_label(node_from, *this);

            /** Get the vertex descriptor **/
            envire::core::LabeledTransformTree::vertex_descriptor v_to =
                boost::vertex_by_label(node_to, *this);

            return this->removeEdge(v_from, v_to, destructive);
        }

        /**@brief Remove an Edge from the Tree
         * Remove an edge using the labels.
         * In case destructive is true, the associated
         * vertex are also remove ONLY in case they do not
         * have other connexions.
         */
        inline void removeEdge(const LabeledTransformTree::vertex_descriptor node_from,
                    const LabeledTransformTree::vertex_descriptor node_to,
                    const bool destructive = false)
        {
            boost::remove_edge(node_from, node_to, *this);

            std::cout<<"[TREE] Edge removed\n";

            if (destructive == true)
            {
                std::cout<<"[TREE] Destructive\n";

                /** Check in edges of node_from **/
                LabeledTransformTree::degree_size_type in_node_from =
                    boost::in_degree(node_from, *this);

                /** Check out edges of node_from **/
                LabeledTransformTree::degree_size_type out_node_from =
                    boost::out_degree(node_from, *this);

                std::cout<<"[TREE] node from in+ out edges: "<<in_node_from+out_node_from<<"\n";

                if (in_node_from + out_node_from == 0)
                {
                    boost::remove_vertex(node_from, this->graph());
                }

                /** Check in edges of node_to **/
                LabeledTransformTree::degree_size_type in_node_to =
                    boost::in_degree(node_to, *this);

                /** Check out edges of node_to **/
                LabeledTransformTree::degree_size_type out_node_to =
                    boost::out_degree(node_to, *this);

                std::cout<<"[TREE] node to in+ out edges: "<<in_node_to+out_node_to<<"\n";

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
