/*
 * GraphExceptions.hpp
 *
 *  Created on: Sep 22, 2015
 *      Author: aboeckmann
 */

#pragma once
#include <stdexcept>
#include <string>
#include <envire_core/items/Frame.hpp>
#include <boost/uuid/uuid.hpp>

namespace envire { namespace core
{
    
    class EdgeAlreadyExistsException : public std::exception
    {
    public:
        explicit EdgeAlreadyExistsException(const FrameId& nameA, const FrameId& nameB) :
          msg("Edge between " + nameA + " and " + nameB + " already exists") {}
        virtual char const * what() const throw() { return msg.c_str(); }
        const std::string msg;
    };

    class UnknownTransformException : public std::exception
    {
    public:
      explicit UnknownTransformException(const FrameId& nameA, const FrameId& nameB) :
          msg("Transform between " + nameA + " and " + nameB + " doesn't exists") {}
        virtual char const * what() const throw() { return msg.c_str(); }
        const std::string msg;
    };
    
    class UnknownEdgeException : public std::exception
    {
    public:
      explicit UnknownEdgeException(const FrameId& nameA, const FrameId& nameB) :
          msg("Edge between " + nameA + " and " + nameB + " doesn't exists") {}
        virtual char const * what() const throw() { return msg.c_str(); }
        const std::string msg;
    };

    class UnknownFrameException : public std::exception
    {
    public:
        explicit UnknownFrameException(const FrameId& name) :
          msg("Frame " + name + " doesn't exist") {}
        virtual char const * what() const throw() { return msg.c_str(); }
        const std::string msg;
    };

    class FoundFrameException : public std::exception
    {
    public:
        explicit FoundFrameException(const FrameId& name) :
          msg("Found Frame " + name) {}
        virtual char const * what() const throw() { return msg.c_str(); }
        const std::string msg;
    };
    
    class FrameAlreadyExistsException : public std::exception
    {
    public:
        explicit FrameAlreadyExistsException(const FrameId& name) :
          msg("Frame " + name + " already exists") {}
        virtual char const * what() const throw() { return msg.c_str(); }
        const std::string msg;
    };
    
    class UnknownItemException : public std::exception
    {
    public:
        explicit UnknownItemException(const FrameId& name, const boost::uuids::uuid& uuid) :
          msg("The item with uuid '" + boost::uuids::to_string(uuid) + 
              "' is not part of frame '" + name + "'") {}
        virtual char const * what() const throw() { return msg.c_str(); }
        const std::string msg;
    };

    class FrameStillConnectedException : public std::exception
    {
    public:
        explicit FrameStillConnectedException(const FrameId& name) :
          msg("Frame " + name + " is stil connected to the graph. All transforms"
              " coming from or leading to this frame need to be removed"
              " before removing the frame") {}
        virtual char const * what() const throw() { return msg.c_str(); }
        const std::string msg;
    };   
        
    class NoItemsOfTypeInFrameException : public std::exception
    {
    public:
        explicit NoItemsOfTypeInFrameException(const FrameId& name, const std::string& type_name) :
          msg("There are no items of type '" + type_name + "' in frame '" + name + "'") {}
        virtual char const * what() const throw() { return msg.c_str(); }
        const std::string msg;
    };
    
    class NullVertexException : public std::exception
    {
    public:
        explicit NullVertexException() :
          msg("Encountered a null_vertex.") {}
        virtual char const * what() const throw() { return msg.c_str(); }
        const std::string msg;
    };
    
    
}}


