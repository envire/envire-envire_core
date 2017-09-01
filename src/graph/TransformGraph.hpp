/**\file TransformTree.hpp
 *
 * This class provided stores and manages the transformation graph
 *
 * @author Javier Hidalgo Carrio et. al (see THANKS  for the full Author list)
 * See LICENSE for the license information
 */
#pragma once

#include <cassert>
#include <string>

#include <envire_core/graph/Graph.hpp>
#include <envire_core/graph/GraphVisitors.hpp>
#include <envire_core/events/GraphEventPublisher.hpp>
#include <boost_serialization/BoostTypes.hpp>
#include <envire_core/items/Transform.hpp>



namespace envire { namespace core
{
    /**
     * FIXME comment
    */
    template <class FRAME_PROP>
    class TransformGraph :
        public Graph<FRAME_PROP, Transform>
    {
    public:
      using vertex_descriptor = GraphTraits::vertex_descriptor;
      using edge_descriptor = GraphTraits::edge_descriptor;
      using Base = Graph<FRAME_PROP, Transform>;
      using Base::num_edges;
      using Base::getFrameId;
      using Base::null_vertex;
      using Base::getVertex;
      using Base::setEdgeProperty;
      using Base::add_edge;
      using Base::remove_edge;
      using EdgePair = typename Base::EdgePair;
      

        /** @return the transform between a and b. Calculating it if necessary.
         * @throw UnknownTransformException if the transformation doesn't exist
         * @throw UnknownFrameException if the @p origin or @p target does not exist*/
        const Transform getTransform(const FrameId& origin, const FrameId& target) const;
        const Transform getTransform(const vertex_descriptor origin, const vertex_descriptor target) const;

         /** @return the transform between a and b. Calculating it if necessary.
         * @throw UnknownTransformException if the transformation doesn't exist
         * @throw UnknownFrameException if the @p origin or @p target does not exist*/
        const Transform getTransform(const FrameId& origin, const FrameId& target, const TreeView &view) const;
        const Transform getTransform(const vertex_descriptor origin, const vertex_descriptor target, const TreeView &view) const;
        /** @return the transform between source(edge) and target(edge) */
        const Transform getTransform(const edge_descriptor edge) const;
        
        /** @return the transform between path.front() and path.back().
         *          Returns Identity if path.size() <= 1.
         *  @throw UnknownTransformException if the edge between path[i] and path[i+1]
         *                                   does not exist.*/
        const Transform getTransform(const std::shared_ptr<Path> path) const;
        
        /**A convenience wrapper around Base::setEdgeProperty */
        void updateTransform(const vertex_descriptor origin, const vertex_descriptor target,
                             const Transform& tf);
        void updateTransform(const FrameId& origin, const FrameId& target, 
                             const Transform& tf);

        void updateTranform(const edge_descriptor edge, const Transform &tf);
        
        /**A convenience wrapper around Base::add_edge */
        void addTransform(const vertex_descriptor origin, const vertex_descriptor target,
                          const Transform& tf);
        void addTransform(const FrameId& origin, const FrameId& target,
                          const Transform& tf);
        
        /**A convenience wrapper around Base::remove_edge */
        void removeTransform(const vertex_descriptor origin, const vertex_descriptor target);
        void removeTransform(const FrameId& origin, const FrameId& target);
        
    protected:
      using Base::graph;
        
    private:
        /**Grants access to boost serialization */
        friend class boost::serialization::access;

        /**boost serialization method*/
        template <typename Archive>
        void serialize(Archive &ar, const unsigned int version);
    };
    
    template <class F>
    const Transform TransformGraph<F>::getTransform(const vertex_descriptor originVertex,
                                                    const vertex_descriptor targetVertex) const
    {
        if(num_edges() == 0)
        {
            throw UnknownTransformException(getFrameId(originVertex), getFrameId(targetVertex));
        }
        
        //direct edges
        EdgePair pair;
        pair = boost::edge(originVertex, targetVertex, *this);
        if(!pair.second)
        {            
            /** It is not a direct edge transformation **/
            Transform tf(base::Position::Zero(), base::Orientation::Identity()); //start with identity transform
            GraphBFSVisitor <vertex_descriptor>visit(targetVertex, this->graph());

            try
            {   
                Base::breadthFirstSearch(originVertex, boost::visitor(visit));

            }catch(const FoundFrameException &e)
            {
                base::TransformWithCovariance &trans(tf.transform);

                /** Compute the transformation **/
                std::deque<vertex_descriptor>::iterator it = visit.tree->begin();
                for (; (it+1) != visit.tree->end(); ++it)
                {
                    pair = boost::edge(*it, *(it+1), graph());
                    trans = trans * (*this)[pair.first].transform;
                }
                return tf;
            }
            //ending up here means, that the breadth_first_search could not find a path from origin to target
            throw UnknownTransformException(getFrameId(originVertex), getFrameId(targetVertex));
        }

        return (*this)[pair.first];
    }

    
  template <class F>
  const Transform TransformGraph<F>::getTransform(const FrameId& origin, const FrameId& target) const
  {
      const vertex_descriptor originVertex = getVertex(origin);
      const vertex_descriptor targetVertex = getVertex(target); 
      
      //originVertex and targetVertex may be null_vertex. That is intended!
      return getTransform(originVertex, targetVertex);
  }

  
    template <class F>
    const Transform TransformGraph<F>::getTransform(const vertex_descriptor originVertex,
                                                    const vertex_descriptor targetVertex,
                                                    const TreeView &view) const
    {
        if (originVertex == targetVertex)
        {
            /* An identity transformation **/
            return Transform(Eigen::Vector3d::Zero(), Eigen::Quaterniond::Identity());
        }

        base::TransformWithCovariance origin_tf = base::TransformWithCovariance::Identity(); // An identity transformation

        /** Get transformation from origin to the root **/
        vertex_descriptor od = originVertex;
        while(!view.isRoot(od))
        {
            EdgePair pair(boost::edge(od, view.getParent(od), *this));
            if (pair.second)
            {
                origin_tf = origin_tf * (*this)[pair.first].transform;
            }
            od = view.getParent(od);
        }

        base::TransformWithCovariance target_tf = base::TransformWithCovariance::Identity(); // An identity transformation

        /** Get transformation from target to the root **/
        vertex_descriptor td = targetVertex;
        while(!view.isRoot(td))
        {
            EdgePair pair;
            pair = boost::edge(td, view.getParent(td), *this);
            if (pair.second)
            {
                target_tf = target_tf * (*this)[pair.first].transform;
            }
            td = view.getParent(td);
        }

        return origin_tf * target_tf.inverse();
    }
    
    template <class F>
    const Transform TransformGraph<F>::getTransform(const std::shared_ptr<Path> path) const
    {
        if(path->getSize() <= 1)
        {
            return Transform(base::Position::Zero(), base::Orientation::Identity());
        }
        
        if(path->isDirty())
        {
          //NOTE this could be done in the Path but then the Path would need to know
          //     about the graph and its template parameters...
          path->setFrames(this->getFrames(path->getOrigin(), path->getTarget()));
          path->setDirty(false);
          if(path->isEmpty())
            throw InvalidPathException();
        }
        
        
        Transform tf = getTransform((*path)[0], (*path)[1]);
        base::TransformWithCovariance &trans(tf.transform);
        for(size_t i = 1; i < path->getSize() - 1; ++i)
        {
            //will throw if no path from path[i] to path[i + 1] exists
            const Transform stepTf = getTransform((*path)[i], (*path)[i + 1]);
            trans = trans * stepTf.transform;
        }
        return tf; 
    }

    template <class F>
    const Transform TransformGraph<F>::getTransform(const FrameId& origin, const FrameId& target, const TreeView &view) const
    {
        const vertex_descriptor originVertex = getVertex(origin);//will throw
        const vertex_descriptor targetVertex = getVertex(target); //will throw
        return getTransform(originVertex, targetVertex, view);
    }

    template <class F>
    const Transform TransformGraph<F>::getTransform(edge_descriptor edge) const
    {
        return (*this)[edge].transform;
    }

    template <class F>
    void TransformGraph<F>::updateTransform(const vertex_descriptor origin,
                                            const vertex_descriptor target,
                                            const Transform& tf)
    {
        setEdgeProperty(origin, target, tf);
    }
    
    template <class F>
    void TransformGraph<F>::updateTransform(const FrameId& origin, const FrameId& target, 
                                            const Transform& tf)
    {
        setEdgeProperty(origin, target, tf);
    }

    template <class F>
    void TransformGraph<F>::updateTranform(const edge_descriptor edge, const Transform &tf)
    {
        vertex_descriptor source_vertex = this->getSourceVertex(edge);
        vertex_descriptor target_vertex = this->getTargetVertex(edge);

        updateTranform(source_vertex, target_vertex, tf);
    }
    
    template <class F>
    void TransformGraph<F>::addTransform(const vertex_descriptor origin,
                                         const vertex_descriptor target,
                                         const Transform& tf)
    {
        add_edge(origin, target, tf);
    }
    template <class F>
    void TransformGraph<F>::addTransform(const FrameId& origin,
                                         const FrameId& target,
                                         const Transform& tf)
    {
        add_edge(origin, target, tf);
    }
    
    template <class F>
    void TransformGraph<F>::removeTransform(const vertex_descriptor origin, const vertex_descriptor target)
    {
        remove_edge(origin, target);
    }
    
    template <class F>
    void TransformGraph<F>::removeTransform(const FrameId& origin, const FrameId& target)
    {
        remove_edge(origin, target);
    }
    
    
    template<class F>
    template <typename Archive>
    void TransformGraph<F>::serialize(Archive &ar, const unsigned int version)
    {
        ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Base);
    }
}}
