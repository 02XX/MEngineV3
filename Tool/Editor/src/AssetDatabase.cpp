#include "AssetDatabase.hpp"
#include "Logger.hpp"
#include "MAsset.hpp"
#include "MFolder.hpp"

#include "MMesh.hpp"
#include "MMeshManager.hpp"
#include "MModel.hpp"
#include "MPBRMaterial.hpp"
#include "MPipeline.hpp"
#include "MTexture.hpp"
#include "Reflect.hpp"
#include "Vertex.hpp"
#include <MModel.hpp>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <filesystem>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace MEngine::Editor
{
std::shared_ptr<MFolder> AssetDatabase::CreateFolder(const std::filesystem::path &parentPath,
                                                     const std::string &newFolderName, const MFolderSetting &setting)
{
    auto folder = CreateAsset<MFolder>(parentPath, newFolderName, setting);

    if (!parentPath.empty() && mPath2UUID.contains(parentPath))
    {
        auto parentFolder = GetAsset<MFolder>(parentPath);
        if (parentFolder)
        {
            folder->SetParentFolder(parentFolder);
        }
    }
    return folder;
}
void AssetDatabase::SaveFolder(std::shared_ptr<MFolder> folder)
{
    std::filesystem::create_directories(folder->GetPath());
    mPath2UUID[folder->GetPath()] = folder->GetID();
}
void AssetDatabase::DeleteFolder(const std::filesystem::path &path)
{
    if (auto folder = GetAsset<MFolder>(path))
    {
        folder->GetParentFolder()->RemoveChild(folder);
    }
    DeleteAsset(path);
}

void AssetDatabase::UpdateAsset(const std::filesystem::path &path)
{
    if (mPath2UUID.contains(path))
    {
        mResourceManager->UpdateAsset(mPath2UUID[path]);
    }
    else
    {
        LogError("Path {} not found in AssetDatabase.", path.string());
    }
}
void AssetDatabase::DeleteAsset(const std::filesystem::path &path)
{
    if (mPath2UUID.contains(path))
    {
        mResourceManager->DeleteAsset(mPath2UUID[path]);
        mPath2UUID.erase(path);
    }
    else
    {
        LogError("Path {} not found in AssetDatabase.", path.string());
    }
}
void AssetDatabase::MoveAsset(const std::filesystem::path &srcPath, const std::filesystem::path &dstPath)
{
}
void AssetDatabase::RenameAsset(const std::filesystem::path &path, const std::string &newName)
{
}
void AssetDatabase::CopyAsset(const std::filesystem::path &srcPath, const std::filesystem::path &dstPath)
{
}
std::shared_ptr<MAsset> AssetDatabase::LoadAsset(const std::filesystem::path &path)
{
    if (std::filesystem::is_directory(path))
    {
        return LoadFolder(path);
    }
    if (!std::filesystem::exists(path))
    {
        LogError("Asset file {} does not exist.", path.string());
        return nullptr;
    }
    std::ifstream file(path);
    if (!file.is_open())
    {
        LogError("Failed to open asset file {}.", path.string());
        return nullptr;
    }
    json j;
    file >> j;
    file.close();
    auto assetTypeStr = j["type"].get<std::string>();
    auto assetType = magic_enum::enum_cast<Core::Asset::MAssetType>(assetTypeStr).value();
    std::shared_ptr<MAsset> asset{};
    switch (assetType)
    {
    case Core::Asset::MAssetType::Texture: {
        auto textureSetting = MTextureSetting{};
        j["setting"].get_to(textureSetting);
        asset = CreateAsset<MTexture, MTextureSetting>(path.parent_path(), path.filename().string(), textureSetting);
        j.get_to<MTexture>(*std::static_pointer_cast<MTexture>(asset));
        break;
    }
    case Core::Asset::MAssetType::Material:
    case Core::Asset::MAssetType::Mesh:
    case Core::Asset::MAssetType::Shader: {
        auto pipelineSetting = MPipelineSetting{};
        j["setting"].get_to(pipelineSetting);
        asset = CreateAsset<MPipeline, MPipelineSetting>(path.parent_path(), path.filename().string(), pipelineSetting);
        j.get_to(*std::static_pointer_cast<MPipeline>(asset));
        break;
    }
    case Core::Asset::MAssetType::Animation:
    case Core::Asset::MAssetType::Scene:
    case Core::Asset::MAssetType::Audio:
    case Core::Asset::MAssetType::Font:
    case Core::Asset::MAssetType::Folder:
        break;
    case Core::Asset::MAssetType::File:
    case Core::Asset::MAssetType::Script:
    case Core::Asset::MAssetType::Unknown:
        break;
    }
    mPath2UUID[path] = asset->GetID();
    return asset;
}
std::shared_ptr<MFolder> AssetDatabase::LoadFolder(const std::filesystem::path &directory)
{
    if (!std::filesystem::exists(directory) || !std::filesystem::is_directory(directory))
    {
        LogError("Directory {} does not exist or is not a directory.", directory.string());
        return nullptr;
    }
    auto folder = CreateFolder(directory.parent_path(), directory.filename().string(), {});
    mPath2UUID[directory] = folder->GetID();
    return folder;
}
std::shared_ptr<MFolder> AssetDatabase::LoadDatabase(const std::filesystem::path &directory,
                                                     const std::filesystem::path &parentDirectory)
{
    std::shared_ptr<MFolder> rootFolder{};
    for (auto &entry : std::filesystem::directory_iterator(directory))
    {
        if (entry.is_directory())
        {
            auto folder = LoadFolder(entry.path());
            rootFolder = folder;
            LoadDatabase(entry.path(), folder->GetPath());
        }
        else if (entry.is_regular_file())
        {
            auto asset = LoadAsset(entry.path());
        }
    }
    return rootFolder;
}
std::filesystem::path AssetDatabase::GenerateUniqueAssetPath(std::filesystem::path path)
{
    if (!mPath2UUID.contains(path))
    {
        return path;
    }
    const auto stem = path.stem();
    std::string extension = "";
    if (!std::filesystem::is_directory(path))
    {
        extension = path.extension().string();
    }
    auto parentPath = path.parent_path();
    int counter = 1;
    std::filesystem::path newPath;
    do
    {
        newPath = parentPath / (stem.string() + " (" + std::to_string(counter) + ")" + extension);
        ++counter;
    } while (std::filesystem::exists(newPath));
    return newPath;
}
std::shared_ptr<MModel> AssetDatabase::LoadFBX(const std::filesystem::path &path)
{
    Assimp::Importer importer;
    auto scene = importer.ReadFile(path.string().c_str(), aiProcess_GenUVCoords | aiProcess_ForceGenNormals |
                                                              aiProcess_Triangulate | aiProcess_CalcTangentSpace |
                                                              aiProcess_FlipUVs);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        LogError("Failed to load FBX file {}: {}", path.string(), importer.GetErrorString());
        throw std::runtime_error("Load fbx file failed.");
    }
    auto sceneName = scene->mRootNode->mName.C_Str();
    auto meshManager = mResourceManager->GetManager<MMesh, MMeshSetting, IMMeshManager>();
    auto materialManager = mResourceManager->GetManager<MPBRMaterial, MPBRMaterialSetting, IMPBRMaterialManager>();
    auto model = mResourceManager->CreateAsset<MModel, MModelSetting>({});
    std::vector<std::shared_ptr<MMesh>> modelMeshes{};
    std::vector<std::shared_ptr<MMaterial>> modelMaterials{};
    std::function<std::unique_ptr<Node>(const aiNode *, Node *parent)> processNode;
    processNode = [&](const aiNode *node, Node *parent) {
        auto modelNode = std::make_unique<Node>();
        auto name = node->mName.C_Str();
        auto transform = node->mTransformation;
        modelNode->Name = name ? name : "Unnamed Node";
        // modelNode->Transform =
        //     glm::mat4(transform.a1, transform.b1, transform.c1, transform.d1, transform.a2, transform.b2,
        //     transform.c2,
        //               transform.d2, transform.a3, transform.b3, transform.c3, transform.d3, transform.a4,
        //               transform.b4, transform.c4, transform.d4);
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            std::vector<Vertex> vertices;
            std::vector<uint32_t> indices;
            for (unsigned int i = 0; i < mesh->mNumVertices; i++)
            {
                Vertex vertex{};
                // Position
                vertex.position.x = mesh->mVertices[i].x;
                vertex.position.y = mesh->mVertices[i].y;
                vertex.position.z = mesh->mVertices[i].z;

                // Normal
                if (mesh->HasNormals())
                {
                    vertex.normal.x = mesh->mNormals[i].x;
                    vertex.normal.y = mesh->mNormals[i].y;
                    vertex.normal.z = mesh->mNormals[i].z;
                }

                // TexCoord
                if (mesh->mTextureCoords[0])
                {
                    vertex.texCoords.x = mesh->mTextureCoords[0][i].x;
                    vertex.texCoords.y = mesh->mTextureCoords[0][i].y;
                }
                vertices.push_back(vertex);
            }
            for (unsigned int i = 0; i < mesh->mNumFaces; i++)
            {
                aiFace face = mesh->mFaces[i];
                for (unsigned int j = 0; j < face.mNumIndices; j++)
                {
                    indices.push_back(face.mIndices[j]);
                }
            }
            if (mesh->mMaterialIndex >= 0)
            {
                aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
            }
            MMeshSetting meshSetting{};
            meshSetting.vertexBufferSize = vertices.size() * sizeof(Vertex);
            meshSetting.indexBufferSize = indices.size() * sizeof(uint32_t);
            modelNode->MeshIndex = static_cast<int>(modelMeshes.size());
            modelNode->MaterialIndex = static_cast<int>(modelMaterials.size());
            modelNode->Parent = parent;
            auto modelMesh = mResourceManager->CreateAsset<MMesh, MMeshSetting>(meshSetting);
            modelMesh->SetVertices(vertices);
            modelMesh->SetIndices(indices);
            meshManager->CreateVulkanResources(modelMesh);
            meshManager->Write(modelMesh, vertices, indices);
            modelMeshes.push_back(modelMesh);
            auto defaultMaterial = materialManager->CreateDefaultMaterial();
            modelMaterials.push_back(defaultMaterial);
        }
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            auto child = processNode(node->mChildren[i], modelNode.get());
            modelNode->Children.push_back(std::move(child));
        }
        return modelNode;
    };
    auto rootNoe = processNode(scene->mRootNode, nullptr);
    model->SetNode(std::move(rootNoe));
    model->SetMeshes(modelMeshes);
    model->SetMaterials(modelMaterials);
    return model;
}
std::shared_ptr<MTexture> AssetDatabase::LoadImage(const std::filesystem::path &path)
{
}
} // namespace MEngine::Editor