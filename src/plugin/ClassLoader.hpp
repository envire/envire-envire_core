//
// Copyright (c) 2015, Deutsches Forschungszentrum für Künstliche Intelligenz GmbH.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#pragma once

#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <base-logging/Singleton.hpp>
#include <plugin_manager/PluginLoader.hpp>

#include <envire_core/items/ItemBase.hpp>

namespace envire { namespace core
{

class Serialization;

/**
 * @class ClassLoader
 * @brief A singleton class used to load class loader based plugins
 * This class inherits from the PluginManager
 */
class ClassLoader : public plugin_manager::PluginLoader
{
    friend class base::Singleton<ClassLoader>;
    friend class envire::core::Serialization;
    static constexpr char envire_item_base_class[] = "envire::core::ItemBase";
    static constexpr char envire_item_class[] = "envire::core::Item";
    static constexpr char envire_collision_base_class[] = "envire::collision::ODECollisionBase";

public:
    /**
     * @brief Returns the singleton instance of this class
     */
    static ClassLoader* getInstance();

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

protected:
    /**
     * @brief This method allows to load the shared library of the given plugin class name.
     *
     * @param item_name the name of the plugin class
     * @returns true if successfully loaded or when already loaded
     */
    bool loadEnvireItemLibrary(const std::string& item_name);

    /**
     * @brief Loads all available shared libraries providing plugins
     * of the base class envire::core::ItemBase.
     *
     * @returns true if all libraries have been loaded successfully
     */
    bool loadAllEnvireItemLibraries();

private:
    /**
     * @brief Constructor for ClassLoader
     * It is protected because this class is a singleton class.
     */
    ClassLoader();

    /**
     * @brief Destructor for ClassLoader
     */
    virtual ~ClassLoader();

};

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
    std::string class_name;
    if (item.getClassName(class_name))
        return createCollisionObjectFor<BaseClass>(class_name, collision_object);
    return false;
}

}}