/**\file Environment.hpp
 *
 * This class provided stores and manages the environment tree
 *
 * @author Javier Hidalgo Carrio et. al (see THANKS  for the full Author list)
 * See LICENSE for the license information
 *
 */


#ifndef __ENVIRE_CORE_ENVIRE_TREE__
#define __ENVIRE_CORE_ENVIRE_TREE__

#include <envire_core/Node.hpp>
#include <envire_core/Edge.hpp>

#include <boost/uuid/uuid.hpp> /** uuid class */
#include <boost/graph/adjacency_list.hpp> /** Boost Adjacency list **/


namespace envire { namespace core
{
    /** Environment information
     */
    struct Environment
    {
        boost::uuids::uuid uuid; /** Envire Unique Identifier */
        std::string name; /** Envire Name **/
        /** Perhaps here some environment information
         * like date of creation or UTM coordinates **/
    };


    /**@class Tree
    *
    * Envire Tree class
    */
    class Tree
    {
        /** private type definitions **/
        public:
            /** std::list for edges, std::vector for vertices, bidirectional graph?, no property vertices, property edges **/
            typedef boost::adjacency_list < boost::listS, boost::vecS,
                    boost::bidirectionalS,
                    envire::core::Node, envire::core::Edge,
                    envire::core::Environment> Graph;

        public:
        Graph tree; /** The envire tree */

        /** public class methods **/
        public:

            Tree();

            Tree(const std::string &environment_name);

            ~Tree();

    };

}}


#endif
