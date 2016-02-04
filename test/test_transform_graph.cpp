#include <boost/test/unit_test.hpp>
#define protected public
#include <envire_core/graph/TransformGraph.hpp>
#include <vector>
#include <string>

using namespace envire::core;
using namespace std;

class FrameProp 
{
public:
  string id;
  const string& getId() const {return id;}
  void setId(const string& _id) {id = _id;}
};

using Tfg = TransformGraph<FrameProp>;
using edge_descriptor = GraphTraits::edge_descriptor;
using vertex_descriptor = GraphTraits::vertex_descriptor;


void compareTranslation(const Transform& a, const Transform& b)
{
    BOOST_CHECK(a.transform.translation.x() == b.transform.translation.x());
    BOOST_CHECK(a.transform.translation.y() == b.transform.translation.y());
    BOOST_CHECK(a.transform.translation.z() == b.transform.translation.z());
  
}

void compareTransform(const Transform& a, const Transform& b)
{
    BOOST_CHECK(a.transform.translation.x() == b.transform.translation.x());
    BOOST_CHECK(a.transform.translation.y() == b.transform.translation.y());
    BOOST_CHECK(a.transform.translation.z() == b.transform.translation.z());
    BOOST_CHECK(a.transform.orientation.x() == b.transform.orientation.x());
    BOOST_CHECK(a.transform.orientation.y() == b.transform.orientation.y());
    BOOST_CHECK(a.transform.orientation.z() == b.transform.orientation.z()); 
    BOOST_CHECK(a.transform.orientation.w() == b.transform.orientation.w());
}


BOOST_AUTO_TEST_CASE(create_test)
{
    Tfg g;
}

BOOST_AUTO_TEST_CASE(get_transform_from_vertices_test)
{
    Tfg graph;
    FrameId a("A");
    FrameId b("B");
    Transform tf;
    tf.transform.translation << 42, 42, 42;
    Transform invTf = tf.inverse();
    
    graph.add_edge(a, b, tf);
    vertex_descriptor aDesc = graph.getVertex(a);
    vertex_descriptor bDesc = graph.getVertex(b);
    
    Transform tf2 = graph.getTransform(aDesc, bDesc);
    compareTranslation(tf, tf2);
    
    Transform invtf2 = graph.getTransform(bDesc, aDesc);
    compareTranslation(invtf2, invTf);
    
}

BOOST_AUTO_TEST_CASE(get_transform_from_frameIds_test)
{
    Tfg graph;
    FrameId a("A");
    FrameId b("B");
    Transform tf;
    tf.transform.translation << -13, 27, 0;
    Transform invTf = tf.inverse();
    
    graph.add_edge(a, b, tf);
    
    Transform tf2 = graph.getTransform(a, b);
    compareTranslation(tf, tf2);
    
    Transform invtf2 = graph.getTransform(b, a);
    compareTranslation(invtf2, invTf);  
}

BOOST_AUTO_TEST_CASE(get_transform_with_descriptor_without_edges_test)
{
    Tfg g;
    FrameId a = "frame_a";
    FrameId b = "frame_b";
    Transform tf;
    g.add_edge(a, b, tf);
    g.disconnectFrame(a);
    vertex_descriptor aDesc = g.vertex(a);
    vertex_descriptor bDesc = g.vertex(b);
    
    BOOST_CHECK_THROW(g.getTransform(aDesc, bDesc), UnknownTransformException);
}


BOOST_AUTO_TEST_CASE(update_transform_with_descriptors_on_disconnected_graph_test)
{
    Tfg g;
    FrameId a = "frame_a";
    FrameId b = "frame_b";
    Transform tf;
    g.add_edge(a, b, tf);
    g.disconnectFrame(a);
    vertex_descriptor aDesc = g.vertex(a);
    vertex_descriptor bDesc = g.vertex(b);
    //test on totally disconnected graph
    BOOST_CHECK_THROW(g.updateTransform(aDesc, bDesc, tf), UnknownEdgeException);
    
    //test on partially disconnected graph
    FrameId c = "frame_c";
    g.add_edge(a, c, tf);
    BOOST_CHECK_THROW(g.updateTransform(aDesc, bDesc, tf), UnknownEdgeException);
    
}

BOOST_AUTO_TEST_CASE(simple_add_get_transform_test)
{
    FrameId a = "frame_a";
    FrameId b = "frame_b";
    Tfg graph;
    Transform tf;
    tf.transform.translation << 42, 21, -42;
    tf.transform.orientation = base::AngleAxisd(0.25, base::Vector3d::UnitX());
    BOOST_CHECK_NO_THROW(graph.add_edge(a, b, tf));
    BOOST_CHECK(graph.num_edges() == 2);
    BOOST_CHECK(graph.num_vertices() == 2);
    Transform readTf = graph.getTransform(a, b);
    compareTransform(readTf, tf);
    Transform invTf = tf;
    invTf.setTransform(tf.transform.inverse());
    Transform readTfInv = graph.getTransform(b, a);
    compareTransform(readTfInv, invTf);
}


BOOST_AUTO_TEST_CASE(complex_add_get_transform_test)
{

    FrameId a = "frame_a";
    FrameId b = "frame_b";
    FrameId c = "frame_c";
    FrameId d = "frame_d";
    Tfg graph;
    Transform tf;
    tf.transform.translation << 42, 21, -42;
    tf.transform.orientation = base::AngleAxisd(0.25, base::Vector3d::UnitX());
    BOOST_CHECK_NO_THROW(graph.addTransform(a, b, tf));
    BOOST_CHECK_NO_THROW(graph.addTransform(b, c, tf));
    BOOST_CHECK_NO_THROW(graph.addTransform(a, d, tf));
    BOOST_CHECK(graph.num_edges() == 6);
    BOOST_CHECK(graph.num_vertices() == 4);

    /* a -> c **/
    Transform readTf;
    BOOST_CHECK_NO_THROW(readTf = graph.getTransform(a, c));
    compareTransform(readTf, tf*tf);

    /* c -> a **/
    Transform invTf;
    invTf.setTransform(tf.transform.inverse() * tf.transform.inverse());
    Transform readTfInv;
    BOOST_CHECK_NO_THROW(readTfInv = graph.getTransform(c, a));
    compareTransform(readTfInv, invTf);

    /* a -> d **/
    BOOST_CHECK_NO_THROW(readTf = graph.getTransform(a, d));
    compareTransform(readTf, tf);

    /* c -> d **/
    Transform complexTf;
    complexTf.setTransform(tf.transform.inverse()*tf.transform.inverse()*tf.transform);
    BOOST_CHECK_NO_THROW(readTf = graph.getTransform(c, d));
    compareTransform(readTf, complexTf);

    /* d -> c **/
    complexTf.setTransform(tf.transform.inverse()*tf.transform*tf.transform);
    BOOST_CHECK_NO_THROW(readTf = graph.getTransform(d, c));
    compareTransform(readTf, complexTf);

    //Close a cycle with an extra frame.
    //In practice it should not happen in envire
    FrameId e = "frame_e";
    BOOST_CHECK_NO_THROW(graph.addTransform(d, e, tf));
    BOOST_CHECK_NO_THROW(graph.addTransform(e, c, tf));

    /* d -> c **/
    BOOST_CHECK_NO_THROW(readTf = graph.getTransform(d, c));
    compareTransform(readTf, tf*tf);

    /* a-> e **/
    BOOST_CHECK_NO_THROW(readTf = graph.getTransform(a, e));
    compareTransform(readTf, tf*tf);

    FrameId f = "frame_f";
    BOOST_CHECK_NO_THROW(graph.addTransform(d, f, tf));

    /* c -> f **/
    BOOST_CHECK_NO_THROW(readTf = graph.getTransform(c, f));
    complexTf.setTransform(tf.transform.inverse()*tf.transform.inverse()*tf.transform);
    compareTransform(readTf, complexTf);

}

BOOST_AUTO_TEST_CASE(add_transform_exception_test)
{
    FrameId a = "frame_a";
    FrameId b = "frame_b";
    Tfg graph;
    Transform tf;
    BOOST_CHECK_NO_THROW(graph.addTransform(a, b, tf));
    BOOST_CHECK_THROW(graph.addTransform(b, a, tf), EdgeAlreadyExistsException);
}

BOOST_AUTO_TEST_CASE(get_transform_exception_test)
{
    FrameId a = "frame_a";
    FrameId c = "frame_c";
    Tfg graph;
    Transform tf;
    BOOST_CHECK_THROW(graph.getTransform(a, c), UnknownFrameException);
}


BOOST_AUTO_TEST_CASE(get_transform_between_unconnected_trees)
{
    Tfg graph;
    FrameId a("a");
    FrameId b("b");
    FrameId c("c");
    FrameId d("d");
    Transform tf;

    graph.addTransform(a, b, tf);
    graph.addTransform(c, d, tf);
    BOOST_CHECK_THROW(graph.getTransform(a, c), UnknownTransformException);
}

BOOST_AUTO_TEST_CASE(get_transform_using_a_tree)
{

    Tfg graph;
    /*       a
     *      / \
     *     c   b
     *   /  \
     *  d   e
     *    /  \
     *   f   g
     */

    FrameId a = "frame_a";
    FrameId b = "frame_b";
    FrameId c = "frame_c";
    FrameId d = "frame_d";
    FrameId e = "frame_e";
    FrameId f = "frame_f";
    FrameId g = "frame_g";

    Transform tf;
    tf.transform.translation << 1, 2, 1;
    tf.transform.orientation = base::AngleAxisd(0.25, base::Vector3d::UnitZ());

    graph.addTransform(a, b, tf);
    graph.addTransform(a, c, tf);
    graph.addTransform(c, d, tf);
    graph.addTransform(c, e, tf);
    graph.addTransform(e, f, tf);
    graph.addTransform(e, g, tf);

    //use a as root
    TreeView view = graph.getTree(graph.getVertex(a));
    BOOST_CHECK(view.root == graph.getVertex(a));

    Transform tree_tf_a_f = graph.getTransform(a, f, view);
    Transform graph_tf_a_f = graph.getTransform(a, f);

    /** Compare transform cannot be used due to round-off errors **/
    BOOST_CHECK(tree_tf_a_f.transform.translation.isApprox(graph_tf_a_f.transform.translation));
    BOOST_CHECK(tree_tf_a_f.transform.orientation.isApprox(graph_tf_a_f.transform.orientation));

    Transform tree_tf_d_g = graph.getTransform(d, g, view);
    Transform graph_tf_d_g = graph.getTransform(d, g);


    /** Compare transform cannot be used due to round-off errors **/
    BOOST_CHECK(tree_tf_d_g.transform.translation.isApprox(graph_tf_d_g.transform.translation));
    BOOST_CHECK(tree_tf_d_g.transform.orientation.isApprox(graph_tf_d_g.transform.orientation));

    Transform tree_tf_f_g = graph.getTransform(f, g, view);
    Transform graph_tf_f_g = graph.getTransform(f, g);


    /** Compare transform cannot be used due to round-off errors **/
    BOOST_CHECK(tree_tf_f_g.transform.translation.isMuchSmallerThan(0.01));
    BOOST_CHECK(graph_tf_f_g.transform.translation.isApprox(Eigen::Vector3d::Zero()));
    BOOST_CHECK(tree_tf_f_g.transform.orientation.isApprox(graph_tf_f_g.transform.orientation));

    /** Identity **/
    Transform tree_tf_f_f = graph.getTransform(f, f, view);
    BOOST_CHECK(tree_tf_f_f.transform.translation.isApprox(Eigen::Vector3d::Zero()));
    BOOST_CHECK(tree_tf_f_f.transform.orientation.isApprox(Eigen::Quaterniond::Identity()));
}


BOOST_AUTO_TEST_CASE(get_transform_with_descriptor_between_unconnected_frames_test)
{
    Tfg graph;
    FrameId a("A");
    FrameId b("B");
    graph.addFrame(a);
    graph.addFrame(b);
    
    vertex_descriptor aDesc = graph.getVertex(a);
    vertex_descriptor bDesc = graph.getVertex(b);
    
    BOOST_CHECK_THROW(graph.getTransform(aDesc, bDesc), UnknownTransformException);
    
}

BOOST_AUTO_TEST_CASE(get_transform_from_edge_test)
{
    Tfg graph;
    FrameId a("A");
    FrameId b("B");
    Transform tf;
    tf.transform.translation << 42, 42, 42;
    
    graph.addTransform(a, b, tf);
    edge_descriptor edge = graph.getEdge(a, b);
    Transform tf2 = graph.getTransform(edge);
    BOOST_CHECK(tf.transform.translation.x() == tf2.transform.translation.x());
    BOOST_CHECK(tf.transform.translation.y() == tf2.transform.translation.y());
    BOOST_CHECK(tf.transform.translation.z() == tf2.transform.translation.z());
}


// // class Vector: public envire::core::Item<Eigen::Vector3d>{};
// 
// BOOST_AUTO_TEST_CASE(graphviz_test)
// {
//     TransformGraph graph;
//     
//     for(int i = 0; i < 12; ++i)
//     {
//         FrameId origin = "frame_" + boost::lexical_cast<std::string>(i);
//         FrameId target = "frame_" + boost::lexical_cast<std::string>(i + 1);
//         Transform tf;
//         graph.addTransform(origin, target, tf);
//     }
//     GraphViz viz;
//     viz.write(graph, "transformGraph_graphviz_test.dot");
// }
// 
// BOOST_AUTO_TEST_CASE(complex_graphviz_test)
// {
//     TransformGraph graph;
//     
//     const FrameId a = "frame_a";
//     const FrameId b = "frame_b";
//     const FrameId c = "frame_c";
//     Transform aToB;
//     aToB.transform.translation << 1, 2, 3;
//     Transform bToC;
//     bToC.transform.translation << 42, 44, -3;
//     
//     graph.addTransform(a, b, aToB);
//     graph.addTransform(b, c, bToC);
//     
//     Item<string>::Ptr item1(new Item<string>("So say we all!"));
//     Item<int>::Ptr item2(new Item<int>(42));
//     Item<float>::Ptr item3(new Item<float>(21.0f)); 
//     
//     graph.addItemToFrame(a, item1);
//     graph.addItemToFrame(a, item2);
//     graph.addItemToFrame(a, item3);
//     
//     GraphViz viz;
//     viz.write(graph, "transformGraph_complex_graphviz_test.dot");
// }
// 



