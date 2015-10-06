/*
 * TransformGraphExceptions.hpp
 *
 *  Created on: Sep 22, 2015
 *      Author: aboeckmann
 */

#pragma once
#include <stdexcept>
#include <string>
#include <envire_core/items/Frame.hpp>

namespace envire { namespace core
{

    class TransformAlreadyExistsException : public std::exception
    {
    public:
        TransformAlreadyExistsException(const FrameId& nameA, const FrameId& nameB) :
          msg("Transform between " + nameA + " and " + nameB + " already exists") {}
        virtual char const * what() const throw() { return msg.c_str(); }
        const std::string msg;
    };

    class UnknownTransformException : public std::exception
    {
    public:
      UnknownTransformException(const FrameId& nameA, const FrameId& nameB) :
          msg("Transform between " + nameA + " and " + nameB + " doesn't exists") {}
        virtual char const * what() const throw() { return msg.c_str(); }
        const std::string msg;
    };

    class UnknownFrameException : public std::exception
    {
    public:
        UnknownFrameException(const FrameId& name) :
          msg("Frame " + name + " doesn't exist") {}
        virtual char const * what() const throw() { return msg.c_str(); }
        const std::string msg;
    };

    class FoundFrameException : public std::exception
    {
    public:
        FoundFrameException(const FrameId& name) :
          msg("Found Frame " + name) {}
        virtual char const * what() const throw() { return msg.c_str(); }
        const std::string msg;
    };
    
    class FrameAlreadyExistsException : public std::exception
    {
    public:
        FrameAlreadyExistsException(const FrameId& name) :
          msg("Frame " + name + " already exists") {}
        virtual char const * what() const throw() { return msg.c_str(); }
        const std::string msg;
    };
    
    class UnknownItemException : public std::exception
    {
    public:
        UnknownItemException(const FrameId& name, ItemBase::Ptr item) :
          msg("The item with uuid '" + item->getIDString() + 
              "' is not part of frame '" + name + "'") {}
        virtual char const * what() const throw() { return msg.c_str(); }
        const std::string msg;
    };
    
    class FrameStillConnectedException : public std::exception
    {
    public:
        FrameStillConnectedException(const FrameId& name) :
          msg("Frame " + name + " is stil connected to the graph. All transforms"
              " coming from or leading to this frame need to be removed"
              " before removing the frame") {}
        virtual char const * what() const throw() { return msg.c_str(); }
        const std::string msg;
    };   
    


}}


