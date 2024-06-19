 //
//  StandardRenderer.hpp
//  VI-RT-LPS
//
//  Created by Luis Paulo Santos on 14/03/2023.
//

#ifndef StandardRenderer_hpp
#define StandardRenderer_hpp

#include "renderer.hpp"
#define SDL_MAIN_HANDLED

#include "SDL.h"

class StandardRenderer: public Renderer {
private:
    int spp;
public:
    StandardRenderer (Camera *cam, Scene * scene, Image * img, Shader *shd, int _spp): Renderer(cam, scene, img, shd) {
        spp = _spp;
    }
    void Render (SDL_Renderer* renderer, bool LAUNCH_WINDOW);
    bool start_window(SDL_Window** window, SDL_Renderer** renderer, int W, int H);
    void close_window(SDL_Window* window, SDL_Renderer* renderer);
    void updatePixel(SDL_Renderer* renderer, int x, int y, unsigned char R, unsigned char G, unsigned char B);
    void updatePixel(SDL_Renderer* renderer, int x, int y, RGB color);
};

#endif /* StandardRenderer_hpp */
