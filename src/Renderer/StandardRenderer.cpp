//
//  StandardRenderer.cpp
//  VI-RT-LPS
//
//  Created by Luis Paulo Santos on 14/03/2023.
//

#include "StandardRenderer.hpp"

void StandardRenderer::Render () {
    int W=0,H=0;  // resolution
    int x,y;

    // get resolution from the camera
    cam->getResolution(&W,&H);
    
    // jitter
    const bool jitter = false;

    // main rendering loop: get primary rays from the camera until done
    for (y=0 ; y< H ; y++) {  // loop over rows
        for (x=0 ; x< W ; x++) { // loop over columns
            Ray primary;
            Intersection isect;
            bool intersected,sucess_ray;
            RGB color;

            // multiple samples per pixel
            for (int ss = 0; ss < spp; ss++) {
                if (jitter) {
                    float jitterV[2];
                    jitterV[0] = ((float)rand()) / ((float)RAND_MAX);
                    jitterV[1] = ((float)rand()) / ((float)RAND_MAX);
                    
                    // Generate Ray (camera)
                    sucess_ray = cam->GenerateRay(x, y, &primary, jitterV);
                }
                else {
                    // Generate Ray (camera)
                    sucess_ray = cam->GenerateRay(x, y, &primary);
                }

                if (!sucess_ray) {
                    std::cout << "Couldn't generate the ray: " << x << " " << y << "\n";
                }

                // trace ray (scene)
                intersected = scene->trace(primary, &isect);       

                // shade this intersection (shader) - remember: depth=0
                RGB this_color = shd->shade(intersected, isect,0);

                color += this_color;
            }                     

            color = color / spp;

            // write the result into the image frame buffer (image)
            img->set(x,y,color);
        }
    }
}
