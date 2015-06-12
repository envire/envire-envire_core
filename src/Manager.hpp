/**\file Manager.hpp
 *
 * This class provided stores and manages the environment
 *
 * @author Javier Hidalgo Carrio et. al (see THANKS  for the full Author list)
 * See LICENSE for the license information
 *
 */


#ifndef __ENVIRE_MANAGER__
#define __ENVIRE_MANAGER__

#include <unordered_map>

namespace envire { namespace core
{
    /**@class Entity
    *
    * Entity class
    */
    class Manager
    {
        typedef std::tuple<base::TransformWithCovariance, envire::core::Entity> EnvireEntity;

        protected:
            std::unordered_map<boost::uuids::uuid, EnvireEntity> entity_hash;

        public:
            
    }

}}


#endif
