## 直线光栅化表示
和绘制一个点相比，绘制一条直线就需要我们关注如何通过直线中每个像素点的位置来表示这个像素点。在photoshop中绘制一条直线时，应用自动产生的像素点的位置。通过下图可以看出，在绘制一条水平或者垂直的直线的时候，只需要连续在对应方向进行绘制即可。但是当直线的斜率为时，表示一条直线就困难很多。此时无法利用像素点直接还原这条直线，只能通过类似表示的方式来表示这条直线。如何绘制直线，演进出很多算法，下面介绍绘制直线的算法。
![](https://ma450921.github.io/images/graphics4/graphics4_lines.png)

## DDA
DDA的思路很简单，具体过程如下
1. 确定递增轴是x还是y，如果斜率 >= 1 则使用x轴作为递增轴，反之则y轴为递增轴
2. 每次循环时叠加递增斜率值，每次y值会有两个选项，递增1或者递增0
3. 当前递增值> 0.5时，则y值递增1并将递增值-1否则递增0。
``` cpp
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
    float derror = std::abs(float(dy)/float(dx));
    float error = 0;
    int y = y0;
    for (int x = x0; x < x1; x++) {
        if (steep) {
            image.set(y, x, color);
        } else {
            image.set(x, y, color);
        }
        error += derror;
        if (error > .5) {
            y += (y1 > y0 ? 1 : -1);
            error -= 1;
        }
    }
}
```

## bresenham
bresenham算法 的优化核心在于，干掉DDA算法中的float类型变量的开销，核心思路如下：
1. 确定递增轴是x还是y，如果斜率 >= 1 则使用x轴作为递增轴，反之则y轴为递增轴，每次递增一个像素
2. 每次循环时叠加递增斜率值，每次y值会有两个选项，递增1或者递增0
3. 这里定义每次的y差值为 2dy, 当x递增时叠加y差值，当叠加值 > dx的时候，y方向上递增1，并令递增值 - 2dx
``` cpp
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
```
