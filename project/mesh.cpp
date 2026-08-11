#include "mesh.h"

void Mesh::readOBJ(std::string filename)
{
    std::ifstream file(filename);

    std::vector<vertex> vertices;
    std::vector<int> faces;
    std::string line;

    while(std::getline(file, line))
    {
        char trash;

        if (line.starts_with("v "))
        {
            vertex curr_v;
            std::sscanf(line.c_str(), "v %f %f %f", &curr_v.x, &curr_v.y, &curr_v.z);
            vertices.push_back(curr_v);
        }
        else if (line.starts_with("f "))
        {
            int v1, v2, v3;
            if (std::sscanf(line.c_str(), "f %i/%*i/%*i %i/%*i/%*i %i/%*i/%*i", &v1, &v2, &v3) == 3)
            {
                faces.push_back(v1 - 1);
                faces.push_back(v2 - 1);
                faces.push_back(v3 - 1);
            }
        }
    }

    this->vertices = vertices;
    this->faces = faces;
    printf("read in mesh. vertices: %i faces: %i\n", vertices.size(), faces.size());
}

int Mesh::numVerts()
{
    return this->vertices.size();
}

int Mesh::numFaces()
{
    return this->faces.size() / 3;
}