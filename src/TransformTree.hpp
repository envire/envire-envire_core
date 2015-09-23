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
#include <string>


#include "TransformTreeTypes.hpp"
#include "TransformTreeExceptions.hpp"
#include "events/TreeEventPublisher.hpp"
#include "events/TransformAddedEvent.hpp"
#include "events/TransformRemovedEvent.hpp"
#include "events/TransformModifiedEvent.hpp"

namespace envire { namespace core
{
    class GraphViz;
    /**
     * A tree-like graph structure.
     * Each vertex contains a labeled Frame. The label must be unique.
     *
     * @note The inheritance from TransformGraph is protected to stop
     *       users from calling boost::graph methods directly.
     *       This way the tree methods are the only way to manipulate
     *       the vertices and edges.
     *       This is done to ensure that
    */
    class TransformTree : protected LabeledTransformGraph, public TreeEventPublisher
    {
        //GraphViz has been implemented for TransformGraph.
        //As it only reads and does no manipulation it is ok to let it know
        //about the inheritance from TransformGraph
        friend class GraphViz;
    public:

        TransformTree(envire::core::Environment const &environment = Environment()):
          LabeledTransformGraph (environment)
        {}


        /**Adds a transform from frame @p a to frame @p b.
         * If the frames do not exist, they will be created.
         * If the transform already exists, it will **not** be updated.
         *
         * Causes a TransformAdded event.
         *
         * The inverse transform will be added automatically.
         *
         * @throw TransformAlreadyExistsException if the transformation already exists.*/
        void addTransform(const FrameId& origin, const FrameId& target,
                          const Transform& tf);

        /**Updates the value of the transform from @p origin to
         * @p target and the inverse transformation according to @p tf.
         * Both frames need to exist beforehand.
         * A direct transformation has to exist between @p orign and @p target
         * for this method to work.
         *
         * Causes two TransformUpdated events. One for the
         * transform and one for the inverse.
         *
         * @throw UnknownTransformException if no direct transformation between
         *        @p origin and @p target exists. */
        void updateTransform(const FrameId& origin, const FrameId& target,
                             const Transform& tf);

        /**Removes the transform between @p origin and @p target as well as
         * the transform between @p target and @p origin.
         * If frames are left unconnected after the transform has been removed, they
         * will be removed as well.
         *
         * Causes TransformRemoved event.
         *
         * @throw UnknownTransformException if the transformation doesn't exist */
        void removeTransform(const FrameId& origin, const FrameId& target);

        /**@return the transform between a and b. Calculating it if necessary.
         * @throw UnknownTransformException if the transformation doesn't exist*/
        const Transform& getTransform(const FrameId& a, const FrameId& b) const;

        /**@return the edge between frame @p origin and @p target
         * @throw UnknownTransformException if there is no such edge  */
        edge_descriptor getEdge(const FrameId& origin, const FrameId& target) const;

        /***************************************************
         * Methods Naming convention
         * Overloading boost methods uses delimited separated
         * words, new methods use Camel Case separated words
         ***************************************************/


        /** @return a reference to the frame identified by the id.
         *  @throw UnknownFrameException if the frame id is invalid **/
        const envire::core::Frame& getFrame(const FrameId& frame);

        /**@brief Get all edges */
        std::pair<edge_iterator, edge_iterator> edges();

        /**@brief Get the source of an edge */
        vertex_descriptor source(const edge_descriptor it_node);

        /**@brief Get the target of an edge */
        vertex_descriptor target(const edge_descriptor it_node);

        /**@brief Get all vertices
         */
        std::pair<vertex_iterator, vertex_iterator>
        vertices();

        /**@brief Add an Edge
         * Add an edge between two vertices.
         *
         * Causes TransformAddedEvent or TransformModifiedEvent.
         *
         * @return an edge descriptor pointing to the new edge and a boolean.
         *         The boolean is false if the edge already existed.
         *         In that case no new edge was added, instead the existing
         *         edge was updated.
         */
        edge_descriptor add_edge(const vertex_descriptor node_from,
                                 const vertex_descriptor node_to,
                                 const envire::core::Transform &tf);


        void remove_edge(edge_descriptor e);

        vertices_size_type num_vertices() const;
        edges_size_type num_edges() const;


        /**@return the total number of edges connected to @param v.
         *         I.e. the sum of the in and out edges.
         */
        degree_size_type degree(const vertex_descriptor v) const;

        /**@return the vertex identified by @param label */
        vertex_descriptor vertex(const std::string& label) const
        {
            return LabeledTransformGraph::vertex(label);
        }

    protected:
         /**@brief Add a vertex
         * @note the frame's name must be unique. */
        vertex_descriptor add_vertex(const FrameId& frameId);

        /**Removes a vertex from the tree.
         * A vertex can only be removed if there are no edges to
         * and from the vertex. Removing a vertex that still has edges
         * attached will result in undefined behavior. */
        void remove_frame(FrameId fId);

        /**Sets the transform value and causes transformModified event. */
        void updateTransform(edge_descriptor ed, const Transform& tf,
                             const FrameId& origin, const FrameId& target);

    };
}}
#endif
