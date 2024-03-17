//
//  geometry.hpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 30/01/2023.
//

#ifndef geometry_hpp
#define geometry_hpp

#include "../BB.hpp"
#include "../../Rays/ray.hpp"
#include "../../Rays/intersection.hpp"

class Geometry {
public:
    Geometry () {}
    ~Geometry () {}
    // return True if r intersects this geometric primitive
    // returns data about intersection on isect
    virtual bool intersect (Ray r, Intersection *isect) { 
        return false; 
    }
    BB bb;  // geometric primitive bounding box
                // this is min={0.,0.,0.} , max={0.,0.,0.} due to the Point constructor
};

#endif /* geometry_hpp */
