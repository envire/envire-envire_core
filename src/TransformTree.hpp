/**\file TransformTree.hpp
 *
 * This class provided stores and manages the transformation tree
 *
 * @author Javier Hidalgo Carrio et. al (see THANKS  for the full Author list)
 * See LICENSE for the license information
 *
 */


#ifndef __ENVIRE_CORE_TRANSFORM_TREE__
#define __ENVIRE_CORE_TRANSFORM_TREE__


#include <boost/graph/directed_graph.hpp> /** Boost directed graph **/
#include <boost/graph/graphviz.hpp>
#include <boost/uuid/uuid.hpp> /** uuid class */

#include <envire_core/EnvireGraph.hpp> /** Envire core graph **/
#include <envire_core/Frame.hpp> /** Frames are for the Vertex **/
#include <envire_core/Transform.hpp> /** Transform are the Edges **/

namespace envire { namespace core
{
    /** Environment information
     */
    struct Environment
    {
        boost::uuids::uuid uuid; /** Environment Unique Identifier */
        std::string name; /** Envire Name **/
        /** Perhaps here some environment information
         * like date of creation or UTM grid coordinates **/

        Environment()
        {
            name = "envire::core::noname";
        }
    };

    template <class _FrameName, class _ID, class _Items>
    inline FrameWriter<_FrameName, _ID, _Items>
    make_node_writer(_FrameName name, _ID id, _Items it)
    {
        return FrameWriter<_FrameName, _ID, _Items>(name, id, it);
    }


    template <class _Time, class _Transform>
    inline TransformWriter<_Time, _Transform>
    make_edge_writer(_Time time, _Transform tf)
    {
        return TransformWriter<_Time, _Transform>(time, tf);
    }

    /**@class Transformation Tree
    *
    * Envire Transformation Tree class
    */
    class TransformTree: public boost::EnvireGraph<envire::core::Frame,
                                    envire::core::Transform,
                                    envire::core::Environment>
    {

    /** public class methods **/
    public:

        TransformTree(envire::core::Environment const &environment = Environment()):
             boost::EnvireGraph<envire::core::Frame,
                                envire::core::Transform,
                                envire::core::Environment>(environment)
        {
        }

        void writeGraphViz(const std::string& filename = "")
        {
            std::streambuf * buf;
            std::ofstream of;

            if(!filename.empty())
            {
                of.open(filename);
                buf = of.rdbuf();
            }
            else
            {
                buf = std::cout.rdbuf();
            }

            std::ostream out(buf);
            boost::write_graphviz (out, (*this),
                    make_node_writer(boost::get(&envire::core::Frame::name, (*this)), boost::get(&envire::core::Frame::uuid, (*this)), boost::get(&envire::core::Frame::items, (*this))),
                    make_edge_writer(boost::get(&envire::core::Transform::time, (*this)), boost::get(&envire::core::Transform::transform, (*this))));

               //boost::dynamic_properties dp;
                //dp.property("color", get(&vertex_info::color, g));
        }


    };

}}


#endif
