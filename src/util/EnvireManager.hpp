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

namespace envire {
namespace core {

class EnvireGraph;


typedef std::shared_ptr<ThreadSaveEnvireGraph> EnvireGraphPtr;

class EnvireManager {
public:

    

    typedef std::map<std::string, EnvireGraphPtr> GraphMap;

    static EnvireGraphPtr getInstance(std::string name, bool force_recreate = false);


    EnvireManager();
    virtual ~EnvireManager();

private:
    static GraphMap graphs;

};

}} // namespaces

