#ifndef __ENVIRE_CORE_FRAME__
#define __ENVIRE_CORE_FRAME__

#include <boost/uuid/uuid.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>
#include <string>

#include <envire_core/ItemBase.hpp>

namespace envire { namespace core
{
    /**@class Frame
    *
    * Frame class
    */
    class Frame
    {
    public:
        std::string name; /** Frame name */
        boost::uuids::uuid uuid; /** Unique Identifier */
        std::vector< boost::shared_ptr<ItemBase> > items; /** List of items in the node */

    public:
        Frame();
        Frame(const std::string &name);
        ~Frame();

        /**@brief setFrame
        *
        * Sets the frame name of the item
        *
        */
        void setName(const std::string& name) { this->name = name; }

        /**@brief getFrame
        *
        * Returns the frame name of the item
        *
        */
        const std::string& getName() const { return this->name; }

   };

    /** Frame Graph Viz property writer for boost graphs **/
    template <class _FrameName, class _ID, class _Items>
    class FrameWriter
    {
    public:
        FrameWriter(_FrameName name, _ID id, _Items it) :name(name), id(id), it(it){}
        template <class _Vertex>
        void operator()(std::ostream &out, const _Vertex& n) const
        {
            out << "[shape=record, label=\"<f0> " << name[n] <<
                "|<f1>" << it[n].size()<<"\""
                <<",style=filled,fillcolor=lightblue]";
        }

    private:
        _FrameName name;
        _ID id;
        _Items it;
    };
}}
#endif
