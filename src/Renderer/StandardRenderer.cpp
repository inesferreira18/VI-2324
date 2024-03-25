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
    
    // main rendering loop: get primary rays from the camera until done
    for (y=0 ; y< H ; y++) {  // loop over rows
        for (x=0 ; x< W ; x++) { // loop over columns
            Ray primary;
            Intersection isect;
            bool intersected,sucess_ray;
            RGB color;
          
            // Generate Ray (camera)
			sucess_ray = cam->GenerateRay(x,y,&primary,NULL);
            if (!sucess_ray) {
                std::cout << "Couldn't generate the ray: " << x << " " << y << "\n";
            }

            // trace ray (scene)
            intersected = scene->trace(primary, &isect);
            
            // shade this intersection (shader) - remember: depth=0
            color = shd->shade(intersected,isect,0);                          

            // write the result into the image frame buffer (image)
            img->set(x,y,color);
            
        }
    }
}
