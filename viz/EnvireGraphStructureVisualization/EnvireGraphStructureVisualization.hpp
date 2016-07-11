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

namespace envire { namespace core 
{
  class Transform;
}}

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
    
    /**Convert envire transform to osg transform */
    std::pair<osg::Quat, osg::Vec3d> convertTransform(const envire::core::Transform& tf) const;
    osg::ref_ptr<osg::Node> drawGraphStructure(const envire::core::EnvireGraph& graph,
                                                const envire::core::FrameId& root);
    
      
  private:
    struct Data;
    std::unique_ptr<Data> p;
  };
}
#endif
