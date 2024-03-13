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
    //minimum and maximum points of the box;
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
        float txNear, txFar, tyNear, tyFar, tzNear, tzFar;
        float aux;

        txNear = (min.X - r.o.X) * r.invDir.X;
        txFar = (max.X - r.o.X) * r.invDir.X;
        if (txNear > txFar) { // The ray is in the oposite direction
            aux = txNear;
            txNear = txFar;
            txFar = aux;
        }

        tyNear = (min.Y - r.o.Y) * r.invDir.Y;
        tyFar = (max.Y - r.o.Y) * r.invDir.Y;
        if (tyNear > tyFar) {
            aux = tyNear;
            tyNear = tyFar;
            tyFar = aux;
        }


        tzNear = (min.Z - r.o.Z) * r.invDir.Z;
        tzFar = (max.Z - r.o.Z) * r.invDir.Z;
        if (tzNear > tzFar) {
            aux = tzNear;
            tzNear = tzFar;
            tzFar = aux;
        }

        
        float tBoxMin = std::max(txNear, tyNear, tzNear);
        float tBoxMax = std::min(txFar, tyFar, tzFar);
        

        return tBoxMin <= tBoxMax;
    }
} BB;

#endif /* AABB_hpp */
