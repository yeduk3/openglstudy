

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

struct MtlData
{
    std::string materialName;

    glm::vec3 ambientColor;
    glm::vec3 diffuseColor;
    glm::vec3 specularColor;

    MtlData(const std::string mName) : materialName(mName) {}

    friend std::ostream &operator<<(std::ostream &os, const MtlData &mtl)
    {
        os << "Material Name: " << mtl.materialName << std::endl;
        os << "Ambient: " << mtl.ambientColor.r << " " << mtl.ambientColor.g << " " << mtl.ambientColor.b << std::endl;
        os << "Diffuse: " << mtl.diffuseColor.r << " " << mtl.diffuseColor.g << " " << mtl.diffuseColor.b << std::endl;
        os << "Specular: " << mtl.specularColor.r << " " << mtl.specularColor.g << " " << mtl.specularColor.b;
        return os;
    }
};

struct ObjData
{
    std::string materialFile = "";
    std::string material = "";
    GLuint nVertices = 0;
    GLuint nElements3 = 0;
    GLuint nElements4 = 0;
    GLuint nNormals = 0;
    GLuint nSyncedNormals = 0;

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> textures;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec3> syncedNormals;
    std::vector<glm::u16vec3> elements3;
    std::vector<glm::u16vec4> elements4;

    std::vector<MtlData> materialData;
};

void loadMtl(const std::string mtlFileName, ObjData *object)
{
    std::fstream file(mtlFileName);
    if (!file.is_open())
    {
        std::cerr << "No .mtl file" << std::endl;
        return;
    }
    std::cout << "Read " << mtlFileName << std::endl;

    std::string type;
    while (!file.eof())
    {
        file >> type;
        if (file.eof())
            break;
        else if (type == "newmtl")
        {
            std::string mName;
            file >> mName;
            object->materialData.push_back(MtlData(mName));
        }
        else if (type == "Ka")
        {
            float r, g, b;
            file >> r >> g >> b;
            object->materialData.back().ambientColor = {r, g, b};
        }
        else if (type == "Kd")
        {
            float r, g, b;
            file >> r >> g >> b;
            object->materialData.back().diffuseColor = {r, g, b};
        }
        else if (type == "Ks")
        {
            float r, g, b;
            file >> r >> g >> b;
            object->materialData.back().specularColor = {r, g, b};
        }
    }

    std::cout << "Material Count: " << object->materialData.size() << std::endl;
    for (auto m : object->materialData)
    {
        std::cout << m << std::endl;
    }
}

ObjData loadObject(const std::string prefix, const std::string objFileName)
{
    std::fstream file(prefix + objFileName);
    ObjData object;
    if (!file.is_open())
    {
        std::cerr << "No .obj file" << std::endl;
        return object;
    }
    std::cout << "Read " << objFileName << std::endl;

    std::vector<std::string> faces;

    std::string type;
    while (!file.eof())
    {
        file >> type;
        if (file.eof())
            break;
        if (type == "mtllib")
        {
            file >> object.materialFile;
            loadMtl(prefix + object.materialFile, &object);
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
            std::string f;
            std::getline(file, f);

            faces.push_back(f);
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

    std::vector<std::vector<glm::vec3>> sNormals(object.nVertices);

    for (auto f : faces)
    {
        // case by case?
        std::vector<GLuint> elem;

        std::regex re("\\d+/\\d+/\\d+");
        auto start = std::sregex_iterator(f.begin(), f.end(), re);
        auto end = std::sregex_iterator();
        while (start != end)
        {
            std::string str = start->str();
            GLuint vertex = std::stoi(str.substr(0, str.find('/'))) - 1;
            elem.push_back(vertex);

            GLuint normal = std::stoi(str.substr(str.find_last_of('/') + 1)) - 1;
            sNormals[vertex].push_back(object.normals[normal]);

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

    for (auto sn : sNormals)
    {
        glm::vec3 sum(0);
        for (auto n : sn)
            sum += n;
        sum /= sn.size();
        object.syncedNormals.push_back(sum);
    }

    object.nElements3 = object.elements3.size();
    object.nElements4 = object.elements4.size();
    object.nNormals = object.normals.size();
    object.nSyncedNormals = object.syncedNormals.size();

    std::cout << "nVertices: " << object.nVertices << std::endl;
    std::cout << "nElements3: " << object.nElements3 << std::endl;
    std::cout << "nElements4: " << object.nElements4 << std::endl;
    std::cout << "nNormals: " << object.nNormals << std::endl;
    std::cout << "nSyncedNormals: " << object.nSyncedNormals << std::endl;

    file.close();

    return object;
}

#endif