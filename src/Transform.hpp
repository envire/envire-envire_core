#ifndef __ENVIRE_CORE_TRANSFORM__
#define __ENVIRE_CORE_TRANSFORM__

#include <vector>
#include <string>

#include <base/TransformWithCovariance.hpp>
#include <base/Time.hpp>

namespace envire { namespace core
{
    class Transform
    {
    public:
        base::Time time; /** Timestamp */
        base::TransformWithCovariance transform; /** the transformation */

    public:
        Transform();

        void setTransform(const base::TransformWithCovariance& tf);


    };

    /** Transform Graph Viz Property writer for boost graphs **/
    template <class _Time, class _Transform>
    class TransformWriter
    {
    public:
        TransformWriter(_Time time, _Transform tf) : time(time), tf(tf){}
        template <class _Edge>
        void operator()(std::ostream &out, const _Edge& e) const
        {
            out << "[label=\"" << time[e].toString(base::Time::Resolution::Seconds) <<
                boost::format("\\nt: (%.1f %.1f %.1f)\\nr: (%.1f %.1f %.1f)") % tf[e].translation.x() % tf[e].translation.y() % tf[e].translation.z()
                % tf[e].orientation.axis()[0] % tf[e].orientation.axis()[1] % tf[e].orientation.axis()[2]
                << "\""
                << ",shape=ellipse,color=red,style=filled,fillcolor=lightcoral]";
        }
    private:
        _Time time;
        _Transform tf;
    };
}}
#endif
