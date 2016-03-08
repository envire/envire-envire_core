#pragma once

#include <stdexcept>
#include <string>

namespace envire { namespace core
{

class CloneMethodNotImplementedException : public std::exception
{
public:
    explicit CloneMethodNotImplementedException() :
        msg("Clone method is not implemented for this event!") {}
    virtual char const * what() const throw() { return msg.c_str(); }
    const std::string msg;
};

}}