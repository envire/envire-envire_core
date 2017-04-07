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

