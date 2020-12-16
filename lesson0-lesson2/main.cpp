#include "tgaimage.h"
#include <cmath>
const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);

// first step
// void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
//     for (float t=0.; t<1.; t+=.1) {
//         int x = x0*(1.-t) + x1*t;
//         int y = y0*(1.-t) + y1*t;
//         image.set(x, y, color);
//     }
// }

/**
 * second attempt
 * 反向绘制某条线是无法绘制的，斜率比较大的时候不够连续
 */
// void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
//     for (int x = x0; x <= x1; x++) {
//         float t = (x - x0) / (float)(x1 - x0);
//         int y = y0*(1.-t) + y1*t;
//         image.set(x, y, color);
//     }
// }

/**
 * thrid attempt
 * 通过判断x0 > x1的情况
 */
// void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
//     bool steep = false;
//     if (std::abs(x0 - x1) < std::abs(y0 - y1)) {
//         // 永远保持斜率<1的情况进行绘制避免不够连续
//         std::swap(x0, y0);
//         std::swap(x1, y1);
//         steep = true;
//     }
//     if (x0 > x1) {
//         std::swap(x0, x1);
//         std::swap(y0, y1);
//     }
//     for (int x = x0; x <= x1; x++)
//     {
//         float t = (x - x0) / (float)(x1 - x0);
//         int y = y0*(1.-t) + y1*t;
//         if (steep) {
//             image.set(y, x, color);
//         } else {
//             image.set(x, y, color);
//         }
//     }
// }

/**
 * fourth attempt
 * thrid attemp方案中使用的方式对于执行效率来说是非常不友好的
 * 因为它里面有很多的浮点型四则运算，尤其是每次计算y值的时候
 * 这里就可以引入DDA算法，这个算法的核心概念是
 * 1. 确定递增轴是x还是y，如果斜率 >= 1 则使用x轴作为递增轴，反之则y轴为递增轴
 * 2. 每次循环时叠加递增斜率值，每次y值会有两个选项，递增1或者递增0
 * 3. 当前递增值> 0.5时，则递增1并将递增值-1否则递增0
 */
// void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
//     bool steep = false;
//     if (std::abs(x0 - x1) < std::abs(y0 - y1)) {
//         // 永远保持斜率<1的情况进行绘制避免不够连续
//         std::swap(x0, y0);
//         std::swap(x1, y1);
//         steep = true;
//     }
//     if (x0 > x1) {
//         std::swap(x0, x1);
//         std::swap(y0, y1);
//     }
//     int dx = x1 - x0;
//     int dy = y1 - y0;
//     float derror = std::abs(float(dy)/float(dx));
//     float error = 0;
//     int y = y0;
//     for (int x = x0; x < x1; x++) {
//         if (steep) {
//             image.set(y, x, color);
//         } else {
//             image.set(x, y, color);
//         }
//         error += derror;
//         if (error > .5) {
//             y += (y1 > y0 ? 1 : -1);
//             error -= 1;
//         }
//     }
// }

/** 
 *  fifth attemp
 *  在 fourth attemp中的DDA算法实际并不是最高效的，最高效的应该是bresenham算法
 *  bresenham算法 的优化核心在于，干掉DDA算法中的float类型变量的开销，核心思路如下：
 *  1. 确定递增轴是x还是y，如果斜率 >= 1 则使用x轴作为递增轴，反之则y轴为递增轴，每次递增一个像素
 *  2. 每次循环时叠加递增斜率值，每次y值会有两个选项，递增1或者递增0
 *  3. 这里定义每次的y差值为 2dy, 当x递增时叠加y差值，当叠加值 > dx的时候，y方向上递增1，并令递增值 - 2dx
 */

void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
    bool steep = false;
    if (std::abs(x0 - x1) < std::abs(y0 - y1)) {
        // 永远保持斜率<1的情况进行绘制避免不够连续
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }
    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    int dx = x1 - x0;
    int dy = y1 - y0;
    int derror2 = dy * 2;
    int error2 = 0;
    int y = y0;
    for (int x = x0; x < x1; x++) {
        if (steep) {
            image.set(y, x, color);
        } else {
            image.set(x, y, color);
        }
        error2 += derror2;
        if (error2 > dx) {
            y += (y1 > y0 ? 1 : -1);
            error2 -= dx * 2;
        }
    }
}


int main(int argc, char** argv) {
    TGAImage image(100, 100, TGAImage::RGB);
    line(13, 20, 80, 40, image, white);
    line(20, 13, 40, 80, image, red); 
    line(80, 40, 13, 20, image, red);
    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
    return 0;
}