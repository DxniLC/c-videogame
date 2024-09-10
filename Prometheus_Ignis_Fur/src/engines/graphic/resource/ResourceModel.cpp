
#include "ResourceModel.hpp"

#include <assimp/Importer.hpp>  // C++ importer interface
#include <assimp/scene.h>       // Output data structure
#include <assimp/postprocess.h> // Post processing flags

#include <iostream>

#include <glm/mat4x4.hpp>

#include <engines/resource/TResourceManager.hpp>

#include "../SGEngine/scene/entity/light/TLight.hpp"

#include <engines/graphic/SGEngine/util/Assimp_GLM.hpp>

void ResourceModel::draw(glm::mat4 matrixTransform)
{
    shader->use();
    if (HasInstances)
    {
        for (auto &mesh : meshes)
            mesh->drawInstances(*shader);
    }
    else
    {
        for (auto &mesh : meshes)
            mesh->draw(*shader, matrixTransform);
    }
}

void ResourceModel::updateBones(std::vector<glm::mat4> const &finalBoneTransforms)
{
    shader->use();
    for (std::size_t i = 0; i < finalBoneTransforms.size(); i++)
        shader->setMat4("finalBonesTransforms[" + std::to_string(i) + "]", finalBoneTransforms[i]);
}

void ResourceModel::loadFile(const char *path)
{

    pathMesh = path;

    filename = pathMesh;

    // Instancia Importer
    Assimp::Importer importer;
    // aiProcess_JoinIdenticalVertices with shadersmooth on blender and export without uv coordinates
    const aiScene *scene = importer.ReadFile(path,
                                             aiProcess_Triangulate |  // si no es solo de triangulos, lo tiene que dividir
                                                 aiProcess_FlipUVs |  // da la vuelta a las coordenadas de la textura cuando sea necesario
                                                 aiProcess_GenNormals // crea normales para cada vertice si no tiene el modelo
                                                                      //  aiProcess_SplitLargeMeshes |   // divide las mallas grandes en sub mallas
    );                                                                // aiProcess_OptimizeMeshes Une varias mallas en una mas grande para reducir las llamadas a draw

    if (not scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || not scene->mRootNode)
    {
        std::cout << "\n *** ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }

    processNode(scene->mRootNode, scene);

    importer.FreeScene();
}

void ResourceModel::loadFileWithInstances(const char *path, std::vector<glm::mat4> &matricesTransforms)
{
    HasInstances = true;
    loadFile(path);

    // Si hay mas de una mesh, ASSERT
    for (auto &mesh : meshes)
        mesh->setupInstancing(matricesTransforms);
}

void ResourceModel::processNode(aiNode *node, const aiScene *scene)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        processMesh(mesh, scene);
        if (not HasInstances)
            meshes.emplace_back(std::make_unique<Mesh>(meshResult))->setup(HasBones);
        else
            meshes.emplace_back(std::make_unique<Mesh>(meshResult));
    }
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

void ResourceModel::processMesh(aiMesh *mesh, const aiScene *scene)
{
    // Reset Values of Result
    meshResult.vertex.clear();
    meshResult.index.clear();
    meshResult.material = nullptr;
    meshResult.position = glm::vec3{0.0f};

    std::pair<float, float> limitX{__FLT_MAX__, -__FLT_MAX__};
    std::pair<float, float> limitY{__FLT_MAX__, -__FLT_MAX__};
    std::pair<float, float> limitZ{__FLT_MAX__, -__FLT_MAX__};

    // Process Vertex
    HasBones = mesh->HasBones();
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;

        if (HasBones)
            setVertexBoneToDefault(vertex);

        vertex.Position = AssimpGLMHelper::aiVector3ToGLM(mesh->mVertices[i]);

        auto const &vector = vertex.Position;

        if (vector.x <= limitX.first)
            limitX.first = vector.x;
        if (vector.x >= limitX.second)
            limitX.second = vector.x;

        if (vector.y <= limitY.first)
            limitY.first = vector.y;
        if (vector.y >= limitY.second)
            limitY.second = vector.y;

        if (vector.z <= limitZ.first)
            limitZ.first = vector.z;
        if (vector.z >= limitZ.second)
            limitZ.second = vector.z;

        // std::cout << "\n Vertice(" << vertex.Position.x << ", " << vertex.Position.y << ", " << vertex.Position.z << ")";

        // Normals
        if (mesh->HasNormals())
            vertex.Normal = AssimpGLMHelper::aiVector3ToGLM(mesh->mNormals[i]);

        // std::cout << "\n Normal(" << vertex.Normal.x << ", " << vertex.Normal.y << ", " << vertex.Normal.z << ")";

        // Texture coords
        if (mesh->mTextureCoords[0])
            vertex.TexCoord = AssimpGLMHelper::aiVector2ToGLM(mesh->mTextureCoords[0][i]);
        else
            vertex.TexCoord = glm::vec2(0.0f, 0.0f);

        meshResult.vertex.emplace_back(vertex);

        meshResult.position.x += mesh->mVertices[i].x;
        meshResult.position.y += mesh->mVertices[i].y;
        meshResult.position.z += mesh->mVertices[i].z;
    }

    meshResult.position /= float(mesh->mNumVertices);

    // Process Index
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace &face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            meshResult.index.emplace_back(face.mIndices[j]);
    }

    // Process Material

    aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

    meshResult.material = this->resourceManager->getMaterial((pathMesh + "/" + material->GetName().C_Str()).c_str());

    meshResult.material->diffuseMap = loadMaterialTextures(material, aiTextureType_DIFFUSE, TextureType::DIFFUSE);
    meshResult.material->specularMap = loadMaterialTextures(material, aiTextureType_SPECULAR, TextureType::SPECULAR);
    meshResult.material->lightMap = loadMaterialTextures(material, aiTextureType_LIGHTMAP, TextureType::LIGHTMAP);

    // // Values of Material
    aiColor4D diffuseColor, specularColor;
    float shininess;

    if (AI_FAILURE != material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor))
    {
        meshResult.material->diffuse = AssimpGLMHelper::aiColor4ToGLM(diffuseColor);
    }

    if (AI_FAILURE != material->Get(AI_MATKEY_COLOR_SPECULAR, specularColor))
    {
        meshResult.material->specular = AssimpGLMHelper::aiColor4ToGLM(specularColor);
    }

    if (AI_FAILURE != material->Get(AI_MATKEY_SHININESS, shininess))
    {
        meshResult.material->shininess = shininess;
    }

    if (limitX.first <= limitModelX.first)
        limitModelX.first = limitX.first;
    if (limitX.second >= limitModelX.second)
        limitModelX.second = limitX.second;

    if (limitY.first <= limitModelY.first)
        limitModelY.first = limitY.first;
    if (limitY.second >= limitModelY.second)
        limitModelY.second = limitY.second;

    if (limitZ.first <= limitModelZ.first)
        limitModelZ.first = limitZ.first;
    if (limitZ.second >= limitModelZ.second)
        limitModelZ.second = limitZ.second;

    dimension.x = limitModelX.second - limitModelX.first;
    dimension.y = limitModelY.second - limitModelY.first;
    dimension.z = limitModelZ.second - limitModelZ.first;

    meshResult.dimensions.x = limitX.second - limitX.first;
    meshResult.dimensions.y = limitY.second - limitY.first;
    meshResult.dimensions.z = limitZ.second - limitZ.first;

    if (HasBones)
        getBoneWeight4Vertices(meshResult.vertex, mesh);
}

ResourceTexture *ResourceModel::loadMaterialTextures(aiMaterial *mat, aiTextureType type, TextureType textureType)
{
    aiString str;
    ResourceTexture *texture{nullptr};
    if (mat->GetTextureCount(type) > 0)
    {
        if (AI_FAILURE != mat->GetTexture(type, 0, &str))
        {
            texture = this->resourceManager->getTexture(str.C_Str());
            texture->type = textureType;
            return texture;
        }
    }
    return nullptr;
}

void ResourceModel::setVertexBoneToDefault(Vertex &vertex)
{
    for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
    {
        vertex.BoneIDs[i] = -1;
        vertex.Weight[i] = 0.0f;
    }
}

void ResourceModel::setVertexBone(Vertex &vertex, int const &boneID, float const &weight)
{
    for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
    {
        if (vertex.BoneIDs[i] < 0)
        {
            vertex.BoneIDs[i] = boneID;
            vertex.Weight[i] = weight;
            break;
        }
    }
}

void ResourceModel::getBoneWeight4Vertices(std::vector<Vertex> &vertices, aiMesh *mesh)
{

    for (unsigned int boneIndex = 0; boneIndex < mesh->mNumBones; boneIndex++)
    {
        int boneID = -1;
        std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();

        auto itr = boneInfoMap.find(boneName);
        // If doesnt exist
        if (itr == boneInfoMap.end())
        {
            BoneInfo boneInfo;

            boneInfo.id = boneCounter;
            boneInfo.offset = AssimpGLMHelper::aiMatrixToGLM(mesh->mBones[boneIndex]->mOffsetMatrix);
            boneInfoMap.emplace(boneName, boneInfo);
            boneID = boneCounter;

            boneCounter++;
        }
        else
        {
            boneID = itr->second.id;
        }
        assert(boneID != -1);
        auto weights = mesh->mBones[boneIndex]->mWeights;
        int numWeights = int(mesh->mBones[boneIndex]->mNumWeights);

        for (int weightIndex = 0; weightIndex < numWeights; weightIndex++)
        {
            std::size_t vertexIndex = weights[weightIndex].mVertexId;
            float weight = weights[weightIndex].mWeight;
            assert(vertexIndex <= vertices.size());
            setVertexBone(vertices[vertexIndex], boneID, weight);
        }
    }
}

void ResourceModel::setTexture(ResourceTexture *path)
{
    for (auto &mesh : meshes)
        mesh->setTexture(path);
}

void ResourceModel::createCube(glm::vec3 const &size)
{
    loadFile("media/models/Elements/cube.obj");
    for (auto &mesh : meshes)
    {
        for (auto &vertex : mesh->vertex)
        {
            vertex.Position *= size;
            vertex.TexCoord.x *= size.x;
            vertex.TexCoord.y *= size.y;
            // vertex.Normal *= size;
            // std::cout << "\n Vertices: (" << vertex.Position.x << ", " << vertex.Position.y << ", " << vertex.Position.z << ")";
        }
        mesh->drop();
        mesh->setup();
    }
}

void ResourceModel::addLightToRender(LightParams &light)
{
    for (auto &mesh : meshes)
        mesh->addLightToRender(light);
}

void ResourceModel::clearLightsToRender()
{
    for (auto &mesh : meshes)
        mesh->lights.clear();
}