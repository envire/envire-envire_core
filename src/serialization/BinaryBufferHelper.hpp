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

#include <vector>
#include <streambuf>
#include <cstdint>

namespace envire { namespace core
{

/**
 * @brief Helper class which defines a std::streambuf which reads
 * direcly out of a std::vector<uint8_t>.
 */
class BinaryInputBuffer : public std::streambuf
{
public:
    BinaryInputBuffer(const std::vector<uint8_t>& buffer) : BinaryInputBuffer(buffer.data(), buffer.size()) {}
    BinaryInputBuffer(const uint8_t* data, size_t size) : BinaryInputBuffer(data, data + size) {}
    BinaryInputBuffer(const uint8_t* begin, const uint8_t* end)
    {
        this->setg((char*)begin, (char*)begin, (char*)end);
    }
};

/**
 * @brief Helper class which defines a std::streambuf which writes
 * direcly in a std::vector<uint8_t>.
 */
class BinaryOutputBuffer : public std::streambuf
{
public:
    BinaryOutputBuffer(std::vector<uint8_t> *buffer) : buffer(buffer)
    {
        this->setp((char*)buffer->data(), (char*)(buffer->data() + buffer->size()));
    }

    /**
     * This method is called by sputc if the current put pointer is equal to the end pointer.
     */
    int overflow(int c)
    {
        // increase buffer size by one
        buffer->resize(buffer->size()+1);
        // reset offsets
        this->setp((char*)(buffer->data() + buffer->size()-1), (char*)(buffer->data() + buffer->size()));
        // recall sputc
        return this->sputc(c);
    }

private:
    std::vector<uint8_t> *buffer;
};

}}