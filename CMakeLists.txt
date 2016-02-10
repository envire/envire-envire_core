# CMakeLists.txt has to be located in the project folder and cmake has to be
# executed from 'project/build' with 'cmake ../'.
cmake_minimum_required(VERSION 2.6)
set(ROCK_USE_CXX11 TRUE)
find_package(Rock)
set(ROCK_TEST_ENABLED ON)
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

option(COVERAGE "Enable code coverage. run 'make test && make coverage' to generate the coverage report. The report will be in ${CMAKE_BINARY_DIR}/cov" OFF)
option(ENABLE_PLUGINS "Enable the plugin system. Disable this to get rid of the dependency to the class_loader" ON)

if(ENABLE_PLUGINS)
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

rock_init(envire_core 0.1)
rock_standard_layout()


