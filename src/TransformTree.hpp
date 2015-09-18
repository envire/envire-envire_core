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


#include <boost/uuid/uuid.hpp>
#include <cassert>

#include "FrameEventPublisher.hpp"
#include "TransformTreeTypes.hpp"
#include "FrameAddedEvent.hpp"
#include "FrameRootAddedEvent.hpp"

namespace envire { namespace core
{
    class GraphViz;
    /**
     *
     * @note The inheritance from TransformGraph is protected to stop
     *       users from calling boost::graph methods directly.
     *       This way the tree methods are the only way to manipulate
     *       the vertices and edges.
     *       This is done to ensure that
    */
    class TransformTree : protected TransformGraph, public FrameEventPublisher
    {
        //GraphViz has been implemented for TransformGraph.
        //As it only reads and does no manipulation it is ok to let it know
        //about the inheritance from TransformGraph
        friend class GraphViz;
    public:

        TransformTree(envire::core::Environment const &environment = Environment()):
                        TransformGraph (environment)
        {}

        /***************************************************
         * Methods Naming convention
         * Overloading boost methods uses delimited separated
         * words, new methods use Camel Case separated words
         ***************************************************/


        /**Adds @param frame below @param parent to the tree */
        vertex_descriptor addFrame(const envire::core::Frame &frame,
            vertex_descriptor parent, const envire::core::Transform &tf)
        {
            vertex_descriptor newNode = add_vertex(frame);
            edge_descriptor newEdge;
            bool edgeAdded = false;
            boost::tie(newEdge, edgeAdded) = add_edge(parent, newNode, tf);
            assert(edgeAdded);
            notify(FrameAddedEvent(parent, newNode, tf));
            return newNode;
        }

        /**Adds @param frame to the tree without connecting it to any existing
         * frame. This method can be used to add new root nodes to the tree. */
        vertex_descriptor addRootFrame(const envire::core::Frame &frame)
        {
          vertex_descriptor newNode = add_vertex(frame);
          notify(FrameRootAddedEvent(newNode));
          return newNode;
        }

        /**@brief Get all edges
         */
        std::pair<edge_iterator, edge_iterator>
        edges()
        {
            return boost::edges(*this);
        }

        /**@brief source of an edge
         *
         * Get source vertex descriptor for edge descriptor
         * */
        vertex_descriptor source(const edge_descriptor it_node)
        {
            return boost::source(it_node, *this);
        }

        /**@brief target of an edge
         *
         * Get target vertex descriptor for edge descriptor
         * */
        vertex_descriptor target(const edge_descriptor it_node)
        {
            return boost::target(it_node, *this);
        }

        /** PROPERTIES METHODS **/

        /** @return a reference to the frame that is attached to the specified vertex.**/
        envire::core::Frame& getFrame(const vertex_descriptor& vd)
        {
          return (*this)[vd].frame;
        }

        /** @return a reference to the frame that is attached to the specified vertex.**/
        envire::core::Frame& getFrame(const vertex_iterator vi)
        {
          return getFrame(*vi);
        }

        /**@brief get Transform
         *
         * Transform associated to an edge
         * */
        envire::core::Transform& getTransform(const edge_descriptor& ed)
        {
            return (*this)[ed].transform;
        }

        /**@brief get Transform
         *
         * Transform associated to an edge
         * */
        envire::core::Transform& getTransform(const edge_iterator ei)
        {
            return getTransform(*ei);
        }

        /**@brief sets the transform property of the specified edge to the
         *        specified value.
         * @param ei The edge whose transform property should be modified
         * @param tf The new transform value
         */
        void setTransform(edge_iterator ei, const Transform& tf)
        {
          boost::put(&TransformProperty::transform, *this, *ei, tf);
        }

        /**@brief Add a vertex
         */
        vertex_descriptor add_vertex(const envire::core::Frame &frame)
        {
            FrameProperty node_prop;
            node_prop.frame = frame;
            vertex_descriptor v = boost::add_vertex(node_prop, *this);
            return v;
        }

        void remove_vertex(vertex_descriptor v)
        {
            TransformGraph::remove_vertex(v);
        }

        void remove_vertex(vertex_iterator vi)
        {
            TransformGraph::remove_vertex(*vi);
        }

        /**@brief Get a vertex
        */
        vertex_descriptor vertex(const vertices_size_type i) const
        {
            return boost::vertex(i, *this);
        }

        /**@brief Get all vertices
         */
        std::pair<vertex_iterator, vertex_iterator>
        vertices()
        {
            return boost::vertices(*this);
        }


        /** EDGES METHODS **/

        /**@brief Add an Edge
         * Add an edge between two vertices.
         * @return an edge descriptor pointing to the new edge and a boolean.
         *         The boolean is false if the edge already existed.
         *         In that case no new edge was added, instead the existing
         *         edge was updated.
         */
        std::pair<edge_descriptor, bool>
        add_edge(const vertex_descriptor node_from,
                    const vertex_descriptor node_to,
                    const envire::core::Transform &tf = envire::core::Transform())
        {
            /* Don't allow parallel edges **/
            std::pair<envire::core::edge_descriptor, bool> edge_pair =
                boost::edge(node_from, node_to, *this);
            //second is true if the edge exists
            /** Update the edge in case it already exists **/
            if (edge_pair.second)
            {
                boost::put(&TransformProperty::transform, *this, edge_pair.first, tf);
                edge_pair.second = false;//to comply with the interface of add_edge
            }
            else
            {
                /** TO-DO: Avoid loops? **/
               // if (boost::in_degree(node_to, *this) == 0)
               // {
                    /** Add the new edge **/
                    TransformProperty tf_prop;
                    tf_prop.transform = tf;
                    edge_pair =  boost::add_edge(node_from, node_to, tf_prop, *this);
               // }
               // else
               // {
               //     throw std::logic_error("envire::core::TransformTree: No loop closure!");
               // }
            }
            return edge_pair;
        }

        void remove_edge(edge_descriptor e)
        {
            TransformGraph::remove_edge(e);
        }

        vertices_size_type num_vertices() const
        {
            return TransformGraph::num_vertices();
        }

        edges_size_type num_edges() const
        {
            return TransformGraph::num_edges();
        }



    };
}}
#endif
