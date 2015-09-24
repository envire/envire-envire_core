#ifndef __ENVIRE_CORE_GRAPHVIZ__
#define __ENVIRE_CORE_GRAPHVIZ__

#include <fstream> // std::ofstream

#include <envire_core/TransformGraph.hpp>
#include <boost/graph/graphviz.hpp>


namespace envire { namespace core
{

    /**@class Transform Writer
    * Frame Graph Viz property writer for boost graphs
    * */
    template <class _Frame>
    class FrameWriter
    {
    public:
        FrameWriter(_Frame _f):f(_f){}
        template <class _Vertex>
        void operator()(std::ostream &out, const _Vertex& n) const
        {
            if(f[n].name.find("camera") != std::string::npos)
            {
                out << "[shape=record, label=\"<f0> " << f[n].name <<
                    "|<f1>" << f[n].items.size()<<"\""
                    <<",style=filled,fillcolor=orange]";

            }
            else
            {
                out << "[shape=record, label=\"<f0> " << f[n].name <<
                    "|<f1>" << f[n].items.size()<<"\""
                    <<",style=filled,fillcolor=lightblue]";
            }
        }

    private:
        _Frame f;

    };

    /**@class Transform Writer
     * Transform Graph Viz Property writer for boost graphs
     * */
    template <class _Transform>
    class TransformWriter
    {
    public:
        TransformWriter(_Transform _tf):tf(_tf){}
        template <class _Edge>
        void operator()(std::ostream &out, const _Edge& e) const
        {
            out << "[label=\"" << tf[e].time.toString(::base::Time::Seconds) <<
                boost::format("\\nt: (%.1f %.1f %.1f)\\nr: (%.1f %.1f %.1f %.1f)") % tf[e].transform.translation.x() % tf[e].transform.translation.y() % tf[e].transform.translation.z()
                % tf[e].transform.orientation.w() % tf[e].transform.orientation.x() % tf[e].transform.orientation.y() % tf[e].transform.orientation.z()
                << "\""
                << ",shape=ellipse,color=red,style=filled,fillcolor=lightcoral]";
        }
    private:
        _Transform tf;
    };

    /**@class Environment Writer
     * Transform Graph Viz Property writer for boost graphs
     * */
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
     * Class to print TransformGraphs in Graph Viz
     * */
    class GraphViz
    {

    protected:

        inline GraphPropWriter
        make_graph_writer()
        {
            return GraphPropWriter();
        }


        /**@brief Writer for Frame Node
         */
        template <class _Frame>
        inline FrameWriter<_Frame>
        make_node_writer(_Frame frame)
        {
            return FrameWriter<_Frame>(frame);
        }


        /**@brief Writer for Frame Node
         */
        template <class _Transform>
        inline TransformWriter<_Transform>
        make_edge_writer(_Transform tf)
        {
            return TransformWriter<_Transform>(tf);
        }

    public:

        /**@brief Export to GraphViz
         *
         */
        void write(const TransformGraphBase &graph, const std::string& filename = "")
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
                    make_node_writer(boost::get(&FrameProperty::frame, graph)),
                    make_edge_writer(boost::get(&TransformProperty::transform, graph)),
                    make_graph_writer());
        }

        void write(const TransformGraph &tree, const std::string& filename = "")
        {
            write(dynamic_cast<const TransformGraphBase&>(tree.graph()), filename);
        }
    };
}}
#endif

