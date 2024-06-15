//
//  StandardRenderer.cpp
//  VI-RT-LPS
//
//  Created by Luis Paulo Santos on 14/03/2023.
//

#include "StandardRenderer.hpp"
#include <omp.h>
#include <random>

float floatRand(const float& min, const float& max) {
    static thread_local std::mt19937 generator;
    std::uniform_real_distribution<float> distribution(min, max);
    return distribution(generator);
}

void StandardRenderer::Render () {
    int W=0,H=0;  // resolution
    int x,y;

    // get resolution from the camera
    cam->getResolution(&W,&H);
    
    // jitter
    const bool jitter = false;

    // define number of threads
    int max_threads = omp_get_max_threads();
    std::cout << "\nNumber of available threads: " << max_threads << std::endl;

    // using 70% of the available threads
    int threads = static_cast<int>(max_threads * 0.7);
    std::cout << "Running with [" << threads << "] threads...\n" << std::endl;
    omp_set_num_threads(threads);

    // main rendering loop: get primary rays from the camera until done
    //#pragma omp parallel reduction(+ : color)
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

                    jitterV[0] = floatRand(0.0f,1.0f);  
                    jitterV[1] = floatRand(0.0f,1.0f);

                    std::cout << jitterV[0] << " " << jitterV[1] << "\n" << std::endl;

                    
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
