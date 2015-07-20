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

/** STD **/
#include <fstream> // std::ofstream
#include <unordered_map>

/** Boost **/
#include <boost/graph/directed_graph.hpp> /** Boost directed graph **/
#include <boost/graph/graphviz.hpp>
#include <boost/uuid/uuid.hpp> /** uuid class */

/** Envire **/
#include <envire_core/Frame.hpp> /** Frames are for the Vertex **/
#include <envire_core/Transform.hpp> /** Transform are the Edges **/

namespace envire { namespace core
{
    /**@brief Environment information
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

    /**@brief Writer for Frame Node
     */
    template <class _FrameName, class _ID, class _Items>
    inline FrameWriter<_FrameName, _ID, _Items>
    make_node_writer(_FrameName name, _ID id, _Items it)
    {
        return FrameWriter<_FrameName, _ID, _Items>(name, id, it);
    }


    /**@brief Writer for Frame Node
     */
    template <class _Time, class _Transform>
    inline TransformWriter<_Time, _Transform>
    make_edge_writer(_Time time, _Transform tf)
    {
        return TransformWriter<_Time, _Transform>(time, tf);
    }

    /** The Envire Graph type **/
    typedef boost::directed_graph<
            envire::core::Frame,
            envire::core::Transform,
            envire::core::Environment> EnvireGraph;

    /**@class Transformation Tree
    *
    * Envire Transformation Tree class
    */
    class TransformTree: public EnvireGraph
    {

    public:
        typedef std::string Label;

    private:

        std::unordered_map<Label, vertex_descriptor> vertex_map;

    /** public class methods **/
    public:

        TransformTree(envire::core::Environment const &environment = Environment()):
                        EnvireGraph(environment)
        {
        }

        /**@brief Add a vertex to the tree
         */
        inline TransformTree::vertex_descriptor addVertex(const envire::core::Frame &node)
        {
            return this->addVertex(node.name, node);
        }

        /**@brief Add a vertex to the tree
         */
        TransformTree::vertex_descriptor addVertex(const Label &label, const envire::core::Frame &node)
        {
            vertex_descriptor v;

            /** Check whether there is a vertex with this label **/
            std::unordered_map<Label, vertex_descriptor>::const_iterator got = this->vertex_map.find(label);
            if (got == vertex_map.end())
            {
                if ((v = this->add_vertex(node)) != TransformTree::null_vertex())
                {
                    this->vertex_map.insert({label, v});
                }
            }
            else
            {
               v = got->second;
            }

            return v;
        }

        /**@brief Get the vertex from a label
         */
        TransformTree::vertex_descriptor vertex(const Label label)
        {
            std::unordered_map<Label, vertex_descriptor>::const_iterator got = this->vertex_map.find(label);
            if (got == vertex_map.end())
            {
                return TransformTree::null_vertex();
            }
            else
            {
                return got->second;
            }
        }


        /**@brief Add an Edge to the Tree
         * overload add_edge method
         */
        //boost::tie(edge_descriptor, bool) add_edge(envire::core::Frame &node_from,
        //                                        envire::core::Frame &node_to,
        //                                        envire::core::Frame &envire::core::Transform)
        //{
        //}

        /**@brief Export to GraphViz
         *
         */
        void writeGraphViz(const std::string& filename = "")
        {
            std::streambuf * buf;
            std::ofstream of;

            if(!filename.empty())
            {
                of.open(filename.c_str());
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
