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
constexpr int width  = 800;
constexpr int height = 800;

void drawLine(int ax, int ay, int bx, int by, TGAImage &image, const TGAColor &color);
void drawMesh(Mesh m, TGAImage &image);
void drawTriangle(vertex v0, vertex v1, vertex v2, TGAImage &image);
vertex project(vertex v);


int main(int argc, char** argv) {
    if (argc != 2)
    {
        printf("Please input filepath for mesh.\n");
        return 1;
    }


    TGAImage framebuffer(width, height, TGAImage::RGB);

    Mesh m;
    m.readOBJ(argv[1]);

    // render mesh data
    drawMesh(m, framebuffer);

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

    for (int i = 0; i < n - 3; i += 3)
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

vertex project(vertex v)
{
    // drop z coord
    vertex newV;
    newV.x = (v.x + 1.0)  * width / 2;
    newV.y = (v.y + 1.0) * height / 2;
    return newV;
}