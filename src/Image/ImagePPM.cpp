//
//  ImagePPM.cpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 09/03/2023.
//

#include "ImagePPM.hpp"
#include <iostream>
#include <fstream>
#include "../utils/vector.hpp"
#include <algorithm>

void ImagePPM::ToneMap () {
    imageToSave = new PPM_pixel[W*H];
    
    // loop over each pixel in the image, clamp and convert to byte format
    for (int j = 0 ; j< H ; j++) {
        for (int i = 0; i < W ; ++i) {
            // Each pixel is encoded using 24 bits, i.e, 3 bytes
            imageToSave[j*W+i].val[0] = (unsigned char)(std::min(1.f, imagePlane[j*W+i].R) * 255);
            imageToSave[j*W+i].val[1] = (unsigned char)(std::min(1.f, imagePlane[j*W+i].G) * 255);
            imageToSave[j*W+i].val[2] = (unsigned char)(std::min(1.f, imagePlane[j*W+i].B) * 255);
        }
    }
}

// Reinhard's Extendend Luminance Tone maping
void ImagePPM::ToneMap_Reinhard() {
    imageToSave = new PPM_pixel[W * H];

    float L = 0.0f, max_L = 0.0f;
    for (int j = 0; j < H; j++) {
        for (int i = 0; i < W; ++i) {
            RGB color = imagePlane[j * W + i];

            // Find the maximum luminance (L = 0.2126*R + 0.7152*G + 0.0722*B)
            L = 0.2126 * color.R + 0.7152 * color.G + 0.0722 * color.B;
            max_L = std::max(max_L, L);
        }
    }

    float l_out = 0.0f, scale = 0.0f;
    for (int j = 0; j < H; j++) {
        for (int i = 0; i < W; ++i) {
            RGB color = imagePlane[j * W + i];

            L = 0.2126 * color.R + 0.7152 * color.G + 0.0722 * color.B;
            l_out = L * (1.0f + (L / (max_L * max_L))) / (1.0f + L);
            scale = l_out / L;

            imageToSave[j * W + i].val[0] = (unsigned char)(std::clamp(scale * color.R, 0.0f, 1.0f) * 255);
            imageToSave[j * W + i].val[1] = (unsigned char)(std::clamp(scale * color.G, 0.0f, 1.0f) * 255);
            imageToSave[j * W + i].val[2] = (unsigned char)(std::clamp(scale * color.B, 0.0f, 1.0f) * 255);
        }
    }
}


// This is the aproximated ACES method
RGB ImagePPM::AcesFilmicToneMapping(RGB color) {
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    RGB mapped;

    mapped.R = (color.R * (a * color.R + b)) / (color.R * (c * color.R + d) + e);
    mapped.G = (color.G * (a * color.G + b)) / (color.G * (c * color.G + d) + e);
    mapped.B = (color.B * (a * color.B + b)) / (color.B * (c * color.B + d) + e);

    // Clamp the output to [0,1]
    mapped.R = std::clamp(mapped.R, 0.0f, 1.f);
    mapped.G = std::clamp(mapped.G, 0.0f, 1.f);
    mapped.B = std::clamp(mapped.B, 0.0f, 1.f);

    return mapped;
}

void ImagePPM::ToneMap_ACES() {
    imageToSave = new PPM_pixel[W * H];

    for (int j = 0; j < H; j++) {
        for (int i = 0; i < W; ++i) {
            RGB color = imagePlane[j * W + i];
 
            // Apply tone mapping
            RGB mappedColor = AcesFilmicToneMapping(color);
            imageToSave[j * W + i].val[0] = (unsigned char)(mappedColor.R * 255);
            imageToSave[j * W + i].val[1] = (unsigned char)(mappedColor.G * 255);
            imageToSave[j * W + i].val[2] = (unsigned char)(mappedColor.B * 255);   
        }
    } 
}


// Details and code on PPM files available at:
// https://www.scratchapixel.com/lessons/digital-imaging/simple-image-manipulations/reading-writing-images.html
bool ImagePPM::Save(std::string filename) {

    // convert from float to {0,1,..., 255}
    //ToneMap();
    ToneMap_Reinhard();
    //ToneMap_ACES();

    // write imageToSave to file
    if (W == 0 || H == 0) {
        fprintf(stderr, "Can't save an empty image\n");
        return false;
    }

    std::ofstream file;
    file.open(filename, std::ios::binary);              // need to spec. binary mode for Windows users
    if (file.fail()) throw("Can't open output file");

    // Save Header 
    file << "P6\n"                    // P6 (stands for binary color image) 
        << W << " " << H << "\n"      // Resolution  
        << "255\n";                   // 255 (number of grey values between black and white)

    // Save Data
    unsigned char r, g, b;
    for (int i = 0; i < W * H; ++i) {
        r = imageToSave[i].val[0];
        g = imageToSave[i].val[1];
        b = imageToSave[i].val[2];

        file << r << g << b;
    }

    file.close();

    return true;
}
