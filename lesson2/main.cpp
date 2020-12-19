#include <vector>
#include <iostream>
#include "tgaimage.h"
#include "geometry.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const TGAColor green = TGAColor(0,   255, 0,   255);
const int width  = 200;
const int height = 200;
// old school method: line sweeping
// void line(Vec2i p0, Vec2i p1, TGAImage &image, TGAColor color) {
//     bool steep = false;
//     if (std::abs(p0.x-p1.x)<std::abs(p0.y-p1.y)) {
//         std::swap(p0.x, p0.y);
//         std::swap(p1.x, p1.y);
//         steep = true;
//     }
//     if (p0.x>p1.x) {
//         std::swap(p0, p1);
//     }

//     for (int x=p0.x; x<=p1.x; x++) {
//         float t = (x-p0.x)/(float)(p1.x-p0.x);
//         int y = p0.y*(1.-t) + p1.y*t;
//         if (steep) {
//             image.set(y, x, color);
//         } else {
//             image.set(x, y, color);
//         }
//     }
// }
// void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color) {
//     line(t0, t1, image, red);
//     line(t1, t2, image, white);
//     line(t2, t0, image, green);
// }

// 逐行扫描确定三角形范围绘制三角形
// void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color) { 
//     if (t0.y==t1.y && t0.y==t2.y) return; // I dont care about degenerate triangles 
//     // sort the vertices, t0, t1, t2 lower−to−upper (bubblesort yay!) 
//     if (t0.y>t1.y) std::swap(t0, t1); 
//     if (t0.y>t2.y) std::swap(t0, t2); 
//     if (t1.y>t2.y) std::swap(t1, t2); 
//     int total_height = t2.y-t0.y; 
//     for (int i=0; i<total_height; i++) { 
//         bool second_half = i>t1.y-t0.y || t1.y==t0.y; 
//         int segment_height = second_half ? t2.y-t1.y : t1.y-t0.y; 
//         float alpha = (float)i/total_height; 
//         float beta  = (float)(i-(second_half ? t1.y-t0.y : 0))/segment_height; // be careful: with above conditions no division by zero here 
//         Vec2i A =               t0 + (t2-t0)*alpha; 
//         Vec2i B = second_half ? t1 + (t2-t1)*beta : t0 + (t1-t0)*beta; 
//         if (A.x>B.x) std::swap(A, B); 
//         for (int j=A.x; j<=B.x; j++) { 
//             image.set(j, t0.y+i, color); // attention, due to int casts t0.y+i != A.y 
//         } 
//     } 
// }

/**
 * 向量方式进行计算
 * 通过判断点是否在三角形内来进行绘制
 */
 
Vec3f barycentric(Vec2i *pts, Vec2i P) { 
    Vec3f u = cross(Vec3f(pts[2][0]-pts[0][0], pts[1][0]-pts[0][0], pts[0][0]-P[0]), Vec3f(pts[2][1]-pts[0][1], pts[1][1]-pts[0][1], pts[0][1]-P[1]));
    /* `pts` and `P` has integer value as coordinates
       so `abs(u[2])` < 1 means `u[2]` is 0, that means
       triangle is degenerate, in this case return something with negative coordinates */
    if (std::abs(u[2])<1) return Vec3f(-1,1,1);
    return Vec3f(1.f-(u.x+u.y)/u.z, u.y/u.z, u.x/u.z); 
} 
 
void triangle(Vec2i *pts, TGAImage &image, TGAColor color) { 
    Vec2i bboxmin(image.get_width()-1,  image.get_height()-1); 
    Vec2i bboxmax(0, 0); 
    Vec2i clamp(image.get_width()-1, image.get_height()-1); 
    for (int i=0; i<3; i++) { 
        for (int j=0; j<2; j++) { 
            bboxmin[j] = std::max(0,        std::min(bboxmin[j], pts[i][j])); 
            bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts[i][j])); 
        } 
    } 
    Vec2i P; 
    for (P.x=bboxmin.x; P.x<=bboxmax.x; P.x++) { 
        for (P.y=bboxmin.y; P.y<=bboxmax.y; P.y++) { 
            Vec3f bc_screen  = barycentric(pts, P); 
            if (bc_screen.x<0 || bc_screen.y<0 || bc_screen.z<0) continue; 
            image.set(P.x, P.y, color); 
        } 
    } 
} 
int main(int argc, char** argv) {
    TGAImage image(width, height, TGAImage::RGB);

    Vec2i t0[3] = {Vec2i(10, 70),   Vec2i(50, 160),  Vec2i(70, 80)};
    Vec2i t1[3] = {Vec2i(180, 50),  Vec2i(150, 1),   Vec2i(70, 180)};
    Vec2i t2[3] = {Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180)};

    triangle(t0[0], t0[1], t0[2], image, red);
    triangle(t1[0], t1[1], t1[2], image, white);
    triangle(t2[0], t2[1], t2[2], image, green);


    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
    return 0;
}

