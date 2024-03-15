//
//  Scene.cpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 30/01/2023.
//

#include "scene.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tinyobjloader/tiny_obj_loader.h"
#include "../Primitive/primitive.hpp"
#include "../Primitive/Geometry/mesh.hpp"
#include "../Primitive/BRDF/Phong.hpp"

#include <iostream>
#include <set>
#include <vector>

using namespace tinyobj;

static void PrintInfo (const ObjReader myObj) {
    const tinyobj::attrib_t attrib = myObj.GetAttrib();
    const std::vector<tinyobj::shape_t> shapes = myObj.GetShapes();
    const std::vector<tinyobj::material_t> materials = myObj.GetMaterials();
    std::cout << "# of vertices  : " << (attrib.vertices.size() / 3) << std::endl;
    std::cout << "# of normals   : " << (attrib.normals.size() / 3) << std::endl;
    std::cout << "# of texcoords : " << (attrib.texcoords.size() / 2)
              << std::endl;

    std::cout << "# of shapes    : " << shapes.size() << std::endl;
    std::cout << "# of materials : " << materials.size() << std::endl;
    
    // Iterate shapes
    auto it_shape = shapes.begin();
    for ( ; it_shape != shapes.end() ; it_shape++) {
        // assume each face has 3 vertices
        std::cout << "Processing shape " << it_shape->name << std::endl;
        // iterate faces
        // assume each face has 3 vertices
        auto it_vertex = it_shape->mesh.indices.begin();
        for ( ; it_vertex != it_shape->mesh.indices.end() ; ) {
            // process 3 vertices
            for (int v=0 ; v<3 ; v++) {
                std::cout << it_vertex->vertex_index;
                it_vertex++;
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
        
        printf("There are %lu material indexes\n", it_shape->mesh.material_ids.size());
    }
    
}

/*
 Use tiny load to load .obj scene descriptions
 https://github.com/tinyobjloader/tinyobjloader
 */

bool Scene::Load (const std::string &fname) {
    ObjReader myObjReader;

    if (!myObjReader.ParseFromFile(fname)) {
        if (!myObjReader.Error().empty()) { std::cerr << "TinyObjReader: " << myObjReader.Error(); }
        return false;
    }
    
    PrintInfo (myObjReader);

    // convert loader's representation to my representation
    attrib_t attribs = myObjReader.GetAttrib();                                     // vertices, normals, texCoords
    const std::vector<tinyobj::shape_t> shapes = myObjReader.GetShapes();           // primitives
    const std::vector<tinyobj::material_t> materials = myObjReader.GetMaterials();  // materials
    
    // Load Materials
    for (auto it_mat = materials.begin(); it_mat != materials.end(); it_mat++) {
        Phong* p = new Phong();

        p->Ka = RGB(it_mat->ambient[0], it_mat->ambient[1], it_mat->ambient[2]);
        p->Kd = RGB(it_mat->diffuse[0], it_mat->diffuse[1], it_mat->diffuse[2]);
        p->Ks = RGB(it_mat->specular[0], it_mat->specular[1], it_mat->specular[2]);
        p->Kt = RGB(it_mat->transmittance[0], it_mat->transmittance[1], it_mat->transmittance[2]);
        p->Ns = it_mat->shininess;
    }

    // Load Primitives
    for (auto it_shape = shapes.begin(); it_shape != shapes.end(); it_shape++) {
        // A shape has a name, a mesh, lines and points

        Mesh* mesh = new Mesh();
        Primitive* prim = new Primitive();

        // A Mesh is from the type Geometry
        prim->g = mesh;

        // assume all faces in the mesh have the same material
        prim->material_ndx = it_shape->mesh.material_ids[0];

        // Load vertices
        for (size_t i = 0; i < attribs.vertices.size(); i += 3) {
            tinyobj::real_t vx = attribs.vertices[i];
            tinyobj::real_t vy = attribs.vertices[i + 1];
            tinyobj::real_t vz = attribs.vertices[i + 2];
            mesh->vertices.push_back(Point(vx, vy, vz));
        }
        mesh->numVertices = mesh->vertices.size();


        // initially set BB.min and BB.max to be the first vertex
        const int V1st = it_shape->mesh.indices.begin()->vertex_index * 3;
        mesh->bb.min = mesh->vertices[V1st];
        mesh->bb.max = mesh->vertices[V1st];
        //....................................................................................
      
      
        // add faces and vertices
        //std::set<rehash> vert_rehash;

        for (auto v_it = it_shape->mesh.indices.begin(); v_it != it_shape->mesh.indices.end(); ) {
            Face* f = new Face;
            Point myVtcs[3];

            // process 3 vertices
            for (int v = 0; v < 3; v++) {
                const int objNdx = v_it->vertex_index;
                myVtcs[v].set(mesh->vertices[objNdx * 3].X, mesh->vertices[objNdx * 3].Y, mesh->vertices[objNdx * 3].Z);

                if (v == 0) {
                    f->bb.min.set(myVtcs[0].X, myVtcs[0].Y, myVtcs[0].Z);
                    f->bb.max.set(myVtcs[0].X, myVtcs[0].Y, myVtcs[0].Z);
                }
                else f->bb.update(myVtcs[v]);


                // add vertex to mesh if new
                /*rehash new_vert = {objNdx, 0};
                auto known_vert = vert_rehash.find(new_vert);

                if (known_vert == vert_rehash.end()) { // new vertice, add it to the mesh
                    new_vert.ourNdx = mesh->numVertices;
                    vert_rehash.insert(new_vert);
                    mesh->vertices.push_back(myVtcs[v]); 
                    mesh->numVertices++;

                    // register in the face
                    f->vert_ndx[v] = new_vert.ourNdx; 
                    mesh->bb.update(myVtcs[v]);
                }
                else f->vert_ndx[v] = known_vert->ourNdx;
                v_it++; // next vertice within this face (there are 3)*/
            }


            // add face to mesh: compute the geometric normal
            Vector v1 = myVtcs[0].vec2point(myVtcs[1]);
            Vector v2 = myVtcs[0].vec2point(myVtcs[2]);
            
            //f->edge1 = v1;                                     AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
            //f->edge2 = v2;
            
            Vector normal = v1.cross(v2);
            normal.normalize();
            f->geoNormal.set(normal);

            //f->FaceID = FaceID++;                              AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
            
            // add face to mesh
            mesh->faces.push_back(*f); 
            mesh->numFaces++;
        }

        // add primitive to scene
        prims.push_back(prim); 
        numPrimitives++;
    }

    return true;
}

bool Scene::trace (Ray r, Intersection *isect) {
    Intersection curr_isect;
    bool intersection = false;    
    
    if (numPrimitives==0) return false;
    
    // iterate over all primitives
    for (auto prim_itr = prims.begin() ; prim_itr != prims.end() ; prim_itr++) {
        if ((*prim_itr)->g->intersect(r, &curr_isect)) {
            if (!intersection) { // first intersection
                intersection = true;
                *isect = curr_isect;
                isect->f = BRDFs[(*prim_itr)->material_ndx];
            }
            else if (curr_isect.depth < isect->depth) {
                *isect = curr_isect;
                isect->f = BRDFs[(*prim_itr)->material_ndx];
            }
        }
    }
    return intersection;
}

// checks whether a point on a light source (distance maxL) is visible
bool Scene::visibility (Ray s, const float maxL) {
    bool visible = true;
    Intersection curr_isect;
    
    if (numPrimitives==0) return true;
    
    // iterate over all primitives while visible
    for (auto prim_itr = prims.begin() ; prim_itr != prims.end() && visible ; prim_itr++) {
        if ((*prim_itr)->g->intersect(s, &curr_isect)) {
            if (curr_isect.depth < maxL) {
                visible = false;
            }
        }
    }
    return visible;
}
