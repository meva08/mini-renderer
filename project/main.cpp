#include <cmath>
#include "tgaimage.h"
#include <string>
#include "mesh.h"

constexpr TGAColor white   = {255, 255, 255, 255}; // attention, BGRA order
constexpr TGAColor green   = {  0, 255,   0, 255};
constexpr TGAColor red     = {  0,   0, 255, 255};
constexpr TGAColor blue    = {255, 128,  64, 255};
constexpr TGAColor yellow  = {  0, 200, 255, 255};

// globals
constexpr int width  = 128;
constexpr int height = 128;

void drawLine(int ax, int ay, int bx, int by, TGAImage &image, const TGAColor &color);
void drawMesh(Mesh m, TGAImage &image);
void drawTriangle(vertex v0, vertex v1, vertex v2, TGAImage &image);
vertex project(vertex v);
void triangle(int x0, int y0, int x1, int y1, int x2, int y2, TGAImage &image, const TGAColor &color);
float cross(int ax, int ay, int bx, int by, int px, int py);


int main(int argc, char** argv) {
    if (argc != 2)
    {
        printf("Please input filepath for mesh.\n");
        return 1;
    }


    TGAImage framebuffer(width, height, TGAImage::RGB);

    Mesh m;
    m.readOBJ(argv[1]);

    triangle(  7, 45, 35, 100, 45,  60, framebuffer, red);
    triangle(120, 35, 90,   5, 45, 110, framebuffer, white);
    triangle(115, 83, 80,  90, 85, 120, framebuffer, green);

    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}

void drawLine(int ax, int ay, int bx, int by, TGAImage &image, const TGAColor &color)
{
    bool steep = std::abs(ax - bx) < std::abs(ay - by);

    if (steep)
    {
        std::swap(ax, ay);
        std::swap(bx, by);
    }

    if (ax > bx)
    {
        std::swap(ax, bx);
        std::swap(ay, by);
    }

    int y = ay;
    int ierror = 0;

    for (float x = ax; x <= bx; x ++)
    {
        float t = (x - ax) / static_cast<float>(bx - ax);

        if (steep)
        {
            image.set(y, x, color);
        }
        else
        {
            image.set(x, y, color);
        }

        ierror += 2 * std::abs(by - ay);

        if (ierror > bx - ax)
        {
            y += (by > ay ? 1 : -1);
            ierror -= 2 * (bx - ax);
        };
    }
}

void drawMesh(Mesh m, TGAImage &image)
{
    int n = m.faces.size();

    for (int i = 0; i + 2 < n; i += 3)
    {
        drawTriangle(m.vertices[m.faces[i]], m.vertices[m.faces[i + 1]], m.vertices[m.faces[i + 2]], image);
    }
}

void drawTriangle(vertex v0, vertex v1, vertex v2, TGAImage &image)
{
    // viewport transform
    vertex vp0 = project(v0);
    vertex vp1 = project(v1);
    vertex vp2 = project(v2);

    // draw triangle points
    image.set(vp0.x, vp0.y, white);
    image.set(vp1.x, vp1.y, white);
    image.set(vp2.x, vp2.y, white);

    // draw lines between them
    drawLine(vp0.x, vp0.y, vp1.x, vp1.y, image, blue);
    drawLine(vp1.x, vp1.y, vp2.x, vp2.y, image, blue);
    drawLine(vp2.x, vp2.y, vp0.x, vp0.y, image, blue);
}

void triangle(int ax, int ay, int bx, int by, int cx, int cy, TGAImage &image, const TGAColor &color)
{
    if (ay > by) { std::swap(ax, bx); std::swap(ay, by); }
    if (ay > cy) { std::swap(ax, cx); std::swap(ay, cy); }
    if (by > cy) { std::swap(bx, cx); std::swap(by, cy); }

    // find min/max values for bb
    int minX = std::min({ax, bx, cx});
    int maxX = std::max({ax, bx, cx});
    int minY = std::min({ay, by, cy});
    int maxY = std::max({ay, by, cy});

    // find area
    float area = cross(ax, ay, bx, by, cx, cy);

    #pragma omp parallel for
    for (int x = minX; x <= maxX; x++)
    {
        for (int y = minY; y <= maxY; y++)
        {
            // for each pixel

            // get w values
            float w0 = cross(ax, ay, bx, by, x, y);
            float w1 = cross(bx, by, cx, cy, x, y);
            float w2 = cross(cx, cy, ax, ay, x, y);

            float alpha = w0 / area;
            float beta = w1 / area;
            float gamma = w2 / area;

            if (alpha >= 0 && beta >= 0 && gamma >= 0)
            {
                image.set(x, y, color);
            }
        }
    }

    // draw outline
    drawLine(ax, ay, bx, by, image, color);
    drawLine(bx, by, cx, cy, image, color);
    drawLine(cx, cy, ax, ay, image, color);
}

vertex project(vertex v)
{
    // drop z coord
    vertex newV;
    newV.x = (v.x + 1.0)  * width / 2;
    newV.y = (v.y + 1.0) * height / 2;
    return newV;
}

float cross(int ax, int ay, int bx, int by, int px, int py)
{
    // vec 1 = B  - A
    float v1X = bx - ax;
    float v1Y = by - ay;
    // vec 2 = P - A
    float v2X = px - ax;
    float v2Y = py - ay;

    return (v1X * v2Y) - (v1Y * v2X);
}