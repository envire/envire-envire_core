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

#ifndef envire_core_EnvireGraphStructureVisualization_H
#define envire_core_EnvireGraphStructureVisualization_H


#include <boost/noncopyable.hpp>

#ifndef Q_MOC_RUN
//workaround for qt bug ( 4.7.3, 4.7.4, 4.8.0, 4.8.1 ) with boost 1.48
//https://bugreports.qt.io/browse/QTBUG-22829
#include <envire_core/graph/EnvireGraph.hpp> 
#endif


#include <vizkit3d/Vizkit3DPlugin.hpp>
#include <osg/Geode>

#include <memory>


namespace vizkit3d
{
  /**A Vizkit3D plugin, that draws the structure of a given EnvireGraph*/
  class EnvireGraphStructureVisualization 
      : public vizkit3d::Vizkit3DPlugin<envire::core::EnvireGraph>
      , boost::noncopyable
  {
  Q_OBJECT
  Q_PROPERTY(QStringList rootNode READ getNodes WRITE setNodes)  

  public:
    EnvireGraphStructureVisualization();
    ~EnvireGraphStructureVisualization();

    Q_INVOKABLE void updateData(envire::core::EnvireGraph const &sample);
  
  public slots:
    QStringList getNodes();
    void setNodes(const QStringList& values);
  
  
  protected:
    virtual osg::ref_ptr<osg::Node> createMainNode();
    virtual void updateMainNode(osg::Node* node);
    virtual void updateDataIntern(envire::core::EnvireGraph const& plan);
      
  private:
    void initNodeList(envire::core::EnvireGraph const &graph);
    
    /** Check if two lists have the same content*/
    bool areSame(const QStringList& a, const QStringList& b) const;
    
    osg::ref_ptr<osg::Node> drawGraphStructure(const envire::core::EnvireGraph& graph,
                                                const envire::core::FrameId& root);
    
      
  private:
    struct Data;
    std::unique_ptr<Data> p;
  };
}
#endif
