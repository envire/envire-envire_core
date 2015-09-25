#ifndef __ENVIRE_CORE_FRAME__
#define __ENVIRE_CORE_FRAME__

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <vector>
#include <string>

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
        std::vector< ItemBase::Ptr > items; /** List of items in the node */

    public:
        Frame() : name("envire::core::noname"),
            uuid(RandomGenerator::getRandomGenerator()()) {}
        Frame(const std::string &_name): 
            name(_name), uuid(RandomGenerator::getRandomGenerator()()) {}

        ~Frame(){ this->items.clear(); }

        /**@brief setFrame
        *
        * Sets the frame name of the item
        */
        void setName(const std::string& name) { this->name = name; }

        /**@brief getFrame
        *
        * Returns the frame name of the item
        */
        const std::string& getName() const { return this->name; }
    };
}}
#endif
