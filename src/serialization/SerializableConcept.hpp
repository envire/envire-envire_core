#pragma once
#include <sstream>
#include <boost/archive/polymorphic_binary_oarchive.hpp>
#include <boost/archive/polymorphic_binary_iarchive.hpp>


namespace envire { namespace core {
  
/**Checks if  T is serializable */
template <class T>
struct SerializableConcept
{
    BOOST_CONCEPT_USAGE(SerializableConcept)
    {
        std::stringstream ss("");

        boost::archive::polymorphic_binary_oarchive oa(ss);
        boost::archive::polymorphic_binary_iarchive ia(ss);

        T* t = NULL;

        oa << *t;
        ia >> *t;
    }
 };
 
}}