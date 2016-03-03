#pragma once

#include <map>
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <class_loader/class_loader.h>
#include <base/Singleton.hpp>
#include <glog/logging.h>

#include "PluginManager.hpp"
#include <envire_core/items/ItemBase.hpp>
#include <envire_core/util/Exceptions.hpp>

namespace envire { namespace core
{

/**
 * @class ClassLoader
 * @brief A singleton class used to load class loader based plugins
 * This class inherits from the PluginManager
 */
class ClassLoader : public PluginManager, public boost::noncopyable
{
    friend class base::Singleton<ClassLoader>;
    static constexpr char envire_item_base_class[] = "envire::core::ItemBase";
    static constexpr char envire_collision_base_class[] = "envire::collision::ODECollisionBase";
    typedef std::map<std::string, boost::shared_ptr<class_loader::ClassLoader> > LoaderMap;

public:
    /**
     * @brief Returns the singleton instance of this class
     */
    static ClassLoader* getInstance();

    /**
     * @brief Returns true if the class is registerd
     * @param class_name the name of the plugin class
     * @returns True if class is available
     */
    bool hasClass(const std::string& class_name) const;

    /**
     * @brief Returns true if the class is registerd and inherits from the given base class.
     * @param class_name the name of the plugin class
     * @param base_class_name the name of the base class
     * @returns True if class is available
     */
    bool hasClassOfType(const std::string& class_name, const std::string& base_class_name) const;

    /**
     * @brief Returns true if the class is registerd and inherits from envire::core::ItemBase.
     * @param class_name the name of the plugin class
     * @returns True if class is available
     */
    bool hasEnvireItem(const std::string& class_name) const;

    /**
     * @brief Returns true if the class is registerd and inherits from envire::collision::ODECollisionBase.
     * @param class_name the name of the plugin class
     * @returns True if class is available
     */
    bool hasCollisionObject(const std::string& class_name) const;

    /**
     * @brief Creates an instance of the given class
     * @param class_name the name of the plugin class
     * @param instance pointer to the base class of the class
     * @return True if an instance of the class could be created
     */
    template<class BaseClass>
    bool createInstance(const std::string& class_name, boost::shared_ptr<BaseClass>& instance);

    /**
     * @brief Creates an instance of the given class and tries to down cast to the actual implementation.
     * @param class_name the name of the plugin class
     * @param instance pointer to the instance of the class
     * @return True if an instance of the class could be created
     * @throws DownCastException if the cast from BaseClass to InheritedClass isn't possible
     */
    template<class InheritedClass, class BaseClass>
    bool createInstance(const std::string& class_name, boost::shared_ptr<InheritedClass>& instance);

    /**
     * @brief Creates an instance of the given class, which has to be inherited from envire::core::ItemBase
     *        and tries to down cast to the actual implementation.
     * @param class_name the name of the plugin class
     * @param item pointer to the envire item of the class
     * @return True if an instance of the class could be created
     * @throws DownCastException if the cast from envire::core::ItemBase to InheritedClass isn't possible
     */
    template<class InheritedClass>
    bool createEnvireItem(const std::string& item_name, boost::shared_ptr<InheritedClass>& item);

    /**
     * @brief Creates an instance of the given class, which has to be inherited from envire::core::ItemBase
     * @param class_name the name of the plugin class
     * @param base_item pointer to the envire item base class
     * @return True if an instance of the class could be created
     */
    bool createEnvireItem(const std::string& item_name, envire::core::ItemBase::Ptr& base_item);

    /**
     * @brief Creates an envire item for the given embedded type
     * @param embedded_type The class name of the embedded type
     * @param base_item pointer to the envire item base class
     * @return True if an instance of the class could be created
     */
    bool createEnvireItemFor(const std::string& embedded_type, envire::core::ItemBase::Ptr& base_item);

    /**
     * @brief Creates a collision object for the given class.
     *        The collision object must inherit from envire::collision::ODECollisionBase.
     * @param class_name class for which we want a collision object
     * @param collision_object pointer to the collision object base class
     * @return True if an instance of the class could be created
     */
    template<class BaseClass>
    bool createCollisionObjectFor(const std::string& class_name, boost::shared_ptr<BaseClass>& collision_object);

    /**
     * @brief Creates a collision object for the given class.
     *        The collision object must inherit from envire::collision::ODECollisionBase.
     * @param item item for which we want a collision object
     * @param collision_object pointer to the collision object base class
     * @return True if an instance of the class could be created
     */
    template<class BaseClass>
    bool createCollisionObjectFor(const envire::core::ItemBase& item, boost::shared_ptr<BaseClass>& collision_object);

    /**
     * @brief Adds an additional library path to the set of library paths
     * Note: A set of paths is already looked up by using the environment variable LD_LIBRARY_PATH
     *       when this class is created.
     * @param library_path path to a library folder
     */
    void addLibraryPath(const std::string& library_path);

protected:
    /**
     * @brief Constructor for ClassLoader
     * It is protected because this class is a singleton class.
     */
    ClassLoader();

    /**
     * @brief Destructor for ClassLoader
     */
    virtual ~ClassLoader();

    /**
     * @brief Loads all paths set in the environment variable LD_LIBRARY_PATH
     *        to the set of library paths.
     */
    void loadLibraryPaths();

private:

    /**
     * @brief Loads the library of the given plugin class
     * @param class_name name of the plugin class
     * @return True if the library could be loaded
     */
    bool loadLibrary(const std::string& class_name);

private:
    /** Mapping between library name and class loader instances */
    LoaderMap loaders;

    /** Set of the known shared library folders */
    std::set<std::string> library_paths;
};

template<class BaseClass>
bool ClassLoader::createInstance(const std::string& class_name, boost::shared_ptr<BaseClass>& instance)
{
    // get library name of the class
    std::string lib_name;
    if(!getClassLibraryPath(class_name, lib_name))
    {
        LOG(ERROR) << "Could not find plugin library for class " << class_name;
        return false;
    }

    // find loader for the class
    LoaderMap::iterator it = loaders.find(lib_name);
    if(it == loaders.end() && loadLibrary(class_name))
        it = loaders.find(lib_name);

    if(it == loaders.end())
    {
        LOG(ERROR) << "Failed to load plugin library " << lib_name;
        return false;
    }

    // try to create an instance of the class
    if(it->second->isClassAvailable<BaseClass>(class_name))
    {
        instance = it->second->createInstance<BaseClass>(class_name);
        return true;
    }

    if(hasNamespace(class_name))
    {
        // try to create an instance of the class using the class name without namespace
        std::string short_class_name = removeNamespace(class_name);
        if(it->second->isClassAvailable<BaseClass>(short_class_name))
        {
            instance = it->second->createInstance<BaseClass>(short_class_name);
            return true;
        }
    }
    else
    {
        // try to create an instance of the class using the full class name
        std::string full_class_name;
        if(getFullClassName(class_name, full_class_name) && it->second->isClassAvailable<BaseClass>(full_class_name))
        {
            instance = it->second->createInstance<BaseClass>(full_class_name);
            return true;
        }
    }

    LOG(ERROR) << "Failed to create and instance of class " << class_name
                << ", it isn't available in the plugin library " << lib_name;
    return false;
}

template<class InheritedClass, class BaseClass>
bool ClassLoader::createInstance(const std::string& class_name, boost::shared_ptr<InheritedClass>& instance)
{
    boost::shared_ptr<BaseClass> base_instance;
    if(!createInstance<BaseClass>(class_name, base_instance))
        return false;

    instance = boost::dynamic_pointer_cast<InheritedClass>(base_instance);
    if(instance == NULL)
        throw DownCastException<InheritedClass, BaseClass>(class_name);
    return true;
}

template<class InheritedClass>
bool ClassLoader::createEnvireItem(const std::string& item_name, boost::shared_ptr<InheritedClass>& item)
{
    return createInstance<InheritedClass, envire::core::ItemBase>(item_name, item);
}

template<class BaseClass>
bool ClassLoader::createCollisionObjectFor(const std::string& class_name, boost::shared_ptr<BaseClass>& collision_object)
{
    std::string associated_class;
    if(getAssociatedClassOfType(class_name, envire_collision_base_class, associated_class))
    {
        return createInstance<BaseClass>(associated_class, collision_object);
    }
    return false;
}

template<class BaseClass>
bool ClassLoader::createCollisionObjectFor(const envire::core::ItemBase& item, boost::shared_ptr<BaseClass>& collision_object)
{
    return createCollisionObjectFor<BaseClass>(item.getClassName(), collision_object);
}

}}