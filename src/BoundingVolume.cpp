#include "BoundingVolume.hpp"

using namespace envire::core;

void envire::core::intrusive_ptr_add_ref( BoundingVolume* bv ) { bv->ref_count++; }
void envire::core::intrusive_ptr_release( BoundingVolume* bv ) { if(!--bv->ref_count) delete bv; }