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
#include <envire_core/items/ItemBase.hpp>
#include <envire_core/events/GraphEvent.hpp>

namespace envire { namespace core
{
    class ItemRemovedEvent : public GraphEvent
    {
    public:
      ItemRemovedEvent(const FrameId& frame, const ItemBase::Ptr item) :
          GraphEvent(GraphEvent::ITEM_REMOVED_FROM_FRAME), frame(frame), item(item){}

        GraphEvent* clone() const
        {
            return new ItemRemovedEvent(frame, item);
        }

      FrameId frame;/**<frame that the no longer contains the item.*/
      /**The item that has been removed.
       * @note Since the item has already been removed, item->getFrame() will
       *       not return a valid value.*/
      ItemBase::Ptr item; 
    };
    
    //a type safe version of the above event
    template <class T>
    struct TypedItemRemovedEvent 
    {
      TypedItemRemovedEvent(const FrameId& frame, const ItemBase::PtrType<T> item) : frame(frame), item(item) {}

        GraphEvent* clone() const
        {
            return new TypedItemRemovedEvent(frame, item);
        }

      FrameId frame;
      ItemBase::PtrType<T> item;
    };
}}
