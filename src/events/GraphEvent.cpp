#include "GraphEvent.hpp"
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
}

}}