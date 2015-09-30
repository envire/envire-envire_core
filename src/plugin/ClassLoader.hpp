#ifndef __ENVIRE_CORE_CLASS_LOADER__
#define __ENVIRE_CORE_CLASS_LOADER__

#include <map>
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <class_loader/class_loader.h>
#include <base/Singleton.hpp>

#include <envire_core/items/ItemBase.hpp>

namespace envire
{

    class ClassLoader
    {
        friend class base::Singleton<ClassLoader>;
        static const std::vector<std::string> plugin_paths;//contains all search paths for plugins
        static constexpr char envire_item_suffix[] = "_envire_plugin";
        static constexpr char envire_collision_suffix[] = "_collision_plugin";
        typedef std::map<std::string, boost::shared_ptr<class_loader::ClassLoader> > LoaderMap;

    public:
        typedef envire::core::ItemBase ItemBaseClass;
        typedef ItemBaseClass::Ptr ItemBaseClassPtr;

        static ClassLoader* getInstance();

        bool hasValidPluginPath();

        bool hasItem(const std::string& class_name);

        ItemBaseClassPtr createItem(const std::string& class_name);

        template<class T>
        typename T::Ptr createItem(const std::string& class_name)
        {
            ItemBaseClassPtr item = createItemIntern(class_name);
            typename T::Ptr inherited_item = boost::dynamic_pointer_cast<T>(item);
            if(inherited_item == NULL)
            {
                std::string error_msg = "Failed to cast item of type ";
                error_msg += class_name;
                throw std::runtime_error(error_msg);
            }
            return inherited_item;
        }
        
        /**Loads LD_LIBRARY_PATH and parses it into a list of paths */
        static std::vector<std::string> loadLibraryPath();
/*
        template<class T>
        static boost::intrusive_ptr<T> createCollisionObject(const std::string& class_name);

        template<class T>
        static boost::intrusive_ptr<T> createCollisionObject(const ItemBaseClass& item);
*/

    protected:
        ClassLoader();
        ~ClassLoader();

    private:

        ItemBaseClassPtr createItemIntern(const std::string& class_name);

        void loadLibrary(const std::string& class_name);

    private:
        LoaderMap loaders;
    };

}

#endif
