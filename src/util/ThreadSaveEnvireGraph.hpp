#pragma once

#include <mutex>
#include <memory>
#include <vector>
#include "../graph/EnvireGraph.hpp"

#include <execinfo.h>

namespace envire { 
namespace core {

/**
 * @brief Template class to make variables thread save
 * 
 * @tparam C The class type to have thread save
 */

class ThreadSaveEnvireGraph {
 public:
    struct GraphAccess {
        GraphAccess(ThreadSaveEnvireGraph* graph,  std::recursive_mutex &mutex):graph(graph) {
            // printf("%s\n",__PRETTY_FUNCTION__);

            // void *array[10];
            // size_t size;

            // // get void*'s for all entries on the stack
            // size = backtrace(array, 10);
            // backtrace_symbols_fd(array, size, STDERR_FILENO);

            guard = std::shared_ptr< std::lock_guard<std::recursive_mutex> > (new std::lock_guard<std::recursive_mutex>(mutex));
        }

        ~GraphAccess(){
            // printf("%s\n",__PRETTY_FUNCTION__);
        }

        envire::core::EnvireGraph* operator->(){
            return graph->graph.get();
        }

        /**
         * @brief Get the Graph Ptr object
         * 
         * @warning, do not use the pointer after access object is deleted
         * @return std::shared_ptr<envire::core::EnvireGraph> 
         */
        std::shared_ptr<envire::core::EnvireGraph> getGraphPtr(){
            return graph->graph;
        }

        private:
        ThreadSaveEnvireGraph* graph;
        std::shared_ptr< std::lock_guard<std::recursive_mutex> > guard;
    };

    ThreadSaveEnvireGraph():graph(std::shared_ptr<envire::core::EnvireGraph>(new envire::core::EnvireGraph)){};
    virtual ~ThreadSaveEnvireGraph(){};


    GraphAccess getThreadSaveAccess(){
        return GraphAccess(this,mutex);
    }

    /**
     * @brief Get the Unsafe Graph object
     * 
     * You may use this savely for e.g. the EnvireVisualizer, as it it only updated by callbacks
     * @return std::shared_ptr<envire::core::EnvireGraph> 
     */

    std::shared_ptr<envire::core::EnvireGraph> getUnsafeGraph() {
        return graph;
    }


    /**
     * setters
     */

    void addFrame(const envire::core::FrameId& name) {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        graph->addFrame(name);
    }

    void removeFrame(const envire::core::FrameId& name) {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        graph->removeFrame(name);
    }

    bool containsFrame(const envire::core::FrameId& name) {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        return graph->containsFrame(name);
    }

    void addTransform(const envire::core::FrameId& origin, const envire::core::FrameId& target, const envire::core::Transform& tf) {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        graph->addTransform(origin, target, tf);
    }

    void updateTransform(const envire::core::FrameId& origin, const envire::core::FrameId& target, const envire::core::Transform &tf) {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        graph->updateTransform(origin, target, tf);
    }

    void removeTransform(const envire::core::FrameId& origin, const envire::core::FrameId& target) {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        graph->removeTransform(origin, target);
    }


    /**********
    * getters
    **********/

    envire::core::Transform getTransform(const envire::core::FrameId& origin, const envire::core::FrameId& target) {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        return graph->getTransform(origin, target);
    }

    base::Position getPosition(const envire::core::FrameId& origin, const envire::core::FrameId& target) {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        return graph->getTransform(origin, target).transform.translation;
    }

    base::Orientation getOrientation(const envire::core::FrameId& origin, const envire::core::FrameId& target) {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        return graph->getTransform(origin, target).transform.orientation;
    }

    std::vector<envire::core::FrameId> getFrames(const envire::core::FrameId &origin, const envire::core::FrameId &target) {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        return graph->getFrames(origin, target);
    }

    /**********
    *   Items
    ***********/

    void addItem(envire::core::ItemBase::Ptr item) {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        graph->addItem(item);
    }

    void addItemToFrame(const envire::core::FrameId& frameId, envire::core::ItemBase::Ptr item) {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        graph->addItemToFrame(frameId, item);
    }

    void removeItemFromFrame(envire::core::ItemBase::Ptr item) {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        graph->removeItemFromFrame(item);
    }

    template< class ITEMTYPE > envire::core::EnvireGraph::ItemIterator<ITEMTYPE> getItem(const envire::core::FrameId& target, const int i = 0) {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        return graph->getItem<ITEMTYPE>(target,i);
    }

    const envire::core::Frame::ItemList & getItems(const envire::core::FrameId & frame, const std::type_index & type) {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        return graph->getItems(frame, type);
    }

    /**
     * @brief lock the protected variable manually to allow a reference access
     */
    void lock() {
        mutex.lock();
    }

    /**
     * @brief unlock the protected variable after it was manually locked
     */
    void unlock() {
        mutex.unlock();
    }


 private:
    std::shared_ptr<envire::core::EnvireGraph> graph;

    // using a recursive mutex to allow graph modifications my reactros
    std::recursive_mutex mutex;

};


}}//namespaces
