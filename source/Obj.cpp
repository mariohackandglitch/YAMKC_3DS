#include "Obj.hpp"
#include <fstream>
#include <iostream>
#include "stdio.h"
#include <sstream>
#include <algorithm>

static inline std::string& Ltrim(std::string& str)
{
	auto it = std::find_if(str.begin(), str.end(), [](char ch) { return (!std::isspace(ch)); });
	str.erase(str.begin(), it);
	return (str);
}

static inline std::string& Rtrim(std::string& str)
{
	auto it = std::find_if(str.rbegin(), str.rend(), [](char ch) { return (!std::isspace(ch)); });
	str.erase(it.base(), str.end());
	return (str);
}

static inline std::string& Trim(std::string& str)
{
	return (Ltrim(Rtrim(str)));
}

static inline std::vector<std::string> Split(const std::string& str, char splitChar, bool ignoreEmpty = true) {
    std::stringstream origStr(str);
    std::string segment;
    std::vector<std::string> seglist;

    while (std::getline(origStr, segment, splitChar)) 
    {
        if (!(segment.empty() && ignoreEmpty)) seglist.push_back(segment);
    }
    return seglist;
}

static const char kPathSeparator = '/';

static inline std::pair<std::string, std::string> SplitFilename(const std::string& str)
{
    size_t found;
    found = str.find_last_of("/\\");
    return { str.substr(0, found), str.substr(found + 1) };
}

Obj::Obj(std::string filename)
{
    FILE* file = fopen(filename.c_str(), "rb");
    
    if (!file)
    {
        std::cout << "Failed to open obj" << std::endl;
        return;
    }
    
    fseek(file, 0, SEEK_END);
    u32 fSize = ftell(file);
    fseek(file, 0, SEEK_SET);
    char* buff = (char*)malloc(fSize);
    fread(buff, 1, fSize, file);
    fclose(file);

    std::istringstream objFile(buff);
    matLib = "";
    std::string line;
    std::string currMat = "Default";

    std::vector<Vector3> objVertices;
    std::vector<Vector3> objNormals;
    std::vector<Vector2> objTexCoords;

    while(std::getline(objFile, line)) {
        line = Trim(line);
        if (line.empty() || line[0] == '#') continue;
        auto pieces = Split(line, ' ');
        if (pieces.empty()) continue;

        if (pieces[0] == "mtllib") {
            if (pieces.size() != 2)
                ;
            matLib = SplitFilename(filename).first + kPathSeparator + Trim(pieces[1]);
        }
        else if (pieces[0] == "usemtl") {
            if (pieces.size() != 2)
                ;
            currMat = Trim(pieces[1]);
        }
        else if (pieces[0] == "v") {
            if (pieces.size() != 4)
                ;
            
            {
                objVertices.push_back(Vector3(std::stof(pieces[1]), std::stof(pieces[2]), std::stof(pieces[3])));
            }
            
            {
                ;
            }
        }
        else if (pieces[0] == "vn") {
            if (pieces.size() != 4)
                ;
            
            {
                Vector3 normal = Vector3(std::stof(pieces[1]), std::stof(pieces[2]), std::stof(pieces[3]));
                normal.Normalize();
                objNormals.push_back(normal);
            }
            
            {
                ;
            }
        }
        else if (pieces[0] == "vt") {
            if (pieces.size() != 3)
                ;
            
            {
                objTexCoords.push_back(Vector2(std::stof(pieces[1]), std::stof(pieces[2])));
            }
            
            {
                ;
            }
        }
        else if (pieces[0] == "f") {
            if (pieces.size() != 4)
                ;
            int vertex[3], normals[3], texCoords[3];
            for (int i = 0; i < 3; i++) {
                vertex[i] = normals[i] = texCoords[i] = -1;
                auto subParts = Split(pieces[i + 1], '/', false);
                if (subParts.empty() || subParts[0].empty())
                    ;
                
                {
                    vertex[i] = std::stoi(subParts[0]) - 1;
                    if (vertex[i] < 0)
                        ;
                }
                
                {
                    ;
                }
                if ((subParts.size() == 3 || subParts.size() == 2) && !subParts[1].empty())
                {
                    
                    {
                        texCoords[i] = std::stoi(subParts[1]) - 1;
                        if (texCoords[i] < 0)
                            ;
                    }
                    
                    {
                        ;
                    }
                }
                if (subParts.size() == 3 && !subParts[2].empty())
                {
                    
                    {
                        normals[i] = std::stoi(subParts[2]) - 1;
                        if (normals[i] < 0)
                            ;
                    }
                    
                    {
                        ;
                    }
                }                
            }
            Graphics::GPUVertex vInfo[3];
            for (int i = 0; i < 3; i++)
            {
                Vector3 position(0.f, 0.f, 0.f);
                Vector3 normal(0.f, 1.f, 0.f);
                Vector2 texCoord(0.f, 0.f);
                if (vertex[i] != -1)
                    position = objVertices[vertex[i]];
                if (normals[i] != -1)
                    normal = objNormals[normals[i]];
                if (texCoords[i] != -1)
                    texCoord = objTexCoords[texCoords[i]];
                
                vInfo[i].position.x = position.x;
                vInfo[i].position.y = position.y;
                vInfo[i].position.z = position.z;
                vInfo[i].normal.x = normal.x;
                vInfo[i].normal.y = normal.y;
                vInfo[i].normal.z = normal.z;
                vInfo[i].texcoord.u = texCoord.x;
                vInfo[i].texcoord.v = texCoord.y;
                vInfo[i].color = {1.f, 1.f, 1.f, 1.f};
                
            }
            GetMaterial(currMat).AddFace(std::make_tuple(vInfo[0], vInfo[1], vInfo[2]));

        }
    }
    objFile.clear();
    free(buff);
    ConvertToVBO();
    if (!matLib.empty())
        LoadMatlib(matLib);
}

Obj::~Obj()
{
}

void Obj::Draw()
{
    C3D_Mtx* m = Graphics::PushModelViewMtx();
    Mtx_Translate(m, position.x, position.y, position.z, true);
    Mtx_RotateX(m, rotation.x.AsRadians(), true);
    Mtx_RotateY(m, rotation.y.AsRadians(), true);
    Mtx_RotateZ(m, rotation.z.AsRadians(), true);
    Mtx_RotateX(m, preRotation.x.AsRadians(), true);
    Mtx_RotateY(m, preRotation.y.AsRadians(), true);
    Mtx_RotateZ(m, preRotation.z.AsRadians(), true);
    Mtx_Scale(m, scale.x, scale.y, scale.z);
    Graphics::UpdateModelViewMtx();

    for (auto it = materials.begin(); it != materials.end(); it++)
        (*it).Draw();
    Graphics::PopModelViewMtx();
}

Vector3& Obj::GetPosition()
{
    return position;
}

Angle3& Obj::GetRotation()
{
    return rotation;
}

Angle3& Obj::GetPreRotation()
{
    return preRotation;
}

Vector3& Obj::GetScale()
{
    return scale;
}

std::vector<Obj::Material>& Obj::Materials() {
    return materials;
}

Obj::Material& Obj::GetMaterial(std::string name)
{
    for (unsigned int i = 0; i < materials.size(); i++)
    {
        if (materials[i].GetName() == name)
            return materials[i];
    }
    materials.push_back(Material(this, name));
    return materials.back();
}

void Obj::LoadMatlib(std::string filename)
{
    FILE* file = fopen(filename.c_str(), "rb");
    
    if (!file)
        std::cout << "Failed to open mtl" << std::endl;
    
    fseek(file, 0, SEEK_END);
    u32 fSize = ftell(file);
    fseek(file, 0, SEEK_SET);
    char* buff = (char*)malloc(fSize);
    fread(buff, 1, fSize, file);
    fclose(file);

    std::istringstream mtlFile(buff);
    matLib = "";
    std::string line;
    std::string currMat = "Default";

    while (std::getline(mtlFile, line)) {
        line = Trim(line);
        if (line.empty() || line[0] == '#') continue;
        auto pieces = Split(line, ' ');
        if (pieces.empty()) continue;

        if (pieces[0] == "newmtl") {
            if (pieces.size() != 2)
                ;
            currMat = Trim(pieces[1]);
        }
        else if (pieces[0] == "Kd") {
            if (pieces.size() != 4)
                ;
            
            {
                float r = std::stof(pieces[1]);
                float g = std::stof(pieces[2]);
                float b = std::stof(pieces[3]);
                if (r < 0.f || r > 1.f || g < 0.f || g > 1.f || b < 0.f || b > 1.f)
                    ;
                //GetMaterial(currMat).SetColor(Material::ColorType::DIFFUSE, Color(r,g,b));
            }
            
            {
                ;
            }            
        }
        else if (pieces[0] == "Ka") {
            if (pieces.size() != 4)
                ;
            
            {
                float r = std::stof(pieces[1]);
                float g = std::stof(pieces[2]);
                float b = std::stof(pieces[3]);
                if (r < 0.f || r > 1.f || g < 0.f || g > 1.f || b < 0.f || b > 1.f)
                    ;
                //GetMaterial(currMat).SetColor(Material::ColorType::AMBIENT, Color(r, g, b));
            }
            
            {
                ;
            }
        }
        else if (pieces[0] == "Ks") {
            if (pieces.size() != 4)
                ;
            
            {
                float r = std::stof(pieces[1]);
                float g = std::stof(pieces[2]);
                float b = std::stof(pieces[3]);
                if (r < 0.f || r > 1.f || g < 0.f || g > 1.f || b < 0.f || b > 1.f)
                    ;
                //GetMaterial(currMat).SetColor(Material::ColorType::SPECULAR, Color(r, g, b));
            }
            
            {
                ;
            }
        }
        else if (pieces[0] == "map_Kd") {
            if (pieces.size() != 2)
                ;
            std::string texFile = SplitFilename(filename).first + kPathSeparator + Trim(pieces[1]);
            GetMaterial(currMat).SetTexture(texFile);
        }
    }
    mtlFile.clear();
    free(buff);
}

void Obj::ConvertToVBO(void)
{
    for (auto it = materials.begin(); it != materials.end(); it++)
        (*it).ConvertToVBO();
}

Obj::Material::Material(Obj* parent, std::string name)
{
    this->parent = parent;
    this->name = name;
    isVisible = true;
    vArray = nullptr;
}

Obj::Material::~Material()
{
    if (texture)
        delete texture;
    if (vArray)
        Graphics::VertexArray::Dispose(vArray);
}

std::string& Obj::Material::GetName()
{
    return name;
}

void Obj::Material::AddFace(const Obj::Face& face)
{
    faces.push_back(face);
}

void Obj::Material::SetTexture(const std::string& fileName)
{
    if (texture)
        delete texture;
    texture = new Texture(fileName);
}

void Obj::Material::SetVisible(bool visible)
{
    isVisible = visible;
}

void Obj::Material::Draw()
{
    if (!isVisible)
        return;
    
    if (texture)
    { // Modulate the vertex color with the texture
        C3D_TexEnv* env = C3D_GetTexEnv(0);
        C3D_TexEnvInit(env);
        C3D_TexEnvSrc(env, C3D_Both, GPU_PRIMARY_COLOR, GPU_TEXTURE0, GPU_CONSTANT); // Last arg is unused
        C3D_TexEnvFunc(env, C3D_Both, GPU_MODULATE);

        texture->Bind();
    }
    else
    { // Only use the vertex color
        C3D_TexEnv* env = C3D_GetTexEnv(0);
        C3D_TexEnvInit(env);
        C3D_TexEnvSrc(env, C3D_Both, GPU_PRIMARY_COLOR, GPU_CONSTANT, GPU_CONSTANT); // Last 2 args is unused
        C3D_TexEnvFunc(env, C3D_Both, GPU_REPLACE);
    }

    vArray->Draw(GPU_TRIANGLES);
}

void Obj::Material::ConvertToVBO(void)
{
    vArray = Graphics::VertexArray::Create();
    for (auto it = faces.cbegin(); it != faces.cend(); it++) {
        vArray->AddVertex(std::get<0>(*it));
        vArray->AddVertex(std::get<1>(*it));
        vArray->AddVertex(std::get<2>(*it));
    }
    vArray->Complete();
    faces.clear();
}