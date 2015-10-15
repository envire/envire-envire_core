#ifndef __ENVIRE_CORE_FRAME__
#define __ENVIRE_CORE_FRAME__

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <vector>
#include <string>
#include <unordered_map>
#include <typeindex>
    
#include "ItemBase.hpp"
#include "RandomGenerator.hpp"

namespace envire { namespace core
{
    using FrameId = std::string;
    
    //TODO comment
    class Frame
    {
    public:
        FrameId name; /** Frame name */
        boost::uuids::uuid uuid; /** Unique Identifier */

        using ItemList = std::vector<ItemBase::Ptr>;
        //contains all items that have been added to the frame sorted by type
        using ItemMap = std::unordered_map<std::type_index, ItemList>;
        ItemMap items;

    public:
        Frame(const FrameId& _name): 
            name(_name), uuid(RandomGenerator::getRandomGenerator()()) {}

        ~Frame(){ this->items.clear(); }

        /**@brief setFrame
        *
        * Sets the frame name of the item
        */
        void setName(const FrameId& name) { this->name = name; }

        /**@brief getFrame
        *
        * Returns the frame name of the item
        */
        const FrameId& getName() const { return this->name; }
    };
}}
#endif
