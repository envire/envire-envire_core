/*
 * Enviremanager.cpp
 *
 *  Created on: 18.04.2016
 *      Author: planthaber
 */

#include "EnvireManager.hpp"

#include <envire_core/graph/EnvireGraph.hpp>


namespace envire { 
namespace core {


EnvireManager::GraphMap EnvireManager::graphs;


EnvireManager::EnvireManager() {
    // TODO Auto-generated constructor stub

}

EnvireManager::~EnvireManager() {}

EnvireGraphPtr EnvireManager::getInstance(std::string name, bool force_recreate) {
    EnvireGraphPtr graph = graphs[name];
    if (!graph.get()){
        //create
        graph = EnvireGraphPtr(new ThreadSaveEnvireGraph());
        graphs[name] = graph;
    } else if (force_recreate) {
        graph.reset(new ThreadSaveEnvireGraph());
    }
    return graph;
}


}} // namespaces


