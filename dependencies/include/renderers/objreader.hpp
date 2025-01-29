

#ifndef objreader_hpp
#define objreader_hpp

#ifdef __APPLE__
#include "OpenGL/gl.h"
#else
#include "GL/glew.h"
#endif

#include <glm/glm.hpp>

#include <vector>
#include <fstream>
#include <iostream>
#include <limits>
#include <string>
#include <regex>

struct ObjData
{
    std::string materialFile = "";
    std::string material = "";
    GLuint nVertices = 0;
    GLuint nElements3 = 0;
    GLuint nElements4 = 0;

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> textures;
    std::vector<glm::vec3> normals;
    std::vector<glm::u16vec3> elements3;
    std::vector<glm::u16vec4> elements4;
};

ObjData loadObject(const char *objFileName)
{
    std::fstream file(objFileName);
    ObjData object;
    if (!file.is_open())
    {
        std::cerr << "No .obj file" << std::endl;
        return object;
    }
    std::cout << "Read " << objFileName << std::endl;

    std::string type;
    while (!file.eof())
    {
        file >> type;
        if (file.eof())
            break;
        if (type == "mtllib")
        {
            file >> object.materialFile;
        }
        else if (type == "usemtl")
        {
            file >> object.material;
        }
        else if (type == "o" || type == "g")
        {
            if (!file.ignore(std::numeric_limits<std::streamsize>::max(), file.widen('\n')))
            {
                std::cerr << "Group Skip!" << std::endl;
            }
        }
        else if (type == "v")
        {
            float x, y, z;
            file >> x >> y >> z;
            object.vertices.push_back({x, y, z});
        }
        else if (type == "vt")
        {
            float tx, ty;
            file >> tx >> ty;
            object.textures.push_back({tx, ty});
        }
        else if (type == "vn")
        {
            float nx, ny, nz;
            file >> nx >> ny >> nz;
            object.normals.push_back({nx, ny, nz});
        }
        else if (type == "f")
        {
            // case by case?
            std::vector<GLuint> elem;
            std::string faces;
            std::getline(file, faces);

            std::regex re("\\d+/\\d+/\\d+");
            auto start = std::sregex_iterator(faces.begin(), faces.end(), re);
            auto end = std::sregex_iterator();
            while (start != end)
            {
                elem.push_back(std::stoi(start->str()) - 1);
                start++;
            }

            if (elem.size() == 4)
            {
                object.elements4.push_back({elem[0], elem[1], elem[2], elem[3]});
                object.elements3.push_back({elem[0], elem[1], elem[2]});
                object.elements3.push_back({elem[0], elem[2], elem[3]});
            }
            else if (elem.size() == 3)
                object.elements3.push_back({elem[0], elem[1], elem[2]});
            else
            {
                std::cerr << "Weird situation! f elements size is not 3 or 4." << std::endl;
                return object;
            }
        }
        else if (type == "l")
        {
            // not in this case
            if (!file.ignore(std::numeric_limits<std::streamsize>::max(), file.widen('\n')))
            {
                std::cerr << "Line polygon skip!" << std::endl;
            }
        }
        else
        {
            if (!file.ignore(std::numeric_limits<std::streamsize>::max(), file.widen('\n')))
            {
                std::cerr << "Weird situation! input " << type << " is not supported." << std::endl;
                return object;
            }
        }
        // std::cout << "Processing type " << type << std::endl;
    }

    object.nVertices = object.vertices.size();
    object.nElements3 = object.elements3.size();
    object.nElements4 = object.elements4.size();

    std::cout << "nVertices: " << object.nVertices << std::endl;
    std::cout << "nElements3: " << object.nElements3 << std::endl;
    std::cout << "nElements4: " << object.nElements4 << std::endl;

    file.close();

    return object;
}

#endif