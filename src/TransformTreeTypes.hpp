/*
 * TransformTreeTypes.hpp
 *
 *  Created on: Sep 16, 2015
 *      Author: aboeckmann
 *
 *  This file is needed to break a circular header dependency between
 *  TransformTree and FrameEventArgs.
 */

#ifndef SRC_TRANSFORMTREETYPES_HPP_
#define SRC_TRANSFORMTREETYPES_HPP_


#include "Frame.hpp" /** Frames are for the Vertex **/
#include "Transform.hpp" /** Transform are the Edges **/
#include "Environment.hpp" /** Environment is the tree property **/

#include <boost/graph/directed_graph.hpp> /** Boost directed graph **/
namespace envire { namespace core
{

    /**@brief Frame Property
     * boost access tag for the Frame property
     */
    struct FrameProperty
    {
        Frame frame;
    };

    /**@brief Transform Property
     * boost access tag for the Transform property
     */
    struct TransformProperty
    {
        Transform transform;
    };


    /**@brief The Transform Graph type
    *
    * The basic class for the Transform Graph
    *
    */
    typedef boost::directed_graph<
        FrameProperty,
        TransformProperty,
        envire::core::Environment> TransformGraph;

    typedef TransformGraph::vertex_descriptor vertex_descriptor;
    typedef TransformGraph::edge_descriptor edge_descriptor;
    //TODO add the others if this works

}}
#endif /* SRC_TRANSFORMTREETYPES_HPP_ */
