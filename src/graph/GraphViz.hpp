#ifndef __ENVIRE_CORE_GRAPHVIZ__
#define __ENVIRE_CORE_GRAPHVIZ__

#include <fstream> // std::ofstream

#include <envire_core/graph/EnvireGraph.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/algorithm/string.hpp>
#include <envire_core/util/Demangle.hpp>

namespace envire { namespace core
{

    /**@class GraphVizConceptWriter
     * Writes edge properties or vertex properties that follow the 
     * concepts specified in graph/GraphTypes.hpp to graphviz output.
     * */
    template <class PROP_MAP>
    class GraphVizConceptWriter
    {
    public:
        GraphVizConceptWriter(PROP_MAP propMap) : propMap(propMap){}
        
        template <class EDGE_OR_VERTEX>
        void operator()(std::ostream &out, const EDGE_OR_VERTEX& eov) const
        {
            out << propMap[eov].toGraphviz();
        }
    private:
        PROP_MAP propMap;
    };


    class GraphPropWriter
    {
    public:
        GraphPropWriter(){}
        void operator()(std::ostream &out) const
        {
            //out<< "graph[rankdir=LR,splines=ortho];\n";
            out<< "graph[size=\"88,136\", ranksep=3.0, nodesep=2.0, fontname=\"Helvetica\", fontsize=8];\n";
        }
    };


    /**@class GraphViz
     * Creates .dot graphs for all Graphs that follow the concepts specified in 
     * graph/GraphTypes.hpp
     * */
    class GraphViz
    {

    protected:
        
        template <class PROP_MAP>
        static GraphVizConceptWriter<PROP_MAP> make_property_writer(PROP_MAP propMap)
        {
            return GraphVizConceptWriter<PROP_MAP>(propMap);
        }
        
    public:
        
        template <class FRAME_PROP, class EDGE_PROP>
        static void write(const Graph<FRAME_PROP, EDGE_PROP> &graph, const std::string& filename = "")
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

            /** Print graph **/
            std::ostream out(buf);
            boost::write_graphviz (out, graph,
                    make_property_writer(boost::get(boost::vertex_bundle, graph)),
                    make_property_writer(boost::get(boost::edge_bundle, graph)),
                    GraphPropWriter());       
        }
    };
}}
#endif

