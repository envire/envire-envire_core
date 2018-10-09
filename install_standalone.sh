#!/bin/bash
set -e #abort with error if any command returns with something other than zero

############
# Install script for envire
# Download this to a folder and run "$> bash install_standalone.sh" to install to the current folder
# Do not clone the envire repository beforehand
############

function build {
  git clone --branch $2 $1 $3
  mkdir $3/build
  cd $3/build
  cmake .. -DCMAKE_INSTALL_PREFIX=$PWD/../../install $4
  make -j install
  cd ../../
}

build https://github.com/rock-core/base-cmake.git master base-cmake
cat <<'EOF' > env.sh
export CMAKE_PREFIX_PATH=$PWD/install
export PKG_CONFIG_PATH=$PWD/install/lib/pkgconfig:$PWD/install/share/pkgconfig:$PWD/install/lib64/pkgconfig:$PKG_CONFIG_PATH
export LD_LIBRARY_PATH=$PWD/install/lib:$PWD/install/lib64:$LD_LIBRARY_PATH
export PATH=$PWD/install/bin:$PATH
EOF
source env.sh

#SISL should be optional for base-types but it isn't
build https://github.com/SINTEF-Geometry/SISL.git master sisl "-DBUILD_SHARED_LIBS=ON"

build https://github.com/rock-core/base-logging.git master base-logging
build https://github.com/rock-core/base-types.git master base-types "-DBINDINGS_RUBY=OFF -DROCK_VIZ_ENABLED=FALSE"

build https://github.com/envire/base-numeric.git master base-numeric
build https://github.com/envire/base-boost_serialization.git master base-boost_serialization

#plugins
build https://github.com/rock-core/base-console_bridge.git master base-console_bridge
build https://github.com/ros/class_loader.git indigo-devel class_loader "-DBUILD_SHARED_LIBS=ON -DCATKIN_DISABLED=ON -DCMAKE_BUILD_TYPE=Release"
build https://github.com/envire/tools-plugin_manager.git master tools-plugin_manager


#envire
#build https://github.com/envire/envire-envire_core.git envire-envire_core "-DENABLE_PLUGINS=OFF -DROCK_TEST_ENABLED=OFF"
build https://github.com/envire/envire-envire_core.git master envire-envire_core "-DROCK_TEST_ENABLED=ON"






