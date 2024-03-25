//
//  mesh.cpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 05/02/2023.
//

#include "mesh.hpp"
#include <iostream>
// see pbrt book (3rd ed.), sec 3.6.2, pag 157
//
// Suggestion: use:
// // https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
// Moller Trumbore intersection algorithm

const float MAXFLOAT = std::numeric_limits<float>::max();


bool Mesh::TriangleIntersect (Ray r, Face f, Intersection *isect) {

    if (!f.bb.intersect(r)) {
        return false;
    }

    Point f1 = vertices[f.vert_ndx[0]];
    Point f2 = vertices[f.vert_ndx[1]];
    Point f3 = vertices[f.vert_ndx[2]];

    Vector h, s, q,edge1,edge2;

    float a, ff, u, v;

    edge1 = f1.vec2point(f2);
    edge2 = f1.vec2point(f3);
    h = r.dir.cross(edge2);
    a = edge1.dot(h);

    //const float par = f.geoNormal.dot(r.dir);
    if (a > (-EPSILON) && a < EPSILON) {
        return false;    // This ray is parallel to this triangle.
    }

    ff = 1.0 / a;

    s = f1.vec2point(r.o);
    u = ff * s.dot(h);
    if (u < 0.0 || u > 1.0) {
        return false;
    }

    q = s.cross(edge1);
    v = ff * r.dir.dot(q);
    if (v < 0.0 || u + v > 1.0) {
        return false;
    }

    // At this stage we can compute t to find out where the intersection point is on the line.
    float t = ff * edge2.dot(q);
    if (t > EPSILON) // ray intersection
    {
        Point pHit = r.o + t * r.dir;

        // Fill Intersection data from triangle hit : pag 165
        Vector wo = -1.f * r.dir;

        // make sure the normal points to the same side of the surface as wo
        Vector for_normal = f.geoNormal;
        for_normal.Faceforward(wo);

        isect->p = pHit;
        isect->gn = for_normal;
        isect->sn = for_normal;
        isect->wo = wo;
        isect->depth = t;
        isect->FaceID = -1;

        isect->isLight = false;

        return true;
    }
    else {// This means that there is a line intersection but not a ray intersection.
        return false;
    }
}

bool Mesh::intersect (Ray r, Intersection *isect) {
    bool intersect = false, intersect_this_face;
    Intersection min_isect, curr_isect;
    float min_depth = MAXFLOAT;

    // Intersect the ray with the mesh BB
    if (!bb.intersect(r)) return false;

    // If it intersects, then loop through the faces
    for (auto face_it = faces.begin(); face_it != faces.end(); face_it++) {
        intersect_this_face = TriangleIntersect(r, *face_it, &curr_isect);

        if (!intersect_this_face) continue;

        intersect = true;
        if (curr_isect.depth < min_depth) {  // this is closer
            min_depth = curr_isect.depth;
            min_isect = curr_isect;
        }
    }

    // If there was an intersection, update the intersection data
    if (intersect) {
        *isect = min_isect;
    }

    return intersect;
}

int Mesh::get_index(Point p) {
    int i = 0, ret = -1;

    while(i < numVertices){
        if (vertices[i].X == p.X && vertices[i].Y == p.Y && vertices[i].Z == p.Z){
            ret = i;
        }
        i++;
    }

    return ret;
}

