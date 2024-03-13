//
//  AABB.hpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 30/01/2023.
//

#ifndef BB_hpp
#define BB_hpp

#include "../utils/vector.hpp"
#include "../Rays/ray.hpp"

typedef struct BB {
public:
    Point min, max;
    void update (Point p) {
        if (p.X < min.X) min.X = p.X;
        else if (p.X > max.X) max.X = p.X;
        
        if (p.Y < min.Y) min.Y = p.Y;
        else if (p.Y > max.Y) max.Y = p.Y;
        
        if (p.Z < min.Z) min.Z = p.Z;
        else if (p.Z > max.Z) max.Z = p.Z;
    }

    /*
     * I suggest you implement:
     *  bool intersect (Ray r) { }
     *
     * based on PBRT's 3rd ed. book , sec 3.1.2, pags 125..128 + 214,217,221
     *
     * or https://doi.org/10.1007/978-1-4842-7185-8_2
     *
     */
    bool intersect(Ray r) {
        float t0 = 0;

        Point pMin, pMax;
        float txNear, txFar, tyNear, tyFar, tzNear, tzFar;

        txNear = (pMin.X - r.o.X) * r.invDir.X;
        txFar = (pMax.X - r.o.X) * r.invDir.X;
        
        tyNear = (pMin.Y - r.o.Y) * r.invDir.Y;
        tyFar = (pMax.Y - r.o.Y) * r.invDir.Y;

        tzNear = (pMin.Z - r.o.Z) * r.invDir.Z;
        tzFar = (pMax.Z - r.o.Z) * r.invDir.Z;


    }
} BB;

#endif /* AABB_hpp */
