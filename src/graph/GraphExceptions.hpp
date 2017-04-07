//
// Copyright (c) 2015, Deutsches Forschungszentrum für Künstliche Intelligenz GmbH.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#pragma once

#include <stdexcept>
#include <string>
#include <envire_core/items/Frame.hpp>
#include <boost/uuid/uuid.hpp>

namespace envire { namespace core
{
    
  
  
    class InvalidPathException : public std::exception
    {
    public:
        explicit InvalidPathException() :
          msg("Invalid Path") {}
        virtual char const * what() const throw() { return msg.c_str(); }
        const std::string msg;
    };
    
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


