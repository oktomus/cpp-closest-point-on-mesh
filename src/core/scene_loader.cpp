#include "scene_loader.h"

#include "mesh.h"
#include "scene.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/material.h>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>

#include <cstddef>
#include <iostream>
#include <list>
#include <vector>

namespace core
{

namespace
{
    Mesh process_mesh_node(aiMesh* mesh, const aiScene* scene)
    {
        std::vector<Mesh::Vertex> vertices;
        std::vector<GLuint> triangles;

        // Load mesh vertices and normals.
        vertices.reserve(mesh->mNumVertices);

        for (std::size_t i = 0; i < mesh->mNumVertices; ++i)
        {
            const aiVector3D& aiVertex = mesh->mVertices[i];
            const aiVector3D& aiNormal = mesh->mNormals[i];

            Mesh::Vertex vertex;
            vertex.pos.x = aiVertex.x;
            vertex.pos.y = aiVertex.y;
            vertex.pos.z = aiVertex.z;

            vertex.normal.x = aiNormal.x;
            vertex.normal.y = aiNormal.y;
            vertex.normal.z = aiNormal.z;

            vertices.push_back(vertex);
        }

        // Load mesh triangles.
        triangles.reserve(mesh->mNumFaces);

        for (std::size_t i = 0; i < mesh->mNumFaces; ++i)
        {
            const aiFace& face = mesh->mFaces[i];

            for (std::size_t j = 0; j < face.mNumIndices; ++j)
            {
                triangles.push_back(face.mIndices[j]);
            }
        }

        return Mesh(vertices, triangles);
    }

    std::vector<Mesh> load_meshes_from_file(const std::string& file_path)
    {
        Assimp::Importer importer;

        // Normalize the scene in [-1, 1]
        importer.SetPropertyInteger("PP_PTV_NORMALIZE", 1);

        const aiScene* scene = importer.ReadFile(
            file_path, 
            aiProcess_Triangulate 
            | aiProcess_FlipUVs
            | aiProcess_GenNormals
            | aiProcess_ForceGenNormals
            | aiProcess_PreTransformVertices);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            std::cerr << "Unable to import file: " << importer.GetErrorString() << "\n";
            return {};
        }

        std::list<aiNode*> process_stack;
        process_stack.push_back(scene->mRootNode);

        std::vector<Mesh> meshes;

        // Process scene tree.
        while (process_stack.size())
        {
            aiNode* node = process_stack.front();
            process_stack.pop_front();

            // Process current node meshes
            for (std::size_t i = 0; i < node->mNumMeshes; ++i)
            {
                aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
                meshes.push_back(process_mesh_node(mesh, scene));
            }

            // Find childrens and add them to the stack
            for (std::size_t i = 0; i < node->mNumChildren; ++i)
            {
                process_stack.push_back(node->mChildren[i]);
            }
        }

        return meshes;
    }
}

Scene* load_scene_from_file(const std::string& file_path)
{
    std::vector<Mesh> scene_meshes = load_meshes_from_file(file_path);
    return new Scene(scene_meshes);
}

} // namespace core