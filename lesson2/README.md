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
在此之后需要在y方向上进行遍历，开始从下往上逐行扫描，每次计算三角形与逐行扫描线的两个交点A和B，填充A与B中间的所有像素
``` cpp
int total_height = t2.y-t0.y; 
for (int y=t0.y; y<=t1.y; y++) { 
    int segment_height = t1.y-t0.y+1; 
    float alpha = (float)(y-t0.y)/total_height; 
    float beta  = (float)(y-t0.y)/segment_height; // be careful with divisions by zero 
    Vec2i A = t0 + (t2-t0)*alpha; 
    Vec2i B = t0 + (t1-t0)*beta; 
    image.set(A.x, y, red); 
    image.set(B.x, y, green); 
}
```
以上代码步骤仅填充了三角形的下半部分，因为上半部分的角度需要通过t1重新计算如下：
```cpp
for (int y=t1.y; y<=t2.y; y++) { 
    int segment_height =  t2.y-t1.y+1; 
    float alpha = (float)(y-t0.y)/total_height; 
    float beta  = (float)(y-t1.y)/segment_height; // be careful with divisions by zero 
    Vec2i A = t0 + (t2-t0)*alpha; 
    Vec2i B = t1 + (t2-t1)*beta; 
    if (A.x>B.x) std::swap(A, B); 
    for (int j=A.x; j<=B.x; j++) { 
        image.set(j, y, color); // attention, due to int casts t0.y+i != A.y 
    } 
} 
```
将上述代码抽象为可读性更强的代码：
``` cpp
void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color) { 
    if (t0.y==t1.y && t0.y==t2.y) return; // I dont care about degenerate triangles 
    // sort the vertices, t0, t1, t2 lower−to−upper (bubblesort yay!) 
    if (t0.y>t1.y) std::swap(t0, t1); 
    if (t0.y>t2.y) std::swap(t0, t2); 
    if (t1.y>t2.y) std::swap(t1, t2); 
    int total_height = t2.y-t0.y; 
    for (int i=0; i<total_height; i++) { 
        bool second_half = i>t1.y-t0.y || t1.y==t0.y; 
        int segment_height = second_half ? t2.y-t1.y : t1.y-t0.y; 
        float alpha = (float)i/total_height; 
        float beta  = (float)(i-(second_half ? t1.y-t0.y : 0))/segment_height; // be careful: with above conditions no division by zero here 
        Vec2i A =               t0 + (t2-t0)*alpha; 
        Vec2i B = second_half ? t1 + (t2-t1)*beta : t0 + (t1-t0)*beta; 
        if (A.x>B.x) std::swap(A, B); 
        for (int j=A.x; j<=B.x; j++) { 
            image.set(j, t0.y+i, color); // attention, due to int casts t0.y+i != A.y 
        } 
    } 
}
```

## 光栅化处理思路
光栅化的处理思路和上述方法类似，归根到底还是遍历屏幕内的每一个像素判断是否在三角形内。
判断是否在三角形内实际上是可以通过向量计算的方法来进行实现的。
假定三角形的三个点分别为 p0 p1 p2
判断的点为 q，计算三个点与q的连线，以及三个点顺序连线的叉乘，若三者同号则q在三角形内
p0p1✖️p0q p1p2✖️p1q p2p0✖️p2q 

另外，扫描整个屏幕的像素显然是不合理的，我们可以通过扫描三角形的包围盒从而来判断是否在三角形内
