/*
 * Enviremanager.h
 *
 *  Created on: 18.04.2016
 *      Author: planthaber
 */

#pragma once

#include <string>
#include <map>
#include "ThreadSaveEnvireGraph.hpp"

#define WORLD_FRAME "world"

namespace envire {
namespace core {

class EnvireGraph;


typedef std::shared_ptr<ThreadSaveEnvireGraph> EnvireGraphPtr;

class EnvireManager {
public:

    

    typedef std::map<std::string, EnvireGraphPtr> GraphMap;

    static EnvireGraphPtr getInstance(std::string name);


    EnvireManager();
    virtual ~EnvireManager();

private:
    static GraphMap graphs;

};

}} // namespaces

