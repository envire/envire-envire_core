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

        LabeledTransformTree(envire::core::Environment const &environment = Environment()):
                        LabeledTransformGraph (environment)
        {
        }


        /***************************************************
         * Methods Naming convention
         * Overloading boost methods uses delimited separated
         * words, new methods use Camel Case separated words
         ***************************************************/

        /** VERTEX METHODS **/

        /**@brief Add a vertex to the tree
         */
        inline LabeledTransformTree::vertex_descriptor add_vertex(const VertexLabel &node_label)
        {
            FrameProperty node_prop;
            envire::core::Frame node(node_label);
            node_prop.frame = node;
            return LabeledTransformGraph::add_vertex(node_label, node_prop);
        }

        /**@brief Add a vertex to the tree
         */
        inline LabeledTransformTree::vertex_descriptor add_vertex(const envire::core::Frame &node)
        {
            FrameProperty node_prop;
            node_prop.frame = node;
            return LabeledTransformGraph::add_vertex(node.name, node_prop);
        }


        /**@brief Insert a vertex to the tree
         */
        inline std::pair<LabeledTransformTree::vertex_descriptor, bool> insert_vertex(const envire::core::Frame &node)
        {
            FrameProperty node_prop;
            node_prop.frame = node;
            return LabeledTransformGraph::insert_vertex(node.name, node_prop);
        }


        /**@brief getVertex
         *
         * Get a vertex descriptor for the vertex/node label
         * */
        inline LabeledTransformTree::vertex_descriptor vertex(const VertexLabel &node_label)
        {
            return boost::vertex_by_label(node_label, *this);
        }

        /**@brief Get all vertices
         */
        inline std::pair<LabeledTransformTree::vertex_iterator, LabeledTransformTree::vertex_iterator>
        vertices()
        {
            return boost::vertices(*this);
        }


        /**@brief Remove a vertex to the tree
         *
         * This method remove the vertex searching by label and
         * its associated edges. If you want to only remove
         * the vertex use the boost remove_vertex method
         */
        inline void remove_vertex(const VertexLabel &node_label)
        {
            /** First remove the associated edges to the vertex **/
            boost::clear_vertex_by_label(node_label, *this);
            return LabeledTransformGraph::remove_vertex(node_label);
        }

        inline LabeledTransformTree::vertices_size_type num_vertices()
        {
            return  boost::num_vertices(*this);
        }

        inline LabeledTransformTree::degree_size_type degree(const LabeledTransformTree::vertex_descriptor vd)
        {
            return boost::degree(vd, *this);
        }

        /** EDGES METHODS **/

        /**@brief Add an Edge to the Tree
         * Add an edge using the labels
         */
        inline std::pair<LabeledTransformTree::edge_descriptor, bool>
        add_edge(const VertexLabel node_from,
                    const VertexLabel node_to,
                    const envire::core::Transform &tf = envire::core::Transform())
        {
            /* Don't allow parallel edges **/
            std::pair<envire::core::LabeledTransformTree::edge_descriptor, bool> edge_pair =
                boost::edge_by_label(node_from, node_to, *this);

            /** Update the edge in case it already exist **/
            if (edge_pair.second)
            {
                boost::put(&TransformProperty::transform, *this, edge_pair.first, tf);
            }
            else
            {
                TransformProperty tf_prop;
                tf_prop.transform = tf;
                edge_pair =  boost::add_edge_by_label(node_from, node_to, tf_prop, *this);
            }
            return edge_pair;
        }

        /**@brief Add an Edge to the Tree
         * Add an edge using the labels
         */
        inline std::pair<LabeledTransformTree::edge_descriptor, bool>
        add_edge(const LabeledTransformTree::vertex_descriptor node_from,
                    const TransformTree::vertex_descriptor node_to,
                    const envire::core::Transform &tf = envire::core::Transform())
        {
            /* Don't allow parallel edges **/
            std::pair<envire::core::LabeledTransformTree::edge_descriptor, bool> edge_pair =
                boost::edge(node_from, node_to, *this);

            /** Update the edge in case it already exist **/
            if (edge_pair.second)
            {
                boost::put(&TransformProperty::transform, *this, edge_pair.first, tf);
            }
            else
            {
                TransformProperty tf_prop;
                tf_prop.transform = tf;
                edge_pair =  boost::add_edge(node_from, node_to, tf_prop, *this);
            }
            return edge_pair;
        }



        /**@brief Remove an Edge from the Tree
         * Remove an edge using the edge descriptor.
         * In case destructive is true, the associated
         * vertex are also remove ONLY in case they do not
         * have other connexions.
         */
        inline void remove_edge(const LabeledTransformTree::edge_descriptor ed, const bool destructive = false)
        {
            /** Get source and target vertices **/
            LabeledTransformTree::vertex_descriptor svd = boost::source(ed, *this);
            LabeledTransformTree::vertex_descriptor tvd = boost::target(ed, *this);

            /** remove the edge **/
            boost::remove_edge(ed, *this);

            if (destructive)
            {
               // LabeledTransformTree::degree_size_type edges_svd =
               //     boost::degree(svd, *this);

                //std::cout<<"[TREE] svd edges: "<<edges_svd<<"\n";
                if(boost::degree(svd, *this) == 0)
                {
                    boost::remove_vertex(svd, this->graph());
                }

                //LabeledTransformTree::degree_size_type edges_tvd =
                //    boost::degree(tvd, *this);

                //std::cout<<"[TREE] tvd edges: "<<edges_tvd<<"\n";
                if(boost::degree(tvd, *this) == 0)
                {
                    boost::remove_vertex(tvd, this->graph());
                }
            }
            return;
        }

        /**@brief Remove an Edge from the Tree
         * Remove an edge using the labels.
         * In case destructive is true, the associated
         * vertex are also remove ONLY in case they do not
         * have other connexions.
         */
        inline void remove_edge(const VertexLabel &node_from,
                    const VertexLabel &node_to,
                    const bool destructive = false)
        {
            /** Get the vertex descriptor **/
            envire::core::LabeledTransformTree::vertex_descriptor v_from =
                boost::vertex_by_label(node_from, *this);

            /** Get the vertex descriptor **/
            envire::core::LabeledTransformTree::vertex_descriptor v_to =
                boost::vertex_by_label(node_to, *this);

            return this->remove_edge(v_from, v_to, destructive);
        }

        /**@brief Remove an Edge from the Tree
         * Remove an edge using the edge descriptor.
         * In case destructive is true, the associated
         * vertex are also remove ONLY in case they do not
         * have other connexions.
         */
        inline void remove_edge(const LabeledTransformTree::vertex_descriptor node_from,
                    const LabeledTransformTree::vertex_descriptor node_to,
                    const bool destructive = false)
        {
            boost::remove_edge(node_from, node_to, *this);

            if (destructive == true)
            {
                if (boost::degree(node_from, *this) == 0)
                {
                    boost::remove_vertex(node_from, this->graph());
                }

                if (boost::in_degree(node_to, *this) == 0)
                {
                    boost::remove_vertex(node_to, this->graph());
                }
            }
            return;
        }

        /**@brief Get all edges
         */
        inline std::pair<LabeledTransformTree::edge_iterator, LabeledTransformTree::edge_iterator>
        edges()
        {
            return boost::edges(*this);
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

        inline std::pair<in_edge_iterator,in_edge_iterator>
        in_edges(const LabeledTransformTree::vertex_descriptor &node)
        {
            return boost::in_edges(node, *this);
        }

        inline std::pair<out_edge_iterator,out_edge_iterator>
        out_edges(const LabeledTransformTree::vertex_descriptor &node)
        {
            return boost::out_edges(node, *this);
        }

        inline LabeledTransformTree::edges_size_type num_edges()
        {
            return  boost::num_edges(*this);
        }


        /** PROPERTIES METHODS **/

        /**@brief Add a vertex to the tree
        */
        inline envire::core::Frame getFrame(const TransformTree::vertex_descriptor vd)
        {
            return boost::get(&FrameProperty::frame, *this, vd);
        }

        /**@brief Add a vertex to the tree
        */
        inline envire::core::Transform getTransform(const LabeledTransformTree::edge_descriptor ed)
        {
            return boost::get(&TransformProperty::transform, *this, ed);
        }


        /** GENERAL PURPOSE METHODS **/

        /**@brief clear
         *
         * Remove all of the edges and vertices from the graph.
         */
        inline void clear(){ return this->graph().clear(); }
    };
}}
#endif
