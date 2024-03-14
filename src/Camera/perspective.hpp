//
//  perspective.hpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 10/02/2023.
//

#ifndef perspective_hpp
#define perspective_hpp

#include "camera.hpp"
#include "../Rays/ray.hpp"


class Perspective: public Camera {
    Point Eye, At;
    Vector Up;
    float fovW, fovH;
    int W, H;
    float c2w[3][3];  // camera 2 world transform
public:
    Perspective (const Point Eye, const Point At, const Vector Up, const int W, const int H, const float fovW, const float fovH): Eye(Eye), At(At), Up(Up), W(W), H(H), fovW(fovW), fovH(fovH)  {
        // compute camera 2 world transform
        
        // -Forward Vector
        Vector F = Vector(At.X - Eye.X, At.Y - Eye.Y, At.Z - Eye.Z);
        F.normalize();

        // Right Vector
        Vector R = F.cross(Up);
        R.normalize();

        // Up normalized

        // Set the matrix c2w
        Vector aux;
        for (int i = 0;i<3; i++){
            if (i==0) {
                aux = R;
            } else {
                if(i==1){
                    aux = Up;
                }else{
                    aux = F;
                }
            }
            c2w[i][0] = aux.X;
            c2w[i][1] = aux.Y;
            c2w[i][2] = aux.Z;
        }
    }
    void GenerateRay(const int x, const int y, Ray *r, const float *cam_jitter=NULL);
    void getResolution (int *_W, int *_H) {*_W=W; *_H=H;}
};

#endif /* perspective_hpp */
