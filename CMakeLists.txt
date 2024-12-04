# CMakeLists.txt has to be located in the project folder and cmake has to be
# executed from 'project/build' with 'cmake ../'.
cmake_minimum_required(VERSION 3.9)
project(envire_core VERSION 0.1 DESCRIPTION "Envire Graph Core Library")


if(INSTALL_DEPS)
  execute_process(COMMAND bash build.bash ${CMAKE_INSTALL_PREFIX}
                  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/source_dependencies)
  set(ENV{PKG_CONFIG_PATH} ${CMAKE_INSTALL_PREFIX}/lib/pkgconfig/)
  set(ENV{CMAKE_PREFIX_PATH} ${CMAKE_INSTALL_PREFIX}/share/rock/cmake/)
endif()


find_package(Rock)
set(ROCK_TEST_ENABLED ON CACHE BOOL "set to ON to enable the unit tests")
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

option(COVERAGE "Enable code coverage. run 'make test && make coverage' to generate the coverage report. The report will be in ${CMAKE_BINARY_DIR}/cov" OFF)


find_package(PkgConfig REQUIRED)
pkg_check_modules(PLUGIN_LIBS plugin_manager class_loader_melodic)
if(PLUGIN_LIBS_FOUND)
  #this definition is used in the source to include/exclude the plugin headers
  message(STATUS "Plugin system enabled")
  add_definitions(-DCMAKE_ENABLE_PLUGINS)
else()
  message("Plugin system disabled")
endif()

if(COVERAGE)
    if(CMAKE_BUILD_TYPE MATCHES Debug)
        add_definitions(-fprofile-arcs -ftest-coverage)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage -lgcov")
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fprofile-arcs -ftest-coverage -lgcov")
        set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage -lgcov")
        
        add_custom_target(coverage
          COMMAND lcov --directory . --capture --output-file cov.info
          COMMAND lcov --remove cov.info 'tests/*' '/usr/*' '*/install/include/*' --output-file cov.info.cleaned
          COMMAND genhtml -o ./cov cov.info.cleaned
          COMMAND cmake -E remove cov.info cov.info.cleaned
          WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        )
    else()
        message(FATAL_ERROR "Code coverage only works in Debug versions" )
    endif()
endif()

rock_init()
rock_standard_layout()

