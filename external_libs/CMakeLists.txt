

cmake_minimum_required(VERSION 3.10)

include(ExternalProject)

#https://stackoverflow.com/questions/15175318/cmake-how-to-build-external-projects-and-include-their-targets
#https://crascit.com/2015/07/25/cmake-gtest/

ExternalProject_Add(base-cmake 
    GIT_REPOSITORY    https://github.com/rock-core/base-cmake.git
    GIT_TAG           master
    INSTALL_DIR       ${PROJECT_SOURCE_DIR}/install
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
)

ExternalProject_Add(base-logging
    GIT_REPOSITORY    https://github.com/rock-core/base-logging.git
    GIT_TAG           master
    INSTALL_DIR       ${PROJECT_SOURCE_DIR}/install
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
)
add_dependencies(base-logging base-cmake)


ExternalProject_Add(base-types
    GIT_REPOSITORY    https://github.com/rock-core/base-types.git
    GIT_TAG           master
    INSTALL_DIR       ${PROJECT_SOURCE_DIR}/install
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR> -DBINDINGS_RUBY=OFF -DUSE_SISL=OFF -DROCK_VIZ_ENABLED=FALSE
)
add_dependencies(base-types base-logging)

ExternalProject_Add(base-numeric
    GIT_REPOSITORY    https://github.com/envire/base-numeric.git
    GIT_TAG           master
    INSTALL_DIR       ${PROJECT_SOURCE_DIR}/install
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR> -DBINDINGS_RUBY=OFF -DUSE_SISL=OFF -DROCK_VIZ_ENABLED=FALSE
)
add_dependencies(base-numeric base-types)

ExternalProject_Add(base-boost_serialization
    GIT_REPOSITORY    https://github.com/envire/base-boost_serialization.git
    GIT_TAG           master
    INSTALL_DIR       ${PROJECT_SOURCE_DIR}/install
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR> -DBINDINGS_RUBY=OFF -DUSE_SISL=OFF -DROCK_VIZ_ENABLED=FALSE
)
add_dependencies(base-boost_serialization base-numeric)

ExternalProject_Add(tools-plugin_manager
    GIT_REPOSITORY    https://github.com/envire/tools-plugin_manager.git
    GIT_TAG           master
    INSTALL_DIR       ${PROJECT_SOURCE_DIR}/install
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR> -DBINDINGS_RUBY=OFF -DUSE_SISL=OFF -DROCK_VIZ_ENABLED=FALSE
)
add_dependencies(tools-plugin_manager base-boost_serialization)

configure_file(env.sh.in ${PROJECT_SOURCE_DIR}/env.sh)
