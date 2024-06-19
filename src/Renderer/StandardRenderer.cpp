//
//  StandardRenderer.cpp
//  VI-RT-LPS
//
//  Created by Luis Paulo Santos on 14/03/2023.
//

#include "StandardRenderer.hpp"
#include "../Camera/perspective.hpp"
#include <omp.h>
#include <random>

float floatRand(const float& min, const float& max) {
    static thread_local std::mt19937 generator;
    std::uniform_real_distribution<float> distribution(min, max);
    return distribution(generator);
}

void StandardRenderer::Render () {
    int W=0,H=0;  // resolution

    // get resolution from the camera
    Perspective* perspCam = dynamic_cast<Perspective*>(cam);
    perspCam->getResolution(&W, &H);

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
    float sumR = 0.0f, sumG = 0.0f, sumB = 0.0f;
    #pragma omp parallel for default(none) reduction(+: sumR,sumG,sumB)
        for (int y = 0; y < H; y++) {     // loop over rows
            for (int x = 0; x < W; x++) { // loop over columns
                sumR = 0.0f;
                sumG = 0.0f;
                sumB = 0.0f;


                // multiple samples per pixel
                for (int ss = 0; ss < spp; ss++) {
                    Ray primary;
                    Intersection isect;

                    float jitterV[2];
                    if (jitter) {
                        jitterV[0] = floatRand(0.0f,1.0f);  
                        jitterV[1] = floatRand(0.0f,1.0f);
                    }

                    // Generate Ray (camera)
                    perspCam->GenerateRay(x, y, &primary, jitterV);
                    
                    // trace ray (scene)
                    bool intersected = scene->trace(primary, &isect);

                    // shade this intersection (shader) - remember: depth=0
                    RGB this_color = shd->shade(intersected, isect, 0);

                    //color += this_color

                    sumR += this_color.R;
                    sumG += this_color.G;
                    sumB += this_color.B;
                }

                // Average
                sumR = sumR / spp;
                sumG = sumG / spp;
                sumB = sumB / spp;

                // write the result into the image frame buffer (image)
                RGB color = { sumR,sumG,sumB};
                img->set(x, y, color);
            }
        }
    
}
