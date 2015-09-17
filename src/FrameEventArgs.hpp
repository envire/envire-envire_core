/*
 * FrameEventArgs.hpp
 *
 *  Created on: Sep 16, 2015
 *      Author: aboeckmann
 */

#pragma once
#ifndef __FRAME_EVENT_ARGS_HPP__
#define __FRAME_EVENT_ARGS_HPP__
#include "Frame.hpp"
#include "TransformTreeTypes.hpp"
#include "Transform.hpp"

namespace envire { namespace core
{
    struct FrameAddedEventArgs
    {
      FrameAddedEventArgs(vertex_descriptor& added, vertex_descriptor& parent,
                          const Transform& tf) : addedVertex(added),
                                                 parentVertex(parent),
                                                 transform(tf){}
      vertex_descriptor addedVertex; /**<The vertex containing the newly added frame */
      vertex_descriptor parentVertex; /**<The parent vertex of the newly added vertex */
      Transform transform; /**<Transformation between parent and newly added frame */
    };

    struct FrameModifedEventArgs
    {

    };

    struct FrameRemovedEventArgs
    {

    };
}}


#endif /* SRC_EVENT_EVENTARGS_HPP_ */
