#include <envire_core/util/Demangle.hpp>

namespace envire { namespace core
{
  
std::string demangleTypeName(const std::type_index& type)
{
    int status = 0;
    std::string result;
    char* p_nice_name = abi::__cxa_demangle(type.name(), NULL, NULL, &status);
    if(0 != status) 
    {
        //if demangling failed return the mangled name
        result = type.name();
    }
    else
    {
        if(nullptr != p_nice_name)
        {
            result = p_nice_name;
            free(p_nice_name);
        }

    }
    return result;  
}

/**escapes angle braces in @p input with "\" */
std::string escapeAngleBraces(const std::string& input) 
{
    const std::string a = boost::replace_all_copy(input, "<", "\\<");
    const std::string b = boost::replace_all_copy(a, ">", "\\>");
    return b;
}

}}