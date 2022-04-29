#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo
    echo "Please specify a prefix. If you want system wide installation use '/usr/local' as prefix. for a local install use e.g. './install'"
    echo "In case you use a local prefix, please source the generated env.sh script in this folder to set up the paths before building the main library"
    echo
    exit -1  
fi

# get global path
ABS_PREFIX=`readlink -f $1`
mkdir -p $ABS_PREFIX

# Setup environemtn variables
export CMAKE_PREFIX_PATH=$ABS_PREFIX/share/rock/cmake/
export PKG_CONFIG_PATH=$ABS_PREFIX/lib/pkgconfig/

#download and build (in subshell to return to original folder)
  
mkdir -p build
cd build
cmake -DCMAKE_INSTALL_PREFIX=$ABS_PREFIX ..
make -j
