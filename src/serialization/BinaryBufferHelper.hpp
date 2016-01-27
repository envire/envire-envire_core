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