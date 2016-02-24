#pragma once

#include <string>
#include <cxxabi.h>
#include <boost/algorithm/string/replace.hpp>
#include <typeindex>

namespace envire { namespace core { 
  
std::string demangleTypeName(const std::type_index& type);

/**escapes angle braces in @p input with "\" */
std::string escapeAngleBraces(const std::string& input);

}}