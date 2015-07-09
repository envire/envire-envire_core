#ifndef __ENVIRE_CORE_NODE__
#define __ENVIRE_CORE_NODE__

#include <boost/uuid/uuid.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>
#include <string>

#include <envire_core/ItemBase.hpp>

namespace envire { namespace core
{
    /**@class Node
    *
    * Node class
    */
    class Node
    {
    public:
        boost::uuids::uuid uuid; /** Unique Identifier */
        std::string frame_name; /** Frame name */
        std::vector< boost::shared_ptr<ItemBase> > items; /** List of items in the node */

    public:
        Node();
        Node(const std::string &frame_name);
        ~Node();

        /**@brief setFrame
        *
        * Sets the frame name of the item
        *
        */
        void setFrame(const std::string& frame_name) { this->frame_name = frame_name; }

        /**@brief getFrame
        *
        * Returns the frame name of the item
        *
        */
        const std::string& getFrame() const { return this->frame_name; }

   };
}}
#endif
