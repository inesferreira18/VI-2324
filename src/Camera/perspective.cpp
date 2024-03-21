//
//  perspective.cpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 10/02/2023.
//

#include "perspective.hpp"

#include <iostream>

Perspective::Perspective(const Point Eye, const Point At, const Vector Up, const int W, const int H, const float fovW, const float fovH) : Eye(Eye), At(At), Up(Up), W(W), H(H), fovW(fovW), fovH(fovH) {
    // -Forward Vector
    Vector F = Vector(At.X - Eye.X, At.Y - Eye.Y, At.Z - Eye.Z);

    F.normalize();

    // Right Vector
    Vector R = F.cross(Up);
    R.normalize();

    // Up normalized

    // Set the matrix c2w
    Vector aux;
    for (int i = 0; i < 3; i++) {
        if (i == 0) {
            aux = R;
        }
        else {
            if (i == 1) {
                aux = Up;
            }
            else {
                aux = F;
            }
        }

        c2w[i][0] = aux.X;
        c2w[i][1] = aux.Y;
        c2w[i][2] = aux.Z;
    }
}

bool Perspective::GenerateRay(const int x, const int y, Ray *r, const float *cam_jitter) {
    //To screen space
    float xs = (2.0f * (x + 0.5f) / W) - 1.0f;
    float ys = (2.0f * ((H - y - 1.0f) + 0.5f) / H) - 1.0f;

    //To camera space
    float xc = xs * tan(fovW / 2.0f);
    float yc = ys * tan(fovH / 2.0f);

    float auxDir[3];
    for (int i =0;i<3;i++){
        float auxX = c2w[i][0] * xc;
        float auxY = c2w[i][1] * yc;
        float aux1 = c2w[i][2] * 1.0f;

		auxDir[i] = auxX + auxY + aux1;
    }
    Vector auxVector = Vector(auxDir[0], auxDir[1], auxDir[2]);
    r->dir.set(auxVector);

	r->o.set(Eye.X, Eye.Y, Eye.Z);

    return true;
}

void Perspective::getResolution(int* _W, int* _H) {
    *_W = W;
    *_H = H;
}