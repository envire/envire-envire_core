#include "DummyItem.hpp"
#include <iostream>

using namespace std;

//envire::core::DummyClass::DummyClass(int number)
//{
//    cout << "Constructor of the Item with one argument" << endl;
//}

void envire::core::DummyClass::welcome()
{
    cout << "This is a method of the class to which the template fits" << endl;
}
//// Inheritance from a template fitting the DummyClass type
void envire::core::DummyItem::hello()
{
    cout << "You successfully created a class that inherited from a template. Hello!" << endl;
}
