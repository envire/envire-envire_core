#pragma once
#include <boost/filesystem.hpp>

struct PathSingleton
{
    //absolute path to the folder the current binary is in
    static boost::filesystem::path binaryFolderPath;
};