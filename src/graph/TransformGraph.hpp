/**\file TransformTree.hpp
 *
 * This class provided stores and manages the transformation tree
 *
 * @author Javier Hidalgo Carrio et. al (see THANKS  for the full Author list)
 * See LICENSE for the license information
 *
 */


#ifndef __ENVIRE_CORE_TRANSFORM_GRAPH__
#define __ENVIRE_CORE_TRANSFORM_GRAPH__


#include <boost/uuid/uuid.hpp>
#include <cassert>
#include <string>


#include "TransformGraphTypes.hpp"
#include "TransformGraphExceptions.hpp"
#include "TransformGraphVisitors.hpp"
#include <envire_core/events/GraphEventPublisher.hpp>


namespace envire { namespace core
{
    /**
     * A tree-like graph structure.
     * Each vertex contains a labeled Frame. The label must be unique.
     *
     * @warning Do **not** manipulate the tree directly using boost functions.
     *          If you do that you will **break** the event system.
     *          Instead use the methods provided by this class for manipulation.
     *          I.e. do not modify transforms, frames or items directly!!!!
     *          
    */
    class TransformGraph : public LabeledTransformGraph, public GraphEventPublisher
    {
    public:

        /***************************************************
         * Methods Naming convention
         * Overloading boost methods uses delimited separated
         * words, new methods use Camel Case separated words
         ***************************************************/
    
        TransformGraph(envire::core::Environment const &environment = Environment());
        
        
        /** Adds an unconnected frame to the graph.
         * 
         * @throw FrameAlreadyExistsException if the frame already exists
         */
        void addFrame(const FrameId& frame);

        /**Adds a transform from frame @p origin to frame @p target.
         * If the frames do not exist, they will be created.
         * If the transform already exists, it will **not** be updated.
         *
         * Causes a TransformAddedEvent.
         * Causes FrameAddedEvents if the frames did not exist before. Those events
         * will occur **before** the TransformAddedEvent.
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
         * Causes a TransformUpdated event.
         *
         * @throw UnknownTransformException if no direct transformation between
         *                                  @p origin and @p target exists. */
        void updateTransform(const FrameId& origin, const FrameId& target,
                             const Transform& tf);

        /**Removes the transform between @p origin and @p target as well as
         * the transform between @p target and @p origin.
         * If frames are left unconnected after the transform has been removed, they
         * will be removed as well.
         *
         * Causes TransformRemoved event.
         * Causes ItemRemovedEvent for each item of the frame if a frame is removed.
         *
         * @throw UnknownTransformException if the transformation doesn't exist */
        void removeTransform(const FrameId& origin, const FrameId& target);

        /** @return the transform between a and b. Calculating it if necessary.
         * @throw UnknownTransformException if the transformation doesn't exist*/
        const Transform getTransform(const FrameId& a, const FrameId& b) const;

        /** @return the edge between frame @p origin and @p target
         * @throw UnknownTransformException if there is no such edge  */
        edge_descriptor getEdge(const FrameId& origin, const FrameId& target) const;

        /** @return a reference to the frame identified by the id.
         *  @throw UnknownFrameException if the frame id is invalid **/
        const envire::core::Frame& getFrame(const FrameId& frame) const;
        const envire::core::Frame& getFrame(const vertex_descriptor desc) const;
        
        /** @return the frame id of the specified @p vertex */
        const envire::core::FrameId& getFrameId(const vertex_descriptor vertex) const;
        
        /** Adds @p item to the item list of the specified frame 
         *  Causes ItemAddedEvent.
         *  @throw UnknownFrameException if the frame id is invalid **/
        void addItemToFrame(const FrameId& frame, ItemBase::Ptr item);
        
        /** Removes @p item from @p frame.
         *  Causes ItemRemovedEvent.
         * @throw UnknownFrameException if the frame does not exist.
         * @throw UnknownItemException if the item is not part of the frame's
         *                             item list.  */ 
        void removeItemFromFrame(const FrameId& frame, ItemBase::Ptr item);
        
        /** @return a list of items that are attached to the specified @p frame.
         *  @throw UnknownFrameException if the @p frame id is invalid.*/
        const std::vector<ItemBase::Ptr>& getItems(const FrameId& frame) const;
        const std::vector<ItemBase::Ptr>& getItems(const vertex_descriptor desc) const;
        
        /**Builds a tree containing all vertices that are accessible starting
         * from @p root.  */
        VertexMap getTree(const vertex_descriptor root) const;

        vertices_size_type num_vertices() const;
        edges_size_type num_edges() const;

    protected:
         /**@brief Add a vertex
         * @note the frame's name must be unique. */
        vertex_descriptor add_vertex(const FrameId& frameId);

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
                                 const envire::core::Transform &tf,
                                 const FrameId& originName,
                                 const FrameId& targetName);

        /**Removes a vertex from the tree.
         * A vertex can only be removed if there are no edges to
         * and from the vertex. Removing a vertex that still has edges
         * attached will result in undefined behavior. */
        void remove_frame(FrameId fId);

        /**Sets the transform value and causes transformModified event. */
        void updateTransform(edge_descriptor ed, const Transform& tf);
    };
}}
#endif
