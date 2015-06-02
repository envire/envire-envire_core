/**\file Entity.hpp
 *
 * This class provided an interface for the Envire Entity
 *
 * @author Javier Hidalgo Carrio et. al (see THANKS  for the full Author list)
 * See LICENSE for the license information
 *
 */


#ifndef __ENVIRE_ENTITY__
#define __ENVIRE_ENTITY__

#include <boost/uuid/uuid.hpp> /** uuid class */

namespace envire { namespace core
{
    /**@class Entity
    *
    * Entity class
    */
    class Entity
    {
        protected:

            boost::uuids::uuid uuid; /** Unique Identifier */

        public:

            /**@brief Example of a Method
            *
            *
            * @param[in]
            *
            * @param[out]
            */

    };
}}
#endif

