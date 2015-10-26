#pragma once
/** This file contains various helpers that are used for meta programming inside
 *  the envire core framework. */

namespace envire { namespace core
{
    /** Extracts the type of a template parameter.
     *  E.g.
     *    extract_value_type<boost::shared_ptr<std::string>>::value_type is std::string
     *    extract_value_type<std::string>:: value_type is std::string
     * */
    template<typename T>
    struct extract_value_type //base template
    {
        typedef T value_type;
    };
    
    template<template<typename> class X, typename T>
    struct extract_value_type <X<T>> //specialization for inner type
    {
        typedef T value_type;
    };
}}