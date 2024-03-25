//
//  ImagePPM.cpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 09/03/2023.
//

#include "ImagePPM.hpp"
#include <iostream>
#include <fstream>

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

// Details and code on PPM files available at:
// https://www.scratchapixel.com/lessons/digital-imaging/simple-image-manipulations/reading-writing-images.html
bool ImagePPM::Save(std::string filename) {

    // convert from float to {0,1,..., 255}
    ToneMap();

    // write imageToSave to file
    if (W == 0 || H == 0) {
        fprintf(stderr, "Can't save an empty image\n");
        return false;
    }

    std::ofstream file;
    file.open(filename, std::ios::binary);              // need to spec. binary mode for Windows users
    if (file.fail()) throw("Can't open output file");

    // Save Header 
    file << "P6\n"                     // P6 (stands for binary color image) 
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
