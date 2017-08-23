//
// Copyright (c) 2015, Deutsches Forschungszentrum für Künstliche Intelligenz GmbH.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#pragma once

#include <fstream> // std::ofstream

#include <envire_core/graph/EnvireGraph.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/algorithm/string.hpp>
#include <envire_core/util/Demangle.hpp>

namespace envire { namespace core
{

    template <class PROP_MAP>
    class EnvireGraphVertexWriter
    {
    public:
        EnvireGraphVertexWriter(PROP_MAP propMap) : propMap(propMap){}
        
        template <class VERTEX>
        void operator()(std::ostream &out, const VERTEX& v) const
        {
            const Frame& frame = propMap[v];
            
            out << "[shape=record, label=\"{{"
                << frame.getId()
                <<   "|" << frame.calculateTotalItemCount() << "}";
                
            for(const auto& itemPair : frame.items)
            {
                std::string typeName = demangleTypeName(itemPair.first);
                typeName = escapeAngleBraces(typeName);
                out << "| {" << typeName  << "|" << itemPair.second.size() << "}";
            }
            out << "}\"" << ",style=filled,fillcolor=lightblue]";           
        }
    private:
        PROP_MAP propMap;
    };
    
    template <class PROP_MAP>
    class GenericVertexWriter
    {
    public:
        GenericVertexWriter(PROP_MAP propMap) : propMap(propMap){}
        
        template <class VERTEX>
        void operator()(std::ostream &out, const VERTEX& v) const
        {
            const auto& vertex = propMap[v];
            
            out << "[shape=record, label=\"{{"
                << vertex.toString() << "}"
                << "}\"" << ",style=filled,fillcolor=lightblue]";           
        }
    private:
        PROP_MAP propMap;
    };
    
    template <class PROP_MAP>
    class EnvireGraphEdgeWriter
    {
    public:
        EnvireGraphEdgeWriter(PROP_MAP propMap) : propMap(propMap){}
        
        template <class EDGE>
        void operator()(std::ostream &out, const EDGE& e) const
        {
            const Transform& tf = propMap[e];
            out << "[label=\"" << tf.time.toString(::base::Time::Seconds) <<
            boost::format("\\nt: (%.2f %.2f %.2f)\\nr: (%.2f %.2f %.2f %.2f)") % tf.transform.translation.x() % tf.transform.translation.y() % tf.transform.translation.z()
            % tf.transform.orientation.w() % tf.transform.orientation.x() % tf.transform.orientation.y() % tf.transform.orientation.z()
            << "\""
            << ",shape=ellipse,color=red,style=filled,fillcolor=lightcoral]";
        }
    private:
        PROP_MAP propMap;
    };
    
    template <class PROP_MAP>
    class GenericEdgeWriter
    {
    public:
        GenericEdgeWriter(PROP_MAP propMap) : propMap(propMap){}
        
        template <class EDGE>
        void operator()(std::ostream &out, const EDGE& e) const
        {
            const auto& edge = propMap[e];
            out << "[label=\"" << edge.toString() << "\"" 
                << ",shape=ellipse,color=red,style=filled,fillcolor=lightcoral]";
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


    /**@class GraphDrawing
     * Creates .dot graphs for all Graphs that follow the concepts specified in 
     * graph/GraphTypes.hpp
     * */
    class GraphDrawing
    {

    protected:
        
        template <class PROP_MAP>
        static EnvireGraphVertexWriter<PROP_MAP> makeEnvireGraphVertexWriter(PROP_MAP propMap)
        {
            return EnvireGraphVertexWriter<PROP_MAP>(propMap);
        }
        
        template <class PROP_MAP>
        static GenericVertexWriter<PROP_MAP> makeGenericVertexWriter(PROP_MAP propMap)
        {
            return GenericVertexWriter<PROP_MAP>(propMap);
        }
        
        template <class PROP_MAP>
        static EnvireGraphEdgeWriter<PROP_MAP> makeEnvireGraphEdgeWriter(PROP_MAP propMap)
        {
            return EnvireGraphEdgeWriter<PROP_MAP>(propMap);
        }
        
        template <class PROP_MAP>
        static GenericEdgeWriter<PROP_MAP> makeGenericEdgeWriter(PROP_MAP propMap)
        {
            return GenericEdgeWriter<PROP_MAP>(propMap);
        }
        
    public:
      
        static void write(const EnvireGraph& graph, std::ostream& out)
        {
            boost::write_graphviz(out, graph,
                    makeEnvireGraphVertexWriter(boost::get(boost::vertex_bundle, graph)),
                    makeEnvireGraphEdgeWriter(boost::get(boost::edge_bundle, graph)),
                    GraphPropWriter());       
        }
        
        template <class FRAME_PROP>
        static void write(const TransformGraph<FRAME_PROP>& graph, std::ostream& out)
        {
            boost::write_graphviz(out, graph,
                    makeGenericVertexWriter(boost::get(boost::vertex_bundle, graph)),
                    makeEnvireGraphEdgeWriter(boost::get(boost::edge_bundle, graph)),
                    GraphPropWriter());       
        }
        
            template <class EDGE_PROP, class FRAME_PROP>
        static void write(const Graph<EDGE_PROP, FRAME_PROP>& graph, std::ostream& out)
        {
            boost::write_graphviz(out, graph,
                    makeGenericVertexWriter(boost::get(boost::vertex_bundle, graph)),
                    makeGenericEdgeWriter(boost::get(boost::edge_bundle, graph)),
                    GraphPropWriter());       
        }
        
        
        template <class T>
        static void write(const T& graph, const std::string& filename = "")
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
            write(graph, out);
        }
    };
}}
