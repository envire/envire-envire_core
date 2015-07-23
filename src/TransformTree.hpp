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

#include <boost/graph/directed_graph.hpp> /** Boost directed graph **/
#include <boost/uuid/uuid.hpp> /** uuid class */

#include <envire_core/Frame.hpp> /** Frames are for the Vertex **/
#include <envire_core/Transform.hpp> /** Transform are the Edges **/
#include <envire_core/Environment.hpp> /** Environment is the tree property **/

namespace envire { namespace core
{
    /**@brief Frame Property Tag
     * boost access tag for the Frame property
     */
    struct FramePropertyTag
    {
        typedef boost::vertex_property_tag kind;
        static  std::size_t const num;
    };

    typedef boost::property <FramePropertyTag, envire::core::Frame>  FrameProperty;

    /**@brief Transform Property Tag
     * boost access tag for the Transform property
     */
    struct TransformPropertyTag
    {
        typedef boost::edge_property_tag kind;
        static  std::size_t const num;
    };

    typedef boost::property <TransformPropertyTag, envire::core::Transform>  TransformProperty;

    /**@brief The Transform Graph type
    *
    * The basic class for the Transform Graph
    *
    */
    typedef boost::directed_graph<
        FrameProperty,
        TransformProperty,
        envire::core::Environment> TransformGraph;

    /**@class Transformation Tree
    *
    * Transformation Tree class
    */
    class TransformTree: public TransformGraph
    {
    public:
        typedef typename boost::property_map <TransformGraph, FramePropertyTag>::type FrameMap;
        typedef typename boost::property_map <TransformGraph, TransformPropertyTag>::type TransformMap;

    public:

        TransformTree(envire::core::Environment const &environment = Environment()):
                        TransformGraph (environment)
        {
        }

        /***************************************************
         * Methods Naming convention
         * Overloading boost methods uses delimited separated
         * words, new methods use Camel Case separated words
         ***************************************************/

        /** VERTEX METHODS **/


        /**@brief Add a vertex
         */
        inline TransformTree::vertex_descriptor add_vertex(const FrameProperty &frame)
        {
            TransformTree::vertex_descriptor v = boost::add_vertex(frame, *this);
            return v;
        }

        /**@brief Get a vertex
        */
        inline TransformTree::vertex_descriptor vertex(const TransformTree::vertices_size_type i)
        {
            return boost::vertex(i, *this);
        }

        /**@brief Get all vertices
         */
        inline std::pair<TransformTree::vertex_iterator, TransformTree::vertex_iterator>
        vertices()
        {
            return boost::vertices(*this);
        }


        /** EDGES METHODS **/

        /**@brief Add an Edge
         * Add an edge between two vertex
         */
        inline std::pair<TransformTree::edge_descriptor, bool>
        add_edge(const TransformTree::vertex_descriptor node_from,
                    const TransformTree::vertex_descriptor node_to,
                    const envire::core::Transform &tf = envire::core::Transform())
        {
            /* Don't allow parallel edges **/
            std::pair<envire::core::TransformTree::edge_descriptor, bool> edge_pair =
                boost::edge(node_from, node_to, *this);

            /** Update the edge in case it already exists **/
            if (edge_pair.second)
            {
                boost::put(TransformPropertyTag(), *this, edge_pair.first, tf);
            }
            else
            {
                /** TO-DO: Avoid loops? **/
               // if (boost::in_degree(node_to, *this) == 0)
               // {
                    /** Add the new edge **/
                    edge_pair =  boost::add_edge(node_from, node_to, tf, *this);
               // }
               // else
               // {
               //     throw std::logic_error("envire::core::TransformTree: No loop closure!");
               // }
            }
            return edge_pair;
        }


        /**@brief Get all edges
         */
        inline std::pair<TransformTree::edge_iterator, TransformTree::edge_iterator>
        edges()
        {
            return boost::edges(*this);
        }

        /**@brief source of an edge
         *
         * Get source vertex descriptor for edge descriptor
         * */
        inline TransformTree::vertex_descriptor source(const TransformTree::edge_descriptor it_node)
        {
            return boost::source(it_node, *this);
        }

        /**@brief target of an edge
         *
         * Get target vertex descriptor for edge descriptor
         * */
        inline TransformTree::vertex_descriptor target(const TransformTree::edge_descriptor it_node)
        {
            return boost::target(it_node, *this);
        }

        /** PROPERTIES METHODS **/

        /**@brief get Frame
         *
         * Frame associated to a node
         * */
        envire::core::Frame getFrame(const TransformTree::vertex_descriptor vd)
        {
            return boost::get(FramePropertyTag(), *this, vd);
        }

        /**@brief get Frame
         *
         * Frame associated to a node
         * */
        envire::core::Frame getFrame(const TransformTree::vertex_iterator vi)
        {
            return boost::get(FramePropertyTag(), *this, *vi);
        }

        /**@brief get Transform
         *
         * Transform associated to an edge
         * */
        envire::core::Transform getTransform(const TransformTree::edge_descriptor ed)
        {
            return boost::get(TransformPropertyTag(), *this, ed);
        }

        /**@brief get Transform
         *
         * Transform associated to an edge
         * */
        envire::core::Transform getTransform(const TransformTree::edge_iterator ei)
        {
            return boost::get(TransformPropertyTag(), *this, *ei);
        }
    };
}}
#endif
