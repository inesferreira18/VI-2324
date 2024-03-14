//
//  perspective.cpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 10/02/2023.
//

#include "perspective.hpp"

void Perspective::GenerateRay(const int x, const int y, Ray *r, const float *cam_jitter) {
    //To screen space
    float xs = (2 * (x + 0.5) / W) - 1;
    float ys = ((2 * (H - y - 1) + 0.5) / H) - 1;

    //To camera space
    float xc = xs * tan(fovW / 2);
    float yc = ys * tan(fovH / 2);

    float auxDir[3];
    for (int i =0;i<3;i++){
        float auxX = c2w[i][0] * xc;
        float auxY = c2w[i][1] * yc;
        float aux1 = c2w[i][2] * 1;

		auxDir[i] = auxX + auxY + aux1;
    }
    Vector auxVector = Vector(auxDir[0], auxDir[1], auxDir[2]);
    r->dir.set(auxVector);

	r->o.set(Eye.X, Eye.Y, Eye.Z);
}
