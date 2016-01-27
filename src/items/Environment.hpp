/**\file Environment.hpp
 *
 * This class provided common information useful for a
 * particular environment
 *
 * @author Javier Hidalgo Carrio et. al (see THANKS  for the full Author list)
 * See LICENSE for the license information
 *
 */


#ifndef __ENVIRE_CORE_ENVIRONMENT__
#define __ENVIRE_CORE_ENVIRONMENT__

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost_serialization/BoostTypes.hpp>

namespace envire { namespace core
{
    /**@brief Environment information
     */
    class Environment
    {
    public:
        boost::uuids::uuid uuid; /** Environment Unique Identifier */
        std::string name; /** Environment Name **/

        /** Perhaps here some environment information
         * like date of creation or UTM grid coordinates **/

    public:
        Environment()
        {
            name = "envire::core::noname";
        }

    private:
        /**Grants access to boost serialization */
        friend class boost::serialization::access;

        /**Serializes the members of this class*/
        template <typename Archive>
        void serialize(Archive &ar, const unsigned int version)
        {
            ar & BOOST_SERIALIZATION_NVP(uuid);
            ar & BOOST_SERIALIZATION_NVP(name);
        }
    };
}}
#endif
