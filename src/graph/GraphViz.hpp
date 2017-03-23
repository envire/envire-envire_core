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

#include <envire_core/graph/Graph.hpp>
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
        static void write(const Graph<FRAME_PROP, EDGE_PROP> &graph, std::ostream& out)
        {
            boost::write_graphviz(out, graph,
                    make_property_writer(boost::get(boost::vertex_bundle, graph)),
                    make_property_writer(boost::get(boost::edge_bundle, graph)),
                    GraphPropWriter());       
        }
        
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
            write(graph, out);
        }
    };
}}
