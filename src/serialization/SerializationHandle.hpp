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

namespace boost { namespace archive
{
    class binary_oarchive;
    class binary_iarchive;
    class text_oarchive;
    class text_iarchive;
}}

namespace envire { namespace core
{

/**
 * @brief Interface class of the item serialization handle.
 * By using the ENVIRE_REGISTER_SERIALIZATION macro a handle class for an
 * item will be generated.
 */
class SerializationHandle
{
public:
    virtual ~SerializationHandle() {}

    virtual bool save(boost::archive::binary_oarchive& ar, const ItemBase::Ptr& item) = 0;
    virtual bool load(boost::archive::binary_iarchive& ar, ItemBase::Ptr& item) = 0;

    virtual bool save(boost::archive::text_oarchive& ar, const ItemBase::Ptr& item) = 0;
    virtual bool load(boost::archive::text_iarchive& ar, ItemBase::Ptr& item) = 0;
};

}}