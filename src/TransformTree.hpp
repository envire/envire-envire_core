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

#include "TransformTreeTypes.hpp"
#include "events/FrameAddedEvent.hpp"
#include "events/TreeEventPublisher.hpp"
#include "events/VertexAddedEvent.hpp"
#include "events/VertexRemovedEvent.hpp"

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
    class TransformTree : protected TransformGraph, public TreeEventPublisher
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


        /**Adds @param frame below @param parent to the tree.
         *
         * Causes an FrameAddedEvent.
         * Does not cause VertexAdded or EdgeAdded events. */
        vertex_descriptor addFrame(const envire::core::Frame &frame,
            vertex_descriptor parent, const envire::core::Transform &tf)
        {
            disableEvents();//otherwise add_vertex and add_edge will cause events

            vertex_descriptor newNode = add_vertex(frame);
            edge_descriptor newEdge;
            bool edgeAdded = false;
            boost::tie(newEdge, edgeAdded) = add_edge(parent, newNode, tf);
            assert(edgeAdded);

            enableEvents();
            notify(FrameAddedEvent(parent, newNode, tf));
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
         *
         * causes a VertexAddedEvent
         */
        vertex_descriptor add_vertex(const envire::core::Frame &frame)
        {
            FrameProperty node_prop;
            node_prop.frame = frame;
            vertex_descriptor v = boost::add_vertex(node_prop, *this);
            notify(VertexAddedEvent(v));
            return v;
        }

        /**Removes a vertex from the tree.
         * A vertex can only be removed if there are no edges to
         * and from the vertex. Removing a vertex that still has edges
         * attached will result in undefined behavior.
         *
         * If you want to remove a vertex and all corresponding edges
         * you should consider removeFrame() */
        void remove_vertex(vertex_descriptor v)
        {
            assert(degree(v) <= 0);
            TransformGraph::remove_vertex(v);
            notify(VertexRemovedEvent());
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

        /**@return the total number of edges connected to @param v.
         *         I.e. the sum of the in and out edges.
         */
        degree_size_type degree(const vertex_descriptor v) const
        {
          return boost::degree(v, *this);
        }
    };
}}
#endif
