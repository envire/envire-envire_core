#ifndef __ENVIRE_CORE_GRAPHVIZ__
#define __ENVIRE_CORE_GRAPHVIZ__

#include <fstream> // std::ofstream

#include <envire_core/graph/TransformGraph.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/algorithm/string.hpp>
#include <cxxabi.h> // for name demangling


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
            const Frame& frame = f[n];
            
            //example output line:
            //label="{{frame_a | 3} | {boost::shared_ptr\<envire::core::Item\<float\> \>|1}| {boost::shared_ptr\<envire::core::Item\<int\> \>|1}| {boost::shared_ptr\<envire::core::Item\<std::string\> \>|1}}", 
            
            out << "[shape=record, label=\"{{" << frame.name <<
                   "|" << frame.calculateTotalItemCount() << "}";
                
            for(const auto& itemPair : frame.items)
            {
                std::string typeName = demangledTypeName(itemPair.first);
                //all types in the graph are stored as shared pointers, but
                //the user does not care about it and it clutters the output
                typeName = stripSharedPtr(typeName);
                //typeName = escapeLabel(typeName);
                out << "| {" << typeName  << "|" << itemPair.second.size() << "}";
            }
            out << "}\"" << ",style=filled,fillcolor=lightblue]";
        }

    private:
      
      std::string demangledTypeName(const std::type_index& type) const
      {
        char* p_nice_name = abi::__cxa_demangle(type.name(),NULL,NULL,NULL);
        std::string result(p_nice_name);
        free(p_nice_name);
        return result;  
      }
      
      /**escapes angle braces because they are not valid dot labels */
      std::string escapeLabel(const std::string& label) const
      {
        const std::string one = boost::replace_all_copy(label, "<", "\\<");
        const std::string two = boost::replace_all_copy(one, ">", "\\>");
        return two;
      }
      
      /**
       * Expects input of the format
       * "boost::shared_ptr<STUFF>"
       * and returns "STUFF"
       */
      std::string stripSharedPtr(const std::string& input) const 
      {
        if(boost::starts_with(input, "boost::shared_ptr<"))
        {
          std::string output = boost::erase_first_copy(input, "boost::shared_ptr<");
          output = boost::erase_last_copy(output, ">");
          return output;
        }
        return input;
      }
      
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

