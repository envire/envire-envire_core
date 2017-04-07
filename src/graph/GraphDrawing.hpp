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
#include <ogdf/basic/Graph.h>
#include <ogdf/basic/GraphAttributes.h>
#include <ogdf/layered/SugiyamaLayout.h>
#include <ogdf/layered/OptimalRanking.h>
#include <ogdf/layered/MedianHeuristic.h>
#include <ogdf/layered/OptimalHierarchyLayout.h>
#include <ogdf/fileformats/GraphIO.h>

#include "Graph.hpp"
#include <unordered_map>
#include <algorithm>

namespace envire { namespace core
{
 
    /**Provides methods to draw envire graphs*/
    class GraphDrawing
    {
    public:
        
        template <class FRAME_PROP, class EDGE_PROP>
        static void writeSVG(const Graph<FRAME_PROP, EDGE_PROP> &graph, const std::string& filename)
        {
            if(filename.empty())
            {
                throw std::runtime_error("No filename specified for graph output");
            }
            std::ofstream of;
            of.open(filename.c_str());
            writeSVG(graph, of);
        }
        
        template <class FRAME_PROP, class EDGE_PROP>
        static void writeSVG(const Graph<FRAME_PROP, EDGE_PROP> &graph, std::ostream& out)
        {
            ogdf::Graph ogdfGraph;
            ogdf::GraphAttributes attrs(ogdfGraph, ogdf::GraphAttributes::edgeLabel |
                                                   ogdf::GraphAttributes::edgeGraphics |
                                                   ogdf::GraphAttributes::nodeLabel |
                                                   ogdf::GraphAttributes::nodeGraphics |
                                                   ogdf::GraphAttributes::nodeStyle);
            
            toOgdf(graph, ogdfGraph, attrs);
            
            ogdf::SugiyamaLayout SL;
            SL.call(attrs);
       
            ogdf::GraphIO::SVGSettings svgSettings;
            svgSettings.margin(400); //icrease size of viewbox to see big labels
            ogdf::GraphIO::drawSVG(attrs, out, svgSettings);
        }
        
        template <class FRAME_PROP, class EDGE_PROP>
        static void toOgdf(const Graph<FRAME_PROP, EDGE_PROP> &graph, ogdf::Graph& outGraph,
                           ogdf::GraphAttributes& outAttributes)
        {
            outGraph.clear();
            
            std::unordered_map<GraphTraits::vertex_descriptor, ogdf::node> nodes;
            
            typename Graph<FRAME_PROP, EDGE_PROP>::edge_iterator it, end;
            std::tie(it, end) = graph.getEdges();
            for(; it != end; ++it)
            {
                const GraphTraits::vertex_descriptor src = graph.getSourceVertex(*it);
                const GraphTraits::vertex_descriptor tar = graph.getTargetVertex(*it);
                
                if(nodes.find(src) == nodes.end())
                {
                    ogdf::node n = outGraph.newNode();
                    nodes[src] = n;
                    const std::string label = graph.getFrameProperty(graph.getFrameId(src)).toString();; 
                    outAttributes.label(n) = label;
                    size_t numLines = std::count(label.begin(), label.end(), '\n');
                    ++numLines; //at least one line
                    //FIXME shouldnt be constant
                    outAttributes.width(n) = 200;
                    outAttributes.height(n) = 20;
                    outAttributes.fillPattern(n) = ogdf::FillPattern::None;
                }
                
                if(nodes.find(tar) == nodes.end())
                {
                    ogdf::node n = outGraph.newNode();
                    nodes[tar] = n;
                    const std::string label = graph.getFrameProperty(graph.getFrameId(tar)).toString();; 
                    outAttributes.label(n) = label;
                    size_t numLines = std::count(label.begin(), label.end(), '\n');
                    ++numLines; //at least one line
                    outAttributes.width(n) = 200;
                    outAttributes.height(n) = 20;
                    outAttributes.fillPattern(n) = ogdf::FillPattern::None;
                }
                
                ogdf::edge edge = outGraph.newEdge(nodes[src], nodes[tar]);
                
                const std::string edgeLabel = graph.getEdgeProperty(*it).toString();
                outAttributes.label(edge) = edgeLabel;
            }
        }
    };
    
}}//end namespace
