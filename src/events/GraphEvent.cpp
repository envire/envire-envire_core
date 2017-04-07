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

#include <envire_core/events/GraphEvent.hpp>
#include <ostream>

namespace envire { namespace core
{

std::ostream& operator<<(std::ostream& ostream, const GraphEvent& graph_event)
{
    switch(graph_event.getType())
    {
        case GraphEvent::EDGE_ADDED:
            ostream << "EDGE_ADDED";
            break;
        case GraphEvent::EDGE_MODIFIED:
            ostream << "EDGE_MODIFIED";
            break;
        case GraphEvent::EDGE_REMOVED:
            ostream << "EDGE_REMOVED";
            break;
        case GraphEvent::FRAME_ADDED:
            ostream << "FRAME_ADDED";
            break;
        case GraphEvent::FRAME_REMOVED:
            ostream << "FRAME_REMOVED";
            break;
        case GraphEvent::ITEM_ADDED_TO_FRAME:
            ostream << "ITEM_ADDED_TO_FRAME";
            break;
        case GraphEvent::ITEM_REMOVED_FROM_FRAME:
            ostream << "ITEM_REMOVED_FROM_FRAME";
    }
    return ostream;
}

}}