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
#include <boost/uuid/uuid.hpp> /** uuid class */

#include <envire_core/Frame.hpp> /** Frames are for the Vertex **/
#include <envire_core/Transform.hpp> /** Transform are the Edges **/
#include <envire_core/Environment.hpp> /** Environment is the tree property **/

namespace envire { namespace core
{
    /**@brief Frame Property Tag
     * boost access tag for the Frame property
     */
    struct FramePropertyTag
    {
        typedef boost::vertex_property_tag kind;
        static  std::size_t const num;
    };

    //std::size_t const FramePropertyTag::num = (std::size_t) &FramePropertyTag::num;
    typedef boost::property <FramePropertyTag, envire::core::Frame>  FrameProperty;

    /**@brief Transform Property Tag
     * boost access tag for the Transform property
     */
    struct TransformPropertyTag
    {
        typedef boost::edge_property_tag kind;
        static  std::size_t const num;
    };

    //std::size_t const TransformPropertyTag::num = (std::size_t) &TransformPropertyTag::num;
    typedef boost::property <TransformPropertyTag, envire::core::Transform>  TransformProperty;

    /**@brief The Transform Graph type
    *
    * The basic class for the Transform Graph
    *
    */
    typedef boost::directed_graph<
        FrameProperty,
        TransformProperty,
        envire::core::Environment> TransformGraph;

    /**@class Transformation Tree
    *
    * Transformation Tree class
    */
    class TransformTree: public TransformGraph
    {
    public:
        typedef typename boost::property_map <TransformGraph, FramePropertyTag>::type FrameMap;
        typedef typename boost::property_map <TransformGraph, TransformPropertyTag>::type TransformMap;

    /** public class methods **/
    public:

        TransformTree(envire::core::Environment const &environment = Environment()):
                        TransformGraph (environment)
        {
        }


    };
}}
#endif
