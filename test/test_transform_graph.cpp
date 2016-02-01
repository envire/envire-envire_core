// #include <boost/test/unit_test.hpp>
// #define protected public
// #include <envire_core/all>
// 
// #include <vector>
// #include <string>
// 
// using namespace envire::core;
// using namespace std;
// 
// bool compareTransform(const Transform& a, const Transform& b)
// {
//     return a.transform.translation.x() == b.transform.translation.x() &&
//            a.transform.translation.y() == b.transform.translation.y() &&
//            a.transform.translation.z() == b.transform.translation.z() &&
//            a.transform.orientation.x() == b.transform.orientation.x() &&
//            a.transform.orientation.y() == b.transform.orientation.y() &&
//            a.transform.orientation.z() == b.transform.orientation.z() &&
//            a.transform.orientation.w() == b.transform.orientation.w();
// }
// 
// class Dispatcher : public GraphEventDispatcher {
// public:
//     vector<TransformAddedEvent> transformAddedEvents;
//     vector<TransformModifiedEvent> transformModifiedEvents;
//     vector<TransformRemovedEvent> transformRemovedEvents;
//     vector<FrameAddedEvent> frameAddedEvents;
//     
//     Dispatcher(TransformGraph& graph) : GraphEventDispatcher(&graph) {}
//     virtual ~Dispatcher() {}
//     
//     virtual void transformAdded(const TransformAddedEvent& e)
//     {
//         transformAddedEvents.push_back(e);
//     }
// 
//     virtual void transformModified(const TransformModifiedEvent& e)
//     {
//         transformModifiedEvents.push_back(e);
//     }
// 
//     virtual void transformRemoved(const TransformRemovedEvent& e)
//     {
//         transformRemovedEvents.push_back(e);
//     }
//     virtual void frameAdded(const FrameAddedEvent& e)
//     {
//         frameAddedEvents.push_back(e);
//     }
// };
// 
// 
// //a simple item event subscriber that logs all events
// struct ItemEventSubscriber : public GraphItemEventDispatcher<Item<string>>,
//                              public GraphItemEventDispatcher<Item<int>>,
//                              public GraphItemEventDispatcher<Item<float>>
// {
//     vector<TypedItemAddedEvent<Item<string>>> stringItemAddedEvents;
//     vector<TypedItemAddedEvent<Item<int>>> intItemAddedEvents;
//     vector<TypedItemAddedEvent<Item<float>>> floatItemAddedEvents; 
//     vector<TypedItemRemovedEvent<Item<string>>> stringItemRemovedEvents;
//     vector<TypedItemRemovedEvent<Item<int>>> intItemRemovedEvents;
//     vector<TypedItemRemovedEvent<Item<float>>> floatItemRemovedEvents; 
// 
//     ItemEventSubscriber(TransformGraph& graph) : 
//       GraphItemEventDispatcher<Item<string>>(&graph),
//       GraphItemEventDispatcher<Item<int>>(&graph),
//       GraphItemEventDispatcher<Item<float>>(&graph) {}
// 
//     virtual void itemAdded(const TypedItemAddedEvent<Item<string>>& event)
//     {
//         stringItemAddedEvents.push_back(event);
//     }
//     
//     virtual void itemAdded(const TypedItemAddedEvent<Item<int>>& event)
//     {
//         intItemAddedEvents.push_back(event);
//     }
//     
//     virtual void itemAdded(const TypedItemAddedEvent<Item<float>>& event)
//     {
//         floatItemAddedEvents.push_back(event);
//     }
//     
//     virtual void itemRemoved(const TypedItemRemovedEvent<Item<string>>& event)
//     {
//         stringItemRemovedEvents.push_back(event);
//     }
//     
//     virtual void itemRemoved(const TypedItemRemovedEvent<Item<int>>& event)
//     {
//         intItemRemovedEvents.push_back(event);
//     }
//     
//     virtual void itemRemoved(const TypedItemRemovedEvent<Item<float>>& event)
//     {
//         floatItemRemovedEvents.push_back(event);
//     }
//     
// };
// 
// 
// 
// BOOST_AUTO_TEST_CASE(simple_add_item_test)
// {
//     FrameId aFrame = "frame_a";
//     const string text("I am so smart, S M A T");
//     
//     TransformGraph g;
//     g.addFrame(aFrame);
//     
// 
//     Item<string>::Ptr item(new Item<string>(text));
//     g.addItemToFrame(aFrame, item);
// 
//     using Iterator = TransformGraph::ItemIterator<Item<string>>;
//     
//     
//     Iterator begin, end;
//     boost::tie(begin, end) = g.getItems<Item<string>>(aFrame);
//     BOOST_CHECK(begin != end);
//     BOOST_CHECK(begin->getData().compare(text) == 0);
// 
// }
// 
// BOOST_AUTO_TEST_CASE(simple_add_item_non_generic_test)
// {
//     FrameId aFrame = "frame_a";
//     const string text("you win again, gravity!");
//     
//     TransformGraph g;
//     g.addFrame(aFrame);
//     
//     ItemBase::Ptr item(new Item<string>(text));
//     item->setFrame(aFrame);
//     g.addItem(item);
//     
//     using Iterator = TransformGraph::ItemIterator<Item<string>>;
// 
//     Iterator begin, end;
//     boost::tie(begin, end) = g.getItems<Item<string>>(aFrame);
//     BOOST_CHECK(begin != end);
//     BOOST_CHECK(begin->getData().compare(text) == 0);
// }
// 
// 
// BOOST_AUTO_TEST_CASE(add_multiple_items_test)
// {
//     FrameId a = "frame_a";
//     struct DataA 
//     {
//         DataA(const string& value) : value(value){}
//         string value;
//     };
//     struct DataB
//     {
//         DataB(const int value) : value(value){}
//         int value;
//     };
//     
//     TransformGraph g;
//     g.addFrame(a);
// 
// 
//     Item<DataA>::Ptr a1(new Item<DataA>("Grandpa, why don't you tell a story?"));
//     Item<DataA>::Ptr a2(new Item<DataA>("Yeah Grandpa, you lived a long and interesting life."));
//     Item<DataA>::Ptr a3(new Item<DataA>("That's a lie and you know it"));
//     
//     Item<DataB>::Ptr b1(new Item<DataB>(42));
//     Item<DataB>::Ptr b2(new Item<DataB>(21));
//     Item<DataB>::Ptr b3(new Item<DataB>(84));
//     
//     g.addItemToFrame(a, a1);
//     g.addItemToFrame(a, a2);
//     g.addItemToFrame(a, a3);
//     g.addItemToFrame(a, b1);
//     g.addItemToFrame(a, b2);
//     g.addItemToFrame(a, b3);
//     
//     using AIterator = TransformGraph::ItemIterator<Item<DataA>>;
//     using BIterator = TransformGraph::ItemIterator<Item<DataB>>;
//     
//     AIterator aBegin, aEnd;
//     boost::tie(aBegin, aEnd) = g.getItems<Item<DataA>>(a);
//     BOOST_CHECK(aBegin != aEnd);
//     BOOST_CHECK(aBegin->getData().value.compare(a1->getData().value) == 0);
//     ++aBegin;
//     BOOST_CHECK(aBegin->getData().value.compare(a2->getData().value) == 0);
//     ++aBegin;
//     BOOST_CHECK(aBegin->getData().value.compare(a3->getData().value) == 0);
//     ++aBegin;
//     BOOST_CHECK(aBegin == aEnd);
//     
//     BIterator bBegin, bEnd;
//     boost::tie(bBegin, bEnd) = g.getItems<Item<DataB>>(a);
//     BOOST_CHECK(bBegin != bEnd);
//     BOOST_CHECK(bBegin->getData().value == 42);
//     ++bBegin;
//     BOOST_CHECK(bBegin->getData().value == 21);
//     ++bBegin;
//     BOOST_CHECK(bBegin->getData().value == 84);
//     ++bBegin;
//     BOOST_CHECK(bBegin == bEnd);
// }
// 
// BOOST_AUTO_TEST_CASE(add_item_to_invalid_frame)
// {
//     FrameId a = "frame_a";
//     const string text("I am so smart, S M A T");
//     TransformGraph g;
//     Item<string>::Ptr item(new Item<string>(text));
//     BOOST_CHECK_THROW(g.addItemToFrame(a, item), UnknownFrameException); 
// }
// 
// BOOST_AUTO_TEST_CASE(simple_remove_item_from_frame_test)
// {
//     FrameId frame = "frame";
//     TransformGraph g;
//     g.addFrame(frame);
//     const string text("Good news everyone!");
//     Item<string>::Ptr item(new Item<string>(text));
//     g.addItemToFrame(frame, item);
//     
//     using Iterator = TransformGraph::ItemIterator<Item<string>>;
//     Iterator it = g.getItem<Item<string>>(frame, 0);
//     
//     g.removeItemFromFrame(frame, it);
//     
//     Iterator begin, end;
//     boost::tie(begin, end) = g.getItems<Item<string>>(frame);
//     BOOST_CHECK(begin == end);
// }
// 
// 
// 
// BOOST_AUTO_TEST_CASE(remove_multiple_items_from_frame_test)
// {
//     FrameId frame = "frame";
//     TransformGraph g;
//     g.addFrame(frame);
//     const string text("This Concept Of ‘Wuv’ Confused And Infuriates Us!");
//     
//     for(int i = 0; i < 42; ++i)
//     {
//         Item<string>::Ptr item(new Item<string>(text));
//         g.addItemToFrame(frame, item);
//     }
//     using Iterator = TransformGraph::ItemIterator<Item<string>>;
//     Iterator begin, end;
//     boost::tie(begin, end) = g.getItems<Item<string>>(frame);
//     
//     BOOST_CHECK(end - begin == 42);//check if there are really 42 items in the vector now
//     for(; begin != end;)
//     {
//         boost::tie(begin, end) = g.removeItemFromFrame(frame, begin);
//     }
//     BOOST_CHECK(begin == end);
//     boost::tie(begin, end) = g.getItems<Item<string>>(frame);
//     BOOST_CHECK(begin == end);
//    
// }
// 
// BOOST_AUTO_TEST_CASE(get_item_test)
// {
//     FrameId frame = "Saturn";
//     TransformGraph g;
//     g.addFrame(frame);
//     const string text("Ford... you're turning into a penguin. Stop it.");
//     Item<string>::Ptr item(new Item<string>(text));
//     g.addItemToFrame(frame, item);
//     
//     Item<string>& retItem = *g.getItem<Item<string>>(frame, 0);
//     BOOST_CHECK(retItem.getData().compare(text) == 0);
// }
// 
// BOOST_AUTO_TEST_CASE(get_items_vertex_desc_non_generic_test)
// {
//     FrameId frame = "Saturn";
//     TransformGraph g;
//     g.addFrame(frame);
//     const string text("I would love to change the world, but they won't give me the source code.");
//     const string text2("Nothing travels faster than the speed of light, with the possible exception of bad news, which obeys its own special laws.");
//     Item<string>::Ptr item(new Item<string>(text));
//     Item<string>::Ptr item2(new Item<string>(text2));
//     g.addItemToFrame(frame, item);
//     g.addItemToFrame(frame, item2);
//     
//     vertex_descriptor desc = g.getVertex(frame);
//     const Frame::ItemList& items = g.getItems(desc, item->getTypeIndex());
//     BOOST_CHECK(items.size() == 2);
//     Item<string>::Ptr itemResult =  boost::dynamic_pointer_cast<Item<string>>(items[0]);
//     BOOST_CHECK(itemResult->getData().compare(text) == 0);
//     Item<string>::Ptr itemResult2 =  boost::dynamic_pointer_cast<Item<string>>(items[1]);
//     BOOST_CHECK(itemResult2->getData().compare(text2) == 0);
// }
// 
// BOOST_AUTO_TEST_CASE(get_items_vertex_desc_non_generic_error_case_test)
// {
//     FrameId frame = "Saturn";
//     TransformGraph g;
//     g.addFrame(frame);
//     Item<string>::Ptr item(new Item<string>("bla"));
//     vertex_descriptor desc = g.getVertex(frame);
//     BOOST_CHECK_THROW(g.getItems(desc, item->getTypeIndex()), NoItemsOfTypeInFrameException);
// }
// 
// BOOST_AUTO_TEST_CASE(get_item_on_empty_frame_test)
// {
//     FrameId frame = "Saturn";
//     TransformGraph g;
//     g.addFrame(frame);
//     BOOST_CHECK_THROW(g.getItem<Item<string>>(frame, 42), NoItemsOfTypeInFrameException);
// }   
// 
// BOOST_AUTO_TEST_CASE(get_item_wrong_index_test)
// {
//     FrameId frame = "Mars";
//     TransformGraph g;
//     g.addFrame(frame);
//     Item<string>::Ptr item(new Item<string>("Arthur: If I asked you where the hell we were, would I regret it? -- Ford: We're safe"));
//     g.addItemToFrame(frame, item);
//     BOOST_CHECK_THROW(g.getItem<Item<string>>(frame, 42), std::out_of_range);
// }   
// 
// BOOST_AUTO_TEST_CASE(get_first_item_on_empty_graph_test)
// {
//     FrameId frame = "blaa";
//     TransformGraph g;
//     BOOST_CHECK_THROW(g.getItem<Item<string>>(frame, 11), UnknownFrameException);
// }
// 
// BOOST_AUTO_TEST_CASE(remove_transform_event_test)
// {
//     FrameId a = "frame_a";
//     FrameId b = "frame_b";
//     TransformGraph graph;
//     Transform tf;
//     tf.transform.translation << 42, 21, -42;
//     tf.transform.orientation = base::AngleAxisd(0.25, base::Vector3d::UnitX());
//     graph.addTransform(a, b, tf);
//     Dispatcher d(graph);
//     graph.removeTransform(a, b);
//     BOOST_CHECK(d.transformRemovedEvents.size() == 2);
//     BOOST_CHECK(d.transformRemovedEvents[0].origin == a);
//     BOOST_CHECK(d.transformRemovedEvents[0].target == b);
//     BOOST_CHECK(d.transformRemovedEvents[1].origin == b);
//     BOOST_CHECK(d.transformRemovedEvents[1].target == a);
// }
// 
// BOOST_AUTO_TEST_CASE(simple_remove_transform_test)
// {
       //FIXME partly tested in test_graph
//     FrameId a = "frame_a";
//     FrameId b = "frame_b";   
//     TransformGraph tree;
//     Transform tf;
//     tf.transform.translation << 42, 21, -42;
//     tf.transform.orientation = base::AngleAxisd(0.25, base::Vector3d::UnitX());
//     tree.addTransform(a, b, tf);
// 
//     tree.removeTransform(b, a);
//     BOOST_CHECK(tree.num_edges() == 0);
//     BOOST_CHECK(tree.num_vertices() == 2); //vertices are still there, just not connected anymore
// }
// 
// BOOST_AUTO_TEST_CASE(remove_existing_but_unconnected_test)
// {
      //FIXME implemented in simple_add_remove_edge_test
//     FrameId a = "frame_a";
//     FrameId b = "frame_b";
//     FrameId c = "frame_c";
//     TransformGraph tree;
//     Transform tf;
//     tree.addTransform(a, b, tf);
//     tree.addTransform(a, c, tf);
//     BOOST_CHECK_THROW(tree.removeTransform(b, c), UnknownTransformException);
// }
// 
// BOOST_AUTO_TEST_CASE(complex_remove_transform_test)
// {
//     FrameId a = "frame_a";
//     FrameId b = "frame_b";
//     FrameId c = "frame_c";
//     TransformGraph tree;
//     Transform tf;
//     tf.transform.translation << 42, 21, -42;
//     tf.transform.orientation = base::AngleAxisd(0.25, base::Vector3d::UnitX());
//     tree.addTransform(a, b, tf);
//     tree.addTransform(a, c, tf);
// 
//     tree.removeTransform(b, a);
//     BOOST_CHECK(tree.num_edges() == 2);
//     BOOST_CHECK(tree.num_vertices() == 3);
//     BOOST_CHECK_NO_THROW(tree.getFrame(a));
//     BOOST_CHECK_NO_THROW(tree.getFrame(b));
//     BOOST_CHECK_NO_THROW(tree.getFrame(c));
// }
// 
// BOOST_AUTO_TEST_CASE(simple_modify_transform_test)
// {
//     FrameId a = "frame_a";
//     FrameId b = "frame_b";
//     TransformGraph tree;
//     Transform tf;
//     tf.transform.translation << 42, 21, -42;
//     tf.transform.orientation = base::AngleAxisd(0.25, base::Vector3d::UnitX());
//     BOOST_CHECK_NO_THROW(tree.addTransform(a, b, tf));
// 
//     Transform tf2;
//     tf2.transform.translation << 0, 1, 2;
//     tf2.transform.orientation = base::AngleAxisd(0.42, base::Vector3d::UnitY());
//     tree.updateTransform(b, a, tf2);
//     Transform readBa = tree.getTransform(b, a);
//     Transform readAb = tree.getTransform(a, b);
//     BOOST_CHECK(compareTransform(readBa, tf2));
// 
//     Transform tf2Inv = tf2;
//     tf2Inv.setTransform(tf2.transform.inverse());
//     BOOST_CHECK(compareTransform(readAb, tf2Inv));
// }
// 
// BOOST_AUTO_TEST_CASE(update_transform_on_empty_tree_test)
// {
//     FrameId a = "frame_a";
//     FrameId b = "frame_b";
//     TransformGraph tree;
//     Transform tf;
//     BOOST_CHECK_THROW(tree.updateTransform(a, b, tf), UnknownFrameException);
// }
// 
// BOOST_AUTO_TEST_CASE(update_transform_invalid_test)
// {
//     FrameId a = "frame_a";
//     FrameId b = "frame_b";
//     FrameId c = "frame_c";
//     TransformGraph tree;
//     Transform tf;
//     tree.addTransform(a, b, tf);
//     BOOST_CHECK_THROW(tree.updateTransform(a, c, tf), UnknownFrameException);
// }
// 
// BOOST_AUTO_TEST_CASE(remove_transform_on_empty_tree_test)
// {
//     FrameId a = "frame_a";
//     FrameId b = "frame_b";
//     TransformGraph tree;
//     BOOST_CHECK_THROW(tree.removeTransform(a, b), UnknownTransformException);
// }
// 
// BOOST_AUTO_TEST_CASE(remove_transform_invalid_test)
// {
//     FrameId a = "frame_a";
//     FrameId b = "frame_b";
//     FrameId c = "frame_c";
//     TransformGraph tree;
//     Transform tf;
//     tree.addTransform(a, b, tf);
//     BOOST_CHECK_THROW(tree.removeTransform(a, c), UnknownTransformException);
// }
// 
// BOOST_AUTO_TEST_CASE(get_invalid_transform_test)
// {
//     FrameId a = "frame_a";
//     FrameId b = "frame_b";
//     FrameId c = "frame_c";
//     TransformGraph tree;
//     Transform tf;
//     tf.transform.translation << 42, 21, -42;
//     tf.transform.orientation = base::AngleAxisd(0.25, base::Vector3d::UnitX());
//     BOOST_CHECK_NO_THROW(tree.addTransform(a, b, tf));
//     BOOST_CHECK_THROW(tree.getTransform(a, c), UnknownFrameException);
// }
// 
// BOOST_AUTO_TEST_CASE(get_transform_with_descriptor_without_edges_test)
// {
//     TransformGraph g;
//     FrameId a = "frame_a";
//     FrameId b = "frame_b";
//     Transform tf;
//     g.addTransform(a, b, tf);
//     g.disconnectFrame(a);
//     vertex_descriptor aDesc = g.vertex(a);
//     vertex_descriptor bDesc = g.vertex(b);
//     
//     BOOST_CHECK_THROW(g.getTransform(aDesc, bDesc), UnknownTransformException);
// }
// 
// BOOST_AUTO_TEST_CASE(update_transform_with_descriptors_on_disconnected_graph_test)
// {
//     TransformGraph g;
//     FrameId a = "frame_a";
//     FrameId b = "frame_b";
//     Transform tf;
//     g.addTransform(a, b, tf);
//     g.disconnectFrame(a);
//     vertex_descriptor aDesc = g.vertex(a);
//     vertex_descriptor bDesc = g.vertex(b);
//     //test on totally disconnected graph
//     BOOST_CHECK_THROW(g.updateTransform(aDesc, bDesc, tf), UnknownTransformException);
//     
//     //test on partially disconnected graph
//     FrameId c = "frame_c";
//     g.addTransform(a, c, tf);
//     BOOST_CHECK_THROW(g.updateTransform(aDesc, bDesc, tf), UnknownTransformException);
//     
// }
// 
// 
// BOOST_AUTO_TEST_CASE(get_transform_with_descriptors_test)
// {
//     TransformGraph g;
//     FrameId a = "frame_a";
//     FrameId b = "frame_b";
//     Transform tf;
//     tf.transform.translation << 42, 21, 10;
//     tf.transform.orientation = base::AngleAxisd(0.25, base::Vector3d::UnitX());
//     g.addTransform(a, b, tf);
//     vertex_descriptor aDesc = g.vertex(a);
//     vertex_descriptor bDesc = g.vertex(b);  
//     Transform tf2 = g.getTransform(aDesc, bDesc);
//     BOOST_CHECK(compareTransform(tf, tf2));
//     
// }
// 
// BOOST_AUTO_TEST_CASE(get_edge_on_empty_tree_test)
// {
//     FrameId a = "frame_a";
//     FrameId b = "frame_b";
//     TransformGraph tree;
//     BOOST_CHECK_THROW(tree.getEdge(a, b), UnknownFrameException);
// }
// 
// BOOST_AUTO_TEST_CASE(disconnect_frame_test)
// {
//     FrameId a = "frame_a";
//     FrameId b = "frame_b";
//     FrameId c = "frame_c";
//     TransformGraph tree;
//     Transform tf;
//     tree.addTransform(a, b, tf);
//     tree.addTransform(a, c, tf);
//     tree.disconnectFrame(a);
//     BOOST_CHECK_THROW(tree.getTransform(a, b), UnknownTransformException);
//     BOOST_CHECK_THROW(tree.getTransform(b, a), UnknownTransformException);
//     BOOST_CHECK_THROW(tree.getTransform(a, c), UnknownTransformException);
//     BOOST_CHECK_THROW(tree.getTransform(c, a), UnknownTransformException); 
//     
//     FrameId d = "frame_d";
//     BOOST_CHECK_THROW(tree.disconnectFrame(d), UnknownFrameException);
//     
// }
// 
// 
// BOOST_AUTO_TEST_CASE(clear_frame_test)
// {
//     FrameId frame = "frame";
//     TransformGraph g;
//     g.addFrame(frame);
//     const string text("424242");
//     
//     for(int i = 0; i < 3; ++i)
//     {
//         Item<string>::Ptr item(new Item<string>(text));
//         g.addItemToFrame(frame, item);
//     }
//     g.clearFrame(frame);
//     using Iterator = TransformGraph::ItemIterator<Item<string>>;
//     Iterator begin, end;
//     boost::tie(begin, end) = g.getItems<Item<string>>(frame);
//     BOOST_CHECK(begin == end);
// }
// 
// 
// struct Joint
// {
//   Joint(int a) : a(a) {}
//   int a;
// };
// struct Sensor
// {
// };
// 
// 
// BOOST_AUTO_TEST_CASE(check_item_existence_example)
// {
//   TransformGraph g;
//   FrameId frame = "frame";
//   g.addFrame(frame);
//   
//   Item<Joint>::Ptr item(new Item<Joint>(42));
//   g.addItemToFrame(frame, item);
//   Item<Sensor>::Ptr item2(new Item<Sensor>());
//   g.addItemToFrame(frame, item2);
//   
// //  bool contains = g.containsItems<Item<string>::Ptr>(frame);
//   
//   //using Iterator = TransformGraph::ItemIterator<Item<Joint>::Ptr>;
//   //Iterator begin, end;
//   //boost:tie(begin, end) = g.getItems<Item<Joint>::Ptr>(frame);
// 
// }
// 
// 
// BOOST_AUTO_TEST_CASE(remove_frame_test)
// {
//     FrameId a = "frame_a";
//     FrameId b = "frame_b";
//     FrameId c = "frame_c";
//     TransformGraph graph;
//     Transform tf;
//     graph.addTransform(a, b, tf);
//     graph.addTransform(a, c, tf);
//     
//     BOOST_CHECK_THROW(graph.removeFrame(a), FrameStillConnectedException);
//     graph.disconnectFrame(a);
//     graph.removeFrame(a);
//     BOOST_CHECK_THROW(graph.getFrame(a), UnknownFrameException);
// }
// 
// BOOST_AUTO_TEST_CASE(get_edge_invalid_test)
// {
//     FrameId a = "frame_a";
//     FrameId b = "frame_b";
//     FrameId c = "frame_c";
//     TransformGraph tree;
//     Transform tf;
//     tree.addTransform(a, b, tf);
//     tree.addTransform(a, c, tf);
//     BOOST_CHECK_THROW(tree.getEdge(b, c), UnknownTransformException);
// }
// 
// BOOST_AUTO_TEST_CASE(frame_added_event_test)
// {
//     FrameId a = "frame_a";
//     FrameId b = "frame_b";
//     TransformGraph graph;
//     Transform tf;  
//     Dispatcher d(graph);
//     graph.addFrame(a);
//     BOOST_CHECK(d.frameAddedEvents.size() == 1);
//     BOOST_CHECK(d.frameAddedEvents[0].addedFrame == a);
//     
//     graph.addTransform(a, b, tf);
//     BOOST_CHECK(d.frameAddedEvents.size() == 2);
//     BOOST_CHECK(d.frameAddedEvents[1].addedFrame == b);
// 
//     FrameId c = "frame_c";
//     FrameId e = "frame_d";
//     graph.addTransform(c, e, tf);
//     BOOST_CHECK(d.frameAddedEvents.size() == 4);
//     BOOST_CHECK(d.frameAddedEvents[2].addedFrame == c);
//     BOOST_CHECK(d.frameAddedEvents[3].addedFrame == e);
//     
//     BOOST_CHECK_THROW(graph.addFrame(a), FrameAlreadyExistsException);
// }
// 
// BOOST_AUTO_TEST_CASE(modify_transform_event_test)
// {
//     FrameId a = "frame_a";
//     FrameId b = "frame_b";
//     TransformGraph graph;
//     Transform tf;
//     tf.transform.translation << 42, 21, -42;
//     tf.transform.orientation = base::AngleAxisd(0.25, base::Vector3d::UnitX());
//     graph.addTransform(a, b, tf);
// 
//     Dispatcher d(graph);
//     Transform tf2;
//     tf2.transform.translation << 0, 1, 2; 
//     tf2.transform.orientation = base::AngleAxisd(0.42, base::Vector3d::UnitY());
//     graph.updateTransform(a, b, tf2);
//     BOOST_CHECK(d.transformModifiedEvents.size() == 1);
//     Transform tf2Inv = tf2;
//     tf2Inv.setTransform(tf2.transform.inverse());
//     Transform tfInv = tf;
//     tfInv.setTransform(tf.transform.inverse());
//     
//     BOOST_CHECK(d.transformModifiedEvents[0].origin == a);
//     BOOST_CHECK(d.transformModifiedEvents[0].target == b);
//     BOOST_CHECK(d.transformModifiedEvents[0].edge == graph.getEdge(a, b));
//     BOOST_CHECK(d.transformModifiedEvents[0].inverseEdge == graph.getEdge(b, a));
// }
// 
// BOOST_AUTO_TEST_CASE(operations_with_transform)
// {
//     Transform tf1, tf2;
//     tf1.transform.translation << 42, 21, -42;
//     tf1.transform.orientation = base::AngleAxisd(0.25, base::Vector3d::UnitX());
//     tf2.transform.translation << 42, 21, -42;
//     tf2.transform.orientation = base::AngleAxisd(0.25, base::Vector3d::UnitX());
// 
//     Transform tf_manual;
//     tf_manual.setTransform(tf1.transform * tf2.transform);
//     Transform tf_operator(tf1 * tf2);
//     BOOST_CHECK(compareTransform(tf_operator, tf_manual));
// }
// 
// BOOST_AUTO_TEST_CASE(simple_add_get_transform_test)
// {
//     FrameId a = "frame_a";
//     FrameId b = "frame_b";
//     TransformGraph tree;
//     Transform tf;
//     tf.transform.translation << 42, 21, -42;
//     tf.transform.orientation = base::AngleAxisd(0.25, base::Vector3d::UnitX());
//     BOOST_CHECK_NO_THROW(tree.addTransform(a, b, tf));
//     BOOST_CHECK(tree.num_edges() == 2);
//     BOOST_CHECK(tree.num_vertices() == 2);
//     Transform readTf = tree.getTransform(a, b);
//     BOOST_CHECK(compareTransform(readTf, tf));
//     Transform invTf = tf;
//     invTf.setTransform(tf.transform.inverse());
//     Transform readTfInv = tree.getTransform(b, a);
//     BOOST_CHECK(compareTransform(readTfInv, invTf));
// }
// 
// 
// BOOST_AUTO_TEST_CASE(add_transform_existing_vertex_test)
// { 
//     FrameId a = "frame_a";
//     FrameId b = "frame_b";
//     TransformGraph graph;
//     graph.addFrame(a);
//     graph.addFrame(b);
//     
//     vertex_descriptor aDesc = graph.vertex(a);
//     vertex_descriptor bDesc = graph.vertex(b);
//     
//     Transform tf;
//     graph.addTransform(aDesc, bDesc, tf);
//     BOOST_CHECK_NO_THROW(graph.getTransform(a, b));
//     
//     
// }
// 
// BOOST_AUTO_TEST_CASE(complex_add_get_transform_test)
// {
// 
//     FrameId a = "frame_a";
//     FrameId b = "frame_b";
//     FrameId c = "frame_c";
//     FrameId d = "frame_d";
//     TransformGraph tree;
//     Transform tf;
//     tf.transform.translation << 42, 21, -42;
//     tf.transform.orientation = base::AngleAxisd(0.25, base::Vector3d::UnitX());
//     BOOST_CHECK_NO_THROW(tree.addTransform(a, b, tf));
//     BOOST_CHECK_NO_THROW(tree.addTransform(b, c, tf));
//     BOOST_CHECK_NO_THROW(tree.addTransform(a, d, tf));
//     BOOST_CHECK(tree.num_edges() == 6);
//     BOOST_CHECK(tree.num_vertices() == 4);
// 
//     /* a -> c **/
//     BOOST_TEST_MESSAGE( "a - > c" );
//     Transform readTf;
//     BOOST_CHECK_NO_THROW(readTf = tree.getTransform(a, c));
//     BOOST_CHECK(compareTransform(readTf, tf*tf));
// 
//     /* c -> a **/
//     BOOST_TEST_MESSAGE( "c - > a" );
//     Transform invTf;
//     invTf.setTransform(tf.transform.inverse() * tf.transform.inverse());
//     Transform readTfInv;
//     BOOST_CHECK_NO_THROW(readTfInv = tree.getTransform(c, a));
//     BOOST_CHECK(compareTransform(readTfInv, invTf));
// 
//     /* a -> d **/
//     BOOST_TEST_MESSAGE( "a - > d" );
//     BOOST_CHECK_NO_THROW(readTf = tree.getTransform(a, d));
//     BOOST_CHECK(compareTransform(readTf, tf));
// 
//     /* c -> d **/
//     BOOST_TEST_MESSAGE( "c - > d" );
//     Transform complexTf;
//     complexTf.setTransform(tf.transform.inverse()*tf.transform.inverse()*tf.transform);
//     BOOST_CHECK_NO_THROW(readTf = tree.getTransform(c, d));
//     BOOST_CHECK(compareTransform(readTf, complexTf));
// 
//     /* d -> c **/
//     BOOST_TEST_MESSAGE( "d - > c" );
//     complexTf.setTransform(tf.transform.inverse()*tf.transform*tf.transform);
//     BOOST_CHECK_NO_THROW(readTf = tree.getTransform(d, c));
//     BOOST_CHECK(compareTransform(readTf, complexTf));
// 
//     //Close a cycle with an extra frame.
//     //In practice it should not happen in envire
//     FrameId e = "frame_e";
//     BOOST_CHECK_NO_THROW(tree.addTransform(d, e, tf));
//     BOOST_CHECK_NO_THROW(tree.addTransform(e, c, tf));
// 
//     /* d -> c **/
//     BOOST_TEST_MESSAGE( "d - > c" );
//     BOOST_CHECK_NO_THROW(readTf = tree.getTransform(d, c));
//     BOOST_CHECK(compareTransform(readTf, tf*tf));
// 
//     /* a-> e **/
//     BOOST_TEST_MESSAGE( "a - > e" );
//     BOOST_CHECK_NO_THROW(readTf = tree.getTransform(a, e));
//     BOOST_CHECK(compareTransform(readTf, tf*tf));
// 
//     FrameId f = "frame_f";
//     BOOST_CHECK_NO_THROW(tree.addTransform(d, f, tf));
// 
//     /* c -> f **/
//     BOOST_TEST_MESSAGE( "c - > f" );
//     BOOST_CHECK_NO_THROW(readTf = tree.getTransform(c, f));
//     complexTf.setTransform(tf.transform.inverse()*tf.transform.inverse()*tf.transform);
//     BOOST_CHECK(compareTransform(readTf, complexTf));
// 
// }
// 
// 
// BOOST_AUTO_TEST_CASE(add_transform_exception_test)
// {
//     FrameId a = "frame_a";
//     FrameId b = "frame_b";
//     TransformGraph tree;
//     Transform tf;
//     BOOST_CHECK_NO_THROW(tree.addTransform(a, b, tf));
//     BOOST_CHECK_THROW(tree.addTransform(b, a, tf), TransformAlreadyExistsException);
// }
// 
// BOOST_AUTO_TEST_CASE(get_transform_exception_test)
// {
//     FrameId a = "frame_a";
//     FrameId c = "frame_c";
//     TransformGraph tree;
//     Transform tf;
//     BOOST_CHECK_THROW(tree.getTransform(a, c), UnknownTransformException);
// }
// 
// 
// BOOST_AUTO_TEST_CASE(simple_add_transform_event_test)
// {
//     FrameId a = "frame_a";
//     FrameId b = "frame_b";
//     TransformGraph graph;
//     Transform tf;
//     tf.transform.translation << 42, 21, -42;
//     tf.transform.orientation = base::AngleAxisd(0.25, base::Vector3d::UnitX());
// 
//     Dispatcher d(graph);
// 
//     graph.addTransform(a, b, tf);
//     BOOST_CHECK(d.transformAddedEvents.size() == 2);
//     edge_descriptor aToB = graph.getEdge(a, b);
//     BOOST_CHECK(d.transformAddedEvents[0].edge == aToB);
//     edge_descriptor bToA = graph.getEdge(b, a);
//     BOOST_CHECK(d.transformAddedEvents[1].edge == bToA);
//     //annoying amount of checks because there is no operator== in eigen
//     BOOST_CHECK(compareTransform(d.transformAddedEvents[0].transform, tf));
//     Transform invTf = tf;
//     invTf.setTransform(tf.transform.inverse());
//     BOOST_CHECK(compareTransform(d.transformAddedEvents[1].transform, invTf));
// }
// 
// 
// 
// BOOST_AUTO_TEST_CASE(get_item_empty_graph_test)
// {
//     FrameId a = "frame_a";
//     const string text("Don't Panic.");
//     TransformGraph g;
//     BOOST_CHECK_THROW(g.getItems<Item<string>>(a), UnknownFrameException);
// }
// 
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
// 
// 


// 
// BOOST_AUTO_TEST_CASE(simple_get_tree_with_invalid_frameId_test)
// {
//     FrameId a = "frame_a";
//     TransformGraph graph;
//     BOOST_CHECK_THROW(graph.getTree(a), UnknownFrameException);
// }
// 
// 
// 
// 
// BOOST_AUTO_TEST_CASE(remove_unknown_frame_test)
// {
//     FrameId a = "frame_a";
//     TransformGraph graph;
//     BOOST_CHECK_THROW(graph.removeFrame(a), UnknownFrameException);
// }
// 
// BOOST_AUTO_TEST_CASE(clear_unknown_frame_test)
// {
//     FrameId a = "frame_a";
//     TransformGraph graph;
//     BOOST_CHECK_THROW(graph.clearFrame(a), UnknownFrameException);
// }
// 
// BOOST_AUTO_TEST_CASE(add_item_event_test)
// {
//     TransformGraph graph;
//     FrameId a("a");
//     FrameId b("b");
//     Transform tf;
//     
//     graph.addTransform(a, b, tf);
//     
//     Item<string>::Ptr item1(new Item<string>("Time is an illusion. Lunchtime doubly so."));
//     Item<int>::Ptr item2(new Item<int>(42));
//     Item<float>::Ptr item3(new Item<float>(21.0f)); 
//     
//     ItemEventSubscriber sub(graph);
// 
//     graph.addItemToFrame(a, item1);
//     graph.addItemToFrame(b, item2);
//     graph.addItemToFrame(a, item3);
//     
// 
//     
//     BOOST_CHECK(sub.floatItemAddedEvents.size() == 1);
//     BOOST_CHECK(sub.intItemAddedEvents.size() == 1);
//     BOOST_CHECK(sub.stringItemAddedEvents.size() == 1);   
//     
//     BOOST_CHECK(sub.floatItemAddedEvents.front().frame == a);
//     BOOST_CHECK(sub.intItemAddedEvents.front().frame == b);
//     BOOST_CHECK(sub.stringItemAddedEvents.front().frame == a);
//     BOOST_CHECK(sub.stringItemAddedEvents.front().item == item1);
//     BOOST_CHECK(sub.intItemAddedEvents.front().item == item2);
//     BOOST_CHECK(sub.floatItemAddedEvents.front().item == item3);
// }
// 
// BOOST_AUTO_TEST_CASE(multiple_item_event_destructor_test)
// {
//     struct ItemEventSubscriber : public GraphItemEventDispatcher<Item<string>>,
//                                  public GraphItemEventDispatcher<Item<int>>,
//                                  public GraphItemEventDispatcher<Item<float>>
//     {
//         ItemEventSubscriber(TransformGraph& graph) : 
//         GraphItemEventDispatcher<Item<string>>(&graph),
//         GraphItemEventDispatcher<Item<int>>(&graph),
//         GraphItemEventDispatcher<Item<float>>(&graph)
//         {
//         }
//         
//         virtual ~ItemEventSubscriber()
//         {
//         }
//     };
//     TransformGraph g;
//     ItemEventSubscriber s(g);
// }
// 
// 
// BOOST_AUTO_TEST_CASE(remove_item_event_test)
// {
//     TransformGraph graph;
//     FrameId a("a");
//     FrameId b("b");
//     Transform tf;
//     
//     graph.addTransform(a, b, tf);
//     Item<string>::Ptr item1(new Item<string>("The ships hung in the sky in much the same way that bricks don't."));
//     Item<int>::Ptr item2(new Item<int>(42));
//     Item<float>::Ptr item3(new Item<float>(21.0f)); 
//     
//     ItemEventSubscriber sub(graph);
//     graph.addItemToFrame(a, item1);
//     graph.addItemToFrame(b, item2);
//     graph.addItemToFrame(a, item3);
//     
//     using StrIterator = TransformGraph::ItemIterator<Item<string>>;
//     using FloatIterator = TransformGraph::ItemIterator<Item<float>>;
//     
//     
//     StrIterator strIt = graph.getItem<Item<string>>(a, 0);
//     graph.removeItemFromFrame(a, strIt);
//     BOOST_CHECK(sub.stringItemRemovedEvents.size() == 1);
//     BOOST_CHECK(sub.stringItemRemovedEvents.front().frame == a);
//     BOOST_CHECK(sub.stringItemRemovedEvents.front().item == item1);
// 
//     FloatIterator fltIt = graph.getItem<Item<float>>(a, 0);
//     graph.removeItemFromFrame(a, fltIt);
//     BOOST_CHECK(sub.floatItemRemovedEvents.size() == 1);
//     BOOST_CHECK(sub.floatItemRemovedEvents.front().frame == a);
//     BOOST_CHECK(sub.floatItemRemovedEvents.front().item == item3);
//     
//     using Iterator = TransformGraph::ItemIterator<Item<int>>;
//     Iterator begin, end;
//     boost::tie(begin, end) = graph.getItems<Item<int>>(b);
//     graph.removeItemFromFrame(b, begin);
//     BOOST_CHECK(sub.intItemRemovedEvents.size() == 1);
//     BOOST_CHECK(sub.intItemRemovedEvents.front().frame == b);
//     BOOST_CHECK(sub.intItemRemovedEvents.front().item == item2);
// }
// 
// BOOST_AUTO_TEST_CASE(contains_item_test)
// {
//     TransformGraph graph;
//     FrameId a("a");
//     Item<string>::Ptr item(new Item<string>("For a moment, nothing happened. Then, after a second or so, nothing continued to happen."));
//     
//     BOOST_CHECK_THROW(graph.containsItems<Item<string>>(a), UnknownFrameException)
//     graph.addFrame(a);
//     graph.addItemToFrame(a, item);
//     BOOST_CHECK(graph.containsItems<Item<string>>(a));
//     BOOST_CHECK(!graph.containsItems<Item<int>>(a));
// }
// 
// BOOST_AUTO_TEST_CASE(get_transform_between_unconnected_trees)
// {
//     TransformGraph graph;
//     FrameId a("a");
//     FrameId b("b");
//     FrameId c("c");
//     FrameId d("d");
//     Transform tf;
// 
//     graph.addTransform(a, b, tf);
//     graph.addTransform(c, d, tf);
//     BOOST_CHECK_THROW(graph.getTransform(a, c), UnknownTransformException);
// }
// 
// BOOST_AUTO_TEST_CASE(get_transform_using_a_tree)
// {
// 
//     TransformGraph graph;
//     /*       a
//      *      / \
//      *     c   b
//      *   /  \
//      *  d   e
//      *    /  \
//      *   f   g
//      */
// 
//     FrameId a = "frame_a";
//     FrameId b = "frame_b";
//     FrameId c = "frame_c";
//     FrameId d = "frame_d";
//     FrameId e = "frame_e";
//     FrameId f = "frame_f";
//     FrameId g = "frame_g";
// 
//     Transform tf;
//     tf.transform.translation << 1, 2, 1;
//     tf.transform.orientation = base::AngleAxisd(0.25, base::Vector3d::UnitZ());
// 
//     graph.addTransform(a, b, tf);
//     graph.addTransform(a, c, tf);
//     graph.addTransform(c, d, tf);
//     graph.addTransform(c, e, tf);
//     graph.addTransform(e, f, tf);
//     graph.addTransform(e, g, tf);
// 
//     //use a as root
//     TreeView view = graph.getTree(graph.getVertex(a));
//     BOOST_CHECK(view.root == graph.getVertex(a));
// 
//     Transform tree_tf_a_f = graph.getTransform(a, f, view);
//     Transform graph_tf_a_f = graph.getTransform(a, f);
// 
//     BOOST_TEST_MESSAGE(tree_tf_a_f.transform);
//     BOOST_TEST_MESSAGE(graph_tf_a_f.transform);
// 
//     /** Compare transform cannot be used due to round-off errors **/
//     BOOST_CHECK(tree_tf_a_f.transform.translation.isApprox(graph_tf_a_f.transform.translation));
//     BOOST_CHECK(tree_tf_a_f.transform.orientation.isApprox(graph_tf_a_f.transform.orientation));
// 
//     Transform tree_tf_d_g = graph.getTransform(d, g, view);
//     Transform graph_tf_d_g = graph.getTransform(d, g);
// 
//     BOOST_TEST_MESSAGE(tree_tf_d_g.transform);
//     BOOST_TEST_MESSAGE(graph_tf_d_g.transform);
// 
//     /** Compare transform cannot be used due to round-off errors **/
//     BOOST_CHECK(tree_tf_d_g.transform.translation.isApprox(graph_tf_d_g.transform.translation));
//     BOOST_CHECK(tree_tf_d_g.transform.orientation.isApprox(graph_tf_d_g.transform.orientation));
// 
//     Transform tree_tf_f_g = graph.getTransform(f, g, view);
//     Transform graph_tf_f_g = graph.getTransform(f, g);
// 
//     BOOST_TEST_MESSAGE(tree_tf_f_g.transform);
//     BOOST_TEST_MESSAGE(graph_tf_f_g.transform);
// 
//     /** Compare transform cannot be used due to round-off errors **/
//     BOOST_CHECK(tree_tf_f_g.transform.translation.isMuchSmallerThan(0.01));
//     BOOST_CHECK(graph_tf_f_g.transform.translation.isApprox(Eigen::Vector3d::Zero()));
//     BOOST_CHECK(tree_tf_f_g.transform.orientation.isApprox(graph_tf_f_g.transform.orientation));
// 
//     /** Identity **/
//     Transform tree_tf_f_f = graph.getTransform(f, f, view);
//     BOOST_CHECK(tree_tf_f_f.transform.translation.isApprox(Eigen::Vector3d::Zero()));
//     BOOST_CHECK(tree_tf_f_f.transform.orientation.isApprox(Eigen::Quaterniond::Identity()));
// }
// 
// BOOST_AUTO_TEST_CASE(item_count_test)
// {
//     TransformGraph graph;
//     FrameId frame("frame");  
//     FrameId unknownFrame("unknown frame");  
//     graph.addFrame(frame);
//     vertex_descriptor vertex = graph.vertex(frame);
//     Item<int>::Ptr i(new Item<int>(42));
//     Item<int>::Ptr j(new Item<int>(21));
//     Item<int>::Ptr k(new Item<int>(11));
//     
//     BOOST_CHECK(graph.getItemCount<Item<int>>(frame) == 0);
//     BOOST_CHECK(graph.getItemCount<Item<int>>(vertex) == 0);
//     BOOST_CHECK_THROW(graph.getItemCount<Item<int>>(unknownFrame), UnknownFrameException);
//     
//     graph.addItemToFrame(frame, i);
//     BOOST_CHECK(graph.getItemCount<Item<int>>(frame) == 1);
//     BOOST_CHECK(graph.getItemCount<Item<int>>(vertex) == 1);
//     
//     graph.addItemToFrame(frame, j);
//     BOOST_CHECK(graph.getItemCount<Item<int>>(frame) == 2);
//     BOOST_CHECK(graph.getItemCount<Item<int>>(vertex) == 2);
//     
//     BOOST_CHECK(graph.getItemCount<Item<float>>(vertex) == 0);
//     
// }
// 
// BOOST_AUTO_TEST_CASE(get_path_test)
// {
//     TransformGraph graph;
//     FrameId A("A");
//     FrameId B("B");
//     FrameId C("C");
//     FrameId D("D");
//     FrameId E("E");
//     FrameId F("F");
//     FrameId G("G");
//     
//     Transform tf;
//     
//     graph.addTransform(A, B, tf);
//     graph.addTransform(B, C, tf);
//     graph.addTransform(B, F, tf);
//     graph.addTransform(A, E, tf);
//     graph.addTransform(E, C, tf);
//     graph.addTransform(C, D, tf);
//     graph.addTransform(D, G, tf);
//     graph.addTransform(C, F, tf);
//     
//     const vector<FrameId> path = graph.getPath(A, D);
//     
//     BOOST_CHECK(path[0] == A);
//     BOOST_CHECK(path[1] == B);
//     BOOST_CHECK(path[2] == C);
//     BOOST_CHECK(path[3] == D);
// }
// 
// BOOST_AUTO_TEST_CASE(get_path_invalid_frame_test)
// {
//     TransformGraph graph;
//     FrameId A("A");
//     FrameId B("B");
//     
//     graph.addFrame(A);
//     BOOST_CHECK_THROW(graph.getPath(A, B), UnknownFrameException);
//     BOOST_CHECK_THROW(graph.getPath(B, A), UnknownFrameException);
// }
// 
// BOOST_AUTO_TEST_CASE(get_empty_path_test)
// {
//     TransformGraph graph;
//     FrameId A("A");
//     FrameId B("B");
//     FrameId C("C");
//     Transform tf;
//     
//     graph.addTransform(A, B, tf);
//     graph.addFrame(C);
//     const vector<FrameId> path = graph.getPath(A, C);
//     BOOST_CHECK(path.size() == 0);
// }
// 
// 




// 




// 
// BOOST_AUTO_TEST_CASE(get_transform_with_descriptor_between_unconnected_frames_test)
// {
//     TransformGraph graph;
//     FrameId a("A");
//     FrameId b("B");
//     graph.addFrame(a);
//     graph.addFrame(b);
//     
//     vertex_descriptor aDesc = graph.getVertex(a);
//     vertex_descriptor bDesc = graph.getVertex(b);
//     
//     BOOST_CHECK_THROW(graph.getTransform(aDesc, bDesc), UnknownTransformException);
//     
// }
// 
// BOOST_AUTO_TEST_CASE(get_transform_from_edge_test)
// {
//     TransformGraph graph;
//     FrameId a("A");
//     FrameId b("B");
//     Transform tf;
//     tf.transform.translation << 42, 42, 42;
//     
//     graph.addTransform(a, b, tf);
//     edge_descriptor edge = graph.getEdge(a, b);
//     Transform tf2 = graph.getTransform(edge);
//     BOOST_CHECK(tf.transform.translation.x() == tf2.transform.translation.x());
//     BOOST_CHECK(tf.transform.translation.y() == tf2.transform.translation.y());
//     BOOST_CHECK(tf.transform.translation.z() == tf2.transform.translation.z());
// }
// 
// 
// 
// 
// 
