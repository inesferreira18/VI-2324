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

bool StandardRenderer::start_window(SDL_Window** window, SDL_Renderer** renderer, int W, int H) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL Error: " << SDL_GetError() << "\n";
        return false;
    }

    *window = SDL_CreateWindow("VI Grupo 9 - Window Output", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, W, H, SDL_WINDOW_SHOWN);
    if (*window == nullptr) {
        std::cerr << "Window could not be created! SDL Error: " << SDL_GetError() << "\n";
        return false;
    }

    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
    if (*renderer == nullptr) {
        std::cerr << "Renderer could not be created! SDL Error: " << SDL_GetError() << "\n";
        SDL_DestroyWindow(*window);
        *window = nullptr;
        return false;
    }

    SDL_SetRenderDrawColor(*renderer, 0xFF, 0xFF, 0xFF, 0xFF);  // White background
    SDL_RenderClear(*renderer);

    return true;
}

void StandardRenderer::close_window(SDL_Window* window, SDL_Renderer* renderer) {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    window = nullptr;
    renderer = nullptr;

    SDL_Quit();
}

void StandardRenderer::updatePixel(SDL_Renderer* renderer, int x, int y, unsigned char R, unsigned char G, unsigned char B) {
    SDL_SetRenderDrawColor(renderer, R, G, B, 255);
    SDL_RenderDrawPoint(renderer, x, y);
    SDL_RenderPresent(renderer);
}

void StandardRenderer::updatePixel(SDL_Renderer* renderer, int x, int y, RGB color) {
    SDL_SetRenderDrawColor(renderer, (std::min(1.f, color.R) * 255), (std::min(1.f, color.G) * 255), (std::min(1.f, color.B) * 255), 255);
    SDL_RenderDrawPoint(renderer, x, y);
}



void StandardRenderer::Render (SDL_Renderer* renderer, bool LAUNCH_WINDOW) {
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

                if (LAUNCH_WINDOW) {
                    #pragma omp critical {
                    updatePixel(renderer, x, y, color);
                    }
                    SDL_RenderPresent(renderer);
                }
            }
        }
    
}
