//
//  main.cpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 30/01/2023.
//

#define SDL_MAIN_HANDLED

#include "SDL.h"

#include <iostream>
#include "Scene/scene.hpp"
#include "Camera/perspective.hpp"
#include "Renderer/StandardRenderer.hpp"
#include "Image/ImagePPM.hpp"
#include "Shader/AmbientShader.hpp"
#include "Shader/WhittedShader.hpp"
#include "Shader/DistributedShader.hpp"
#include "Shader/PathTracerShader.hpp"
#include "Light/AmbientLight.hpp"
#include "Light/PointLight.hpp"
#include "Light/AreaLight.hpp"

#include <time.h>
#include <filesystem>

namespace fs = std::filesystem;


int main(int argc, char * argv[]) {
    Scene scene;
    Perspective *cam; // Camera
    ImagePPM *img;    // Image
    Shader *shd;
    bool success;
    clock_t start, end;
    double cpu_time_used;

    bool LAUNCH_WINDOW = true;

    fs::path currentPath = fs::current_path();
    fs::path path = currentPath /".." / "src" / "Scene" / "tinyobjloader" / "models" / "cornell_box_VI.obj";
    std::string pathStr = path.string();
    success = scene.Load(pathStr);

    if (!success) {
        std::cout << "ERROR!! :o\n";
        return 1;
    }
    std::cout << "Scene Load: SUCCESS!! :-)\n";
    
    
    // add an ambient light to the scene
    AmbientLight ambient(RGB(0.05,0.05,0.05));
    scene.lights.push_back(&ambient);
    scene.numLights++;
    
    // add a point light to the scene
    /*PointLight point(RGB(0.9, 0.9, 0.9), Point(288,508,282));
    scene.lights.push_back(&point);
    scene.numLights++;
    
	PointLight point1(RGB(0.4, 0.4, 0.4), Point(288-50, 508, 282-50));
	scene.lights.push_back(&point1);
	scene.numLights++;

	PointLight point2(RGB(0.4, 0.4, 0.4), Point(288 + 50, 508, 282 - 50));
	scene.lights.push_back(&point2);
	scene.numLights++;

	PointLight point3(RGB(0.4, 0.4, 0.4), Point(288 - 50, 508, 282 + 50));
	scene.lights.push_back(&point3);
	scene.numLights++;

	PointLight point4(RGB(0.4, 0.4, 0.4), Point(288 + 50, 508, 282 + 50));
	scene.lights.push_back(&point4);
	scene.numLights++;*/

    // add an area light to the scene (548 is the ceiling)
    Point v1 = {343, 548, 227};
    Point v2 = {343, 548, 332};
    Point v3 = {213, 548, 332};
    Point v4 = {213, 548, 227};
    Vector n = {0, -1, 0};
    RGB power = {1.0, 1.0, 1.0};

    AreaLight* al1 = new AreaLight(power, v1, v2, v3, n);
    scene.lights.push_back(al1);
    scene.numLights++;

    AreaLight* al2 = new AreaLight(power, v1, v3, v4, n);
    scene.lights.push_back(al2);
    scene.numLights++;

    // Image resolution
    const int W= 720;
    const int H= 720;
    
    img = new ImagePPM(W,H);
    
    // Camera parameters
    const Point Eye = { 280,275,-330 };
    const Point At={280,265,0};
    const Vector Up={0,1,0};
    const float fovW = 90.f;
    const float fovH = fovW * (float)H/(float)W;  // in degrees
    const float fovWrad = fovW*3.14f/180.f, fovHrad = fovH*3.14f/180.f;    // to radians
    cam = new Perspective(Eye, At, Up, W, H, fovWrad, fovHrad);
    
    // create the shader
    RGB background(0.05, 0.05, 0.55);
    //shd = new WhittedShader(&scene, background);
    //shd = new DistributedShader(&scene, background);
    shd = new PathTracerShader(&scene, background);

    // declare the renderer
    // samples per pixel
    int spp=2048;     
    StandardRenderer myRender (cam, &scene, img, shd, spp);

    SDL_Window* window;
    SDL_Renderer* renderer = nullptr;

    if(LAUNCH_WINDOW)
        if (!myRender.start_window(&window, &renderer, W, H)) {
            std::cerr << "Failed to start window!\n";
            return -1;
        }

    // render
    start = clock();
    myRender.Render(renderer, LAUNCH_WINDOW);
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

    // save the image
    img->Save("../src/MyImage_Rein.ppm");
    
    fprintf (stdout, "Rendering time = %.3lf secs\n\n", cpu_time_used);
    
    scene.printSummary();
    std::cout << std::endl;

    if (LAUNCH_WINDOW) {
        std::cout << "Updating window...\n";
        for (int i = 0; i < H; i++) {
            for (int j = 0; j < W; ++j) {
                myRender.updatePixel(renderer, j, i, img->getImage()[i * W + j].val[0], img->getImage()[i * W + j].val[1], img->getImage()[i * W + j].val[2]);
                SDL_RenderPresent(renderer);
            }
        }
    }


    std::cout << "Press Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::cout << "That's all, folks!" << std::endl;
    return 0;
}

