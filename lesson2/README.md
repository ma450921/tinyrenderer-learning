## 三角形光栅化处理
将一个模型拆解为多个三角形是光栅化API的常见的逻辑，因此三角形的光栅化处理对于光栅化API极为重要。

## old school method
首先能想到的方式就是连接三角形的三个顶点，构成一个三角形的轮廓。 
``` cpp
void line(Vec2i p0, Vec2i p1, TGAImage &image, TGAColor color) {
    bool steep = false;
    if (std::abs(p0.x-p1.x)<std::abs(p0.y-p1.y)) {
        std::swap(p0.x, p0.y);
        std::swap(p1.x, p1.y);
        steep = true;
    }
    if (p0.x>p1.x) {
        std::swap(p0, p1);
    }

    for (int x=p0.x; x<=p1.x; x++) {
        float t = (x-p0.x)/(float)(p1.x-p0.x);
        int y = p0.y*(1.-t) + p1.y*t;
        if (steep) {
            image.set(y, x, color);
        } else {
            image.set(x, y, color);
        }
    }
}
void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color) {
    line(t0, t1, image, red);
    line(t1, t2, image, white);
    line(t2, t0, image, green);
}
```

## fill rect
得到三角形的轮廓之后，关键在于如何填充这个三角形。首先需要获取Y轴上填充的起点和终点, 通过替换三个点的变量值来保证 t0 t1 t2 三个点在y轴方向上的顺序
``` cpp
if (t0.y > t1.y) std::swap(t0, t1);
if (t0.y > t2.y) std::swap(t0, t2);
if (t1.y > t2.y) std::swap(t1, t2);
```

