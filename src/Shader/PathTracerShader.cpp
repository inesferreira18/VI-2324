//
//  AmbientShader.cpp
//  VI-RT-LPS
//
//  Created by Luis Paulo Santos on 14/03/2023.
//

#include "PathTracerShader.hpp"
#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <random>
#include <__msvc_chrono.hpp>

//#include "DEB.h"
float floatRand() {
    static thread_local std::mt19937 generator;
    std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
    return distribution(generator);
}


RGB PathTracerShader::directLighting (Intersection isect, Phong *f){
    
    RGB color(0.,0.,0.);
    Light *l;
    int l_ndx;
    const bool RANDOM_SAMPLE_ONE=true;
    float light_pdf;

    for (auto l_iter=scene->lights.begin() ; l_iter != scene->lights.end() ; l_iter++) {
        RGB this_l_color (0.,0.,0.);
        l = (Light *) (*l_iter);
        
        // if random sampling reaasign l
        if (RANDOM_SAMPLE_ONE) {
            // randomly select one light source
            l_ndx = rand() % scene->numLights;
            l = scene->lights[l_ndx];
            light_pdf = 1.f/((float)scene->numLights);
        }
        
        if (l->type == AMBIENT_LIGHT) {  // is it an ambient light ?
            if (!f->Ka.isZero()) {
                RGB Ka = f->Ka;
                this_l_color = Ka * l->L();
            }
        }
        if (l->type == POINT_LIGHT) {  // is it a point light ?
            if (!f->Kd.isZero()) {
                RGB L, Kd = f->Kd;
                Point lpoint;
                
                // get the position and radiance of the light source
                L = l->Sample_L(NULL, &lpoint);
                
                // compute the direction from the intersection point to the light source
                Vector Ldir = isect.p.vec2point(lpoint);
                const float Ldistance = Ldir.norm();
                
                // now normalize Ldir
                Ldir.normalize();
                
                // compute the cosine between Ldir  and the shading normal at the intersection point
                float cosL = Ldir.dot(isect.sn);
                
                // shade
                if (cosL>0.)  { // the light is NOT behind the porimitive
                    // generate the shadow ray
                    Ray shadow(isect.p, Ldir);
                    
                    shadow.pix_x = isect.pix_x;
                    shadow.pix_y = isect.pix_y;
                    
                    shadow.FaceID = isect.FaceID;
                    
                    // adjust origin by an EPSILON along the normal to avoid self occlusion at the origin
                    shadow.adjustOrigin(isect.gn);
                    
                    if (scene->visibility(shadow, Ldistance-EPSILON)) {  // if light source not occluded
                        this_l_color = Kd * L * cosL;
                    }
                } // end cosL > 0.
            }
        }
        if (l->type == AREA_LIGHT) {  // is it an area light ?
            if (!f->Kd.isZero()) {
                RGB L, Kd = f->Kd;
                Point lpoint;
                float l_pdf;
                AreaLight *al = (AreaLight *)l;
                
                // get the position and radiance of the light source
                // get 2 random number in [0,1[
                float rnd[2] = { floatRand(), floatRand()};
                L = al->Sample_L(rnd, &lpoint, l_pdf);
                
                // compute the direction from the intersection point to the light source
                Vector Ldir = isect.p.vec2point(lpoint);
                const float Ldistance = Ldir.norm();
                
                // now normalize Ldir
                Ldir.normalize();
                
                // compute the cosine between Ldir  and the shading normal at the intersection point
                float cosL = Ldir.dot(isect.sn);
                
                // compute the cosine between Ldir  and the area light source normal
                float cosL_LA = Ldir.dot(al->gem->normal);
                
                // shade
                if (cosL>0. && cosL_LA<=0.)  { // light is NOT behind primitive AND light normal points to the ray o
                    // generate the shadow ray
                    Ray shadow(isect.p, Ldir);
                    
                    shadow.pix_x = isect.pix_x;
                    shadow.pix_y = isect.pix_y;
                    
                    shadow.FaceID = isect.FaceID;
                    
                    // adjust origin by an EPSILON along the normal to avoid self occlusion at the origin
                    shadow.adjustOrigin(isect.gn);
                    
                    if (scene->visibility(shadow, Ldistance-EPSILON)) {  // if light source not occluded
                        this_l_color += (Kd * L * cosL) / l_pdf;
                    }
                } // end cosL > 0.
            }
        }  // end area light
        
        // if random sampling adjust the contribution and break the for loop
        if (RANDOM_SAMPLE_ONE) {
            color = this_l_color / light_pdf;
            break;
        } else {   // not random sampling, sum the individual contributions
            color += this_l_color;
        }
    } // for loop
    return color;
}

RGB PathTracerShader::specularReflection (Intersection isect, Phong *f, int depth) {
    RGB color(0.,0.,0.);
    Vector Rdir, s_dir;
    float pdf;
    Intersection s_isect;

    
    // generate the specular ray
    
    
    // IDEAL SPECULAR REFLECTION
    // direction R = 2 (N.V) N - V
    float cos = isect.gn.dot(isect.wo);
    Rdir = 2.f * cos * isect.gn - isect.wo;
    

    if (f->Ns < 1000) { // glossy materials
        // actual direction distributed around Rdir according to the cosine lobe
        // generate the cosine lobel sampled direction around (0,0,1)
        // following item (36) of the Global illumination compendium
        // get 2 random number in [0,1[
        float rnd[2] = { floatRand(), floatRand() };

        Vector S_around_N;
        //  generate s_dir
        // ...
        
        Ray specular(isect.p, Rdir);
        
        specular.pix_x = isect.pix_x;
        specular.pix_y = isect.pix_y;
        
        specular.FaceID = isect.FaceID;

        specular.adjustOrigin(isect.gn);

        // OK, we have the ray : trace and shade it recursively
        bool intersected;
        Intersection s_isect;
        // trace ray
        intersected = scene->trace(specular, &s_isect);

        // shade this intersection
        RGB Rcolor = shade (intersected, s_isect, depth+1);
        
        // evaluate this ray contribution, i.e., color
        // ...

        return color;

    }
    else {          // ideal specular reflection
        Ray specular(isect.p, Rdir);
        
        specular.pix_x = isect.pix_x;
        specular.pix_y = isect.pix_y;
        
        specular.FaceID = isect.FaceID;

        specular.adjustOrigin(isect.gn);

        // OK, we have the ray : trace and shade it recursively
        bool intersected;
        Intersection s_isect;
        // trace ray
        intersected = scene->trace(specular, &s_isect);

        // shade this intersection
        RGB Rcolor = shade (intersected, s_isect, depth+1);
        
        color = (f->Ks  * Rcolor)  ;
        return color;
    }
    
}

RGB PathTracerShader::diffuseReflection (Intersection isect, Phong *f, int depth) {
    RGB color(0.,0.,0.);
    Vector dir;
    float pdf;
    
    // generate the specular ray
    
    // actual direction distributed around N
    // get 2 random number in [0,1[
    float rnd[2] = { floatRand(), floatRand() };

    // cosine sampling
    Vector D_around_Z;
    float cos_theta= D_around_Z.Z = sqrtf(rnd[1]); // cos sampling
    D_around_Z.Y = sinf(2.*M_PI*rnd[0])*sqrtf(1.-rnd[1]);
    D_around_Z.X = cosf(2.*M_PI*rnd[0])*sqrtf(1.-rnd[1]);
    pdf = cos_theta / ( M_PI );
        
    // generate a coordinate system from N
    Vector Rx, Ry;
    isect.gn.CoordinateSystem(&Rx, &Ry);
        
    dir = D_around_Z.Rotate  (Rx, Ry, isect.gn);
        
    Ray diffuse(isect.p, dir);
        
    diffuse.pix_x = isect.pix_x;
    diffuse.pix_y = isect.pix_y;
        
    diffuse.FaceID = isect.FaceID;

    diffuse.adjustOrigin(isect.gn);

    // OK, we have the ray : trace and shade it recursively
    bool intersected;
    Intersection d_isect;
    // trace ray
    intersected = scene->trace(diffuse, &d_isect);

    if (!d_isect.isLight) {  // if light source return 0 ; handled by direct
        // shade this intersection
        RGB Rcolor = shade (intersected, d_isect, depth+1);
        color = (f->Kd * cos_theta * Rcolor) / pdf ;
    }
    return color;

}


RGB PathTracerShader::shade(bool intersected, Intersection isect, int depth) {
    RGB color(0.,0.,0.);
    
    // if no intersection, return background
    if (!intersected) {
        return (background);
    }
    
    if (isect.isLight) { // intersection with a light source
        return isect.Le;
    }
    
    // get the BRDF
    Phong *f = (Phong *)isect.f;

    if (depth <MAX_DEPTH) {
        RGB lcolor;
        // random select between specular and diffuse
        float s_p = f->Ks.Y() /(f->Ks.Y()+f->Kd.Y());
        float rnd = floatRand();
        if (rnd <= s_p || s_p >= (1.-EPSILON)) // do specular
            lcolor = specularReflection (isect, f, depth) / s_p;
        else // do diffuse
            lcolor = diffuseReflection (isect, f, depth) / (1.-s_p);
        color += lcolor;
    }

    // if there is a diffuse component do direct light
    if (!f->Kd.isZero()) {
        color += directLighting(isect, f);
    }
        
    return color;
};
