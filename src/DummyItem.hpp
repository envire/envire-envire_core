/*
 *  Copyright 2015, DFKI GmbH Robotics Innovation Center
 *
 *  This file is part of the MARS simulation framework.
 *
 *  MARS is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License
 *  as published by the Free Software Foundation, either version 3
 *  of the License, or (at your option) any later version.
 *
 *  MARS is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public License
 *   along with MARS.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/**
 * \file ItemPhysics.h
 * \author Raul Dominguez 
 * \brief "ItemPhysics" implements the physical ode stuff for the items that
 * are stored in the EnvireMarsTree.
 *
 *
 * ToDo:
 *
 *      - First task: Test using a simple item that is a flat surface for
 *      the ground. The item is part of a EnvireMarsTree and is loaded with
 *      its particular plugin. It will interact with the Wordlphysics class
 *      and will be visible using the Envire Visualization tools, but not
 *      those from Mars.
 *
 */

#ifndef ITEM_PHYSICS_H
#define ITEM_PHYSICS_H

#ifdef _PRINT_HEADER_
  #warning "ItemPhysics.h"
#endif

//#include "WorldPhysics.h"
//#include "envire_core/TransformTree.hpp"
#include "envire_core/Item.hpp"

using namespace envire::core;

namespace envire{
  namespace core{
    class DummyClass 
    {
      public: 
        int id;
        std::vector<double> myVector;
        DummyClass();
        DummyClass(const int& number);
        DummyClass(const int& number, const std::vector<double>& vector);
        void welcome();
        int getId();
        std::vector<double> getMyVector();
    };
    // Inheritance from a template fitting the DummyClass type
    class DummyItem : public Item<DummyClass> {
      //protected:
      //  envire::core::DummyClass user_data;
      public: 
        template <typename... Ts>                                                      
          DummyItem(Ts&&... args) : Item<DummyClass> (std::forward<Ts>(args)...){}
        // Overriding the constructor like this is not possible.
        //DummyItem() : Item() {
        //  user_data = DummyClass(1);
        //}
        void hello();
    };
    // Template specialization: methods of the template class get lost e.g. setData.
    //template <>
    //class Item <envire::core::DummyClass>{
    //  public: 
    //    /**
    //     * Print a welcome to stdout
    //     * \return nothing
    //     */
    //    void hello(); 
    //};
  } // end namespace envire-core
}

#endif  // ITEM_PHYSICS_H
