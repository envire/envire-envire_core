/**
 * \file RandomGenerator.hpp
 * \author Arne Boeckmann
 */

#ifndef RANDOMGENERATOR_HPP
#define RANDOMGENERATOR_HPP

#include <boost/thread/tss.hpp>
#include <boost/uuid/random_generator.hpp>

namespace envire { namespace core
{
    /**A singleton that provides access to a thread local random_generator
     * for uuid creation. */
    class RandomGenerator 
    {
    public: 
        /**Returns a reference to a thread local random_generator that can be
         * used for uuid creation */
        static boost::uuids::random_generator& getRandomGenerator()
        {
            //boost::uuids::random_generator is not thread safe. I.e. calling it
            //from multiple threads may cause duplicate ids. Therefore thread_specific_ptr
            //is used.
            static boost::thread_specific_ptr<boost::uuids::random_generator> randGen;
            if(!randGen.get()) {
                randGen.reset(new boost::uuids::random_generator());
            }
            return *randGen;
        }
    };
}}
#endif	/* RANDOMGENERATOR_HPP */

