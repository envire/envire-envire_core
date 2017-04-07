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

#include <boost/test/unit_test.hpp>
#include <envire_core/all>
#include <vector>
#include <chrono>

using namespace envire::core;
using namespace std;


vector<FrameId> ids;

void fillIds()
{
    for(int i = 0; i <= 5000000; ++i)
    {
        ids.push_back(boost::lexical_cast<FrameId>(i));
    }    
}

void addTransforms()
{
    TransformGraph graph;
    //create ids beforehand. we are not benchmarking boost

    
    Transform tf;
    auto start = chrono::steady_clock::now();
    for(size_t i = 0; i < ids.size() - size_t(1); ++i)
    {
        graph.addTransform(ids[i], ids[i + 1], tf);
    }
    auto end = chrono::steady_clock::now();
    auto diff = end - start;
    cout << ids.size() << " insert: " << chrono::duration <double, milli> (diff).count() << " ms" << endl;

}

void getTransform()
{
    TransformGraph graph;    
    Transform tf;
    for(size_t i = 0; i < ids.size() - 1; ++i)
    {
        graph.addTransform(ids[i], ids[i + 1], tf);
    }    

    auto start = chrono::steady_clock::now();
    for(size_t i = 0; i < ids.size() - 1; ++i)
    {
        graph.getTransform(ids[i], ids[i + 1]);
    }    
    auto end = chrono::steady_clock::now();
    auto diff = end - start;
    cout << ids.size() << " get 0: " << chrono::duration <double, milli> (diff).count() << " ms" << endl;
}

void getTransform1(const int len)
{ 
    TransformGraph graph;    
    Transform tf;
    for(size_t i = 0; i < ids.size() - 1; ++i)
    { 
        graph.addTransform(ids[i], ids[i + 1], tf);
    }  
    
    auto start = chrono::steady_clock::now();
    for(size_t i = 0; i < ids.size() - len + 1; ++i)
    {
        graph.getTransform(ids[i], ids[i + len]);
    }    
    auto end = chrono::steady_clock::now();
    auto diff = end - start;
    cout << ids.size() << " get " << len <<": " << chrono::duration <double, milli> (diff).count() << " ms" << endl;
}


BOOST_AUTO_TEST_CASE(add_transforms)
{
    fillIds();
    addTransforms();
    getTransform();
    for(int i = 1; i < 10; ++i)
      getTransform1(i);
    
  
}
