#pragma once

#include <string>
#include <vector>
#include <fstream>

struct vertex
{
    float x, y, z;
};

class Mesh
{
    public:
        std::vector<vertex> vertices;
        std::vector<int> faces;
        void readOBJ(std::string filename);
        int numVerts();
        int numFaces();
};