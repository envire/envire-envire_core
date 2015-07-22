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
    };
}}
#endif
