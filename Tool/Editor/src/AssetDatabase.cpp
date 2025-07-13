#include "AssetDatabase.hpp"
#include "ImageUtil.hpp"
#include "Logger.hpp"
#include "MAsset.hpp"
#include "MFolder.hpp"
#include "MMesh.hpp"
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
#include <vulkan/vulkan_enums.hpp>

namespace MEngine::Editor
{
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
void AssetDatabase::MoveAsset(std::shared_ptr<MAsset> asset, std::shared_ptr<MFolder> dstFolder)
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
    if (!std::filesystem::exists(path))
    {
        LogError("Asset file {} does not exist.", path.string());
        return nullptr;
    }
    std::shared_ptr<MAsset> asset{};
    auto folderManager = mResourceManager->GetManager<MFolder, IMFolderManager>();
    if (std::filesystem::is_directory(path))
    {
        auto folderSetting = MFolderSetting{};
        auto folder = folderManager->Create(path.filename().stem().string(), folderSetting);
        if (mPath2UUID.contains(path.parent_path()))
        {
            auto parentFolder = folderManager->Get(mPath2UUID.at(path.parent_path()));
            folder->SetParentFolder(parentFolder);
        }
        asset = folder;
    }
    else
    {
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open())
        {
            LogError("Failed to open asset file {}.", path.string());
            return nullptr;
        }
        json j = json::from_msgpack(file);
        file.close();
        auto assetTypeStr = j["type"].get<std::string>();
        auto assetType = magic_enum::enum_cast<Core::Asset::MAssetType>(assetTypeStr).value();
        switch (assetType)
        {
        case Core::Asset::MAssetType::Texture: {
            auto textureManager = mResourceManager->GetManager<MTexture, IMTextureManager>();
            auto textureSetting = MTextureSetting{};
            j["setting"].get_to(textureSetting);
            auto texture =
                textureManager->Create("New Texture", {1, 1, 4}, textureManager->GetWhiteData(), textureSetting);
            textureManager->Remove(texture->GetID());
            j.get_to<MTexture>(*texture);
            textureManager->Update(texture);
            textureManager->CreateVulkanResources(texture);
            if (!texture->GetImageData().empty())
            {
                textureManager->Write(texture);
            }
            asset = texture;
            break;
        }
        case Core::Asset::MAssetType::Material: {
            auto materialTypeStr = j["materialType"].get<std::string>();
            auto materialType = magic_enum::enum_cast<Core::Asset::MMaterialType>(materialTypeStr).value();
            switch (materialType)
            {
            case Core::Asset::MMaterialType::Unknown:
            case Core::Asset::MMaterialType::PBR: {
                auto pbrMaterialManager = mResourceManager->GetManager<MPBRMaterial, IMPBRMaterialManager>();
                auto pbrMaterialSetting = MPBRMaterialSetting{};
                auto pbrMaterialProperties = MPBRMaterialProperties{};
                auto pbrMaterialTextures = MPBRTextures{};
                j["setting"].get_to(pbrMaterialSetting);
                auto pbrMaterial =
                    pbrMaterialManager->Create("New PBR Material", PipelineType::ForwardOpaquePBR,
                                               pbrMaterialProperties, pbrMaterialTextures, pbrMaterialSetting);
                pbrMaterialManager->Remove(pbrMaterial->GetID());
                j.get_to(*pbrMaterial);
                pbrMaterialManager->Update(pbrMaterial);
                pbrMaterialManager->CreateVulkanResources(pbrMaterial);
                pbrMaterialManager->Write(pbrMaterial);
                asset = pbrMaterial;
                break;
            }
            case Core::Asset::MMaterialType::Unlit:
            case Core::Asset::MMaterialType::Custom:
                break;
            }
            break;
        }
        case Core::Asset::MAssetType::Mesh:
        case Core::Asset::MAssetType::Shader: {
            auto pipelineManager = mResourceManager->GetManager<MPipeline, IMPipelineManager>();
            auto pipelineSetting = MPipelineSetting{};
            j["setting"].get_to(pipelineSetting);
            auto pipeline = pipelineManager->Create("New Pipeline", pipelineSetting);
            pipelineManager->RemoveByName(pipeline->GetName());
            j.get_to(*pipeline);
            pipelineManager->Update(pipeline);
            pipelineManager->CreateVulkanResources(pipeline);
            asset = pipeline;
            break;
        }
        case Core::Asset::MAssetType::Model: {
            auto meshManager = mResourceManager->GetManager<MMesh, IMMeshManager>();
            auto materialManager = mResourceManager->GetManager<MPBRMaterial, IMPBRMaterialManager>();
            auto modelManager = mResourceManager->GetManager<MModel, IMModelManager>();
            auto modelSetting = MModelSetting{};
            // mesh
            for (const auto &meshJson : j["meshes"])
            {
                auto meshSetting = MMeshSetting{};
                auto name = meshJson["name"].get<std::string>();
                auto mesh = meshManager->Create(name, {}, {}, meshSetting);
                meshManager->Remove(mesh->GetID());
                meshJson.get_to(*mesh);
                meshManager->Update(mesh);
                meshManager->CreateVulkanResources(mesh);
                meshManager->Write(mesh);
            }
            // material
            for (const auto &materialJson : j["materials"])
            {
                auto materialTypeStr = materialJson["materialType"].get<std::string>();
                auto materialType = magic_enum::enum_cast<Core::Asset::MMaterialType>(materialTypeStr).value();
                switch (materialType)
                {
                case Core::Asset::MMaterialType::Unknown:
                case Core::Asset::MMaterialType::PBR: {
                    auto pbrMaterialManager = mResourceManager->GetManager<MPBRMaterial, IMPBRMaterialManager>();
                    auto textureManager = mResourceManager->GetManager<MTexture, IMTextureManager>();
                    auto materialSetting = MPBRMaterialSetting{};
                    auto name = materialJson["name"].get<std::string>();
                    auto pbrMaterialProperties = MPBRMaterialProperties{};
                    auto pbrMaterialTextures = MPBRTextures{};
                    auto pbrMaterial =
                        materialManager->Create(name, PipelineType::ForwardOpaquePBR, pbrMaterialProperties,
                                                pbrMaterialTextures, materialSetting);
                    pbrMaterialManager->Remove(pbrMaterial->GetID());
                    materialJson.get_to(*pbrMaterial);
                    pbrMaterialManager->Update(pbrMaterial);
                    materialManager->CreateVulkanResources(pbrMaterial);
                    materialManager->Write(pbrMaterial);
                    break;
                }
                case Core::Asset::MMaterialType::Unlit:
                case Core::Asset::MMaterialType::Custom:
                    break;
                }
            }
            j["setting"].get_to(modelSetting);
            auto model = modelManager->Create("New Model", {}, {}, nullptr, modelSetting);
            modelManager->Remove(model->GetID());
            j.get_to(*model);
            modelManager->Update(model);
            modelManager->CreateVulkanResources(model);
            asset = model;
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
    }
    auto parentFolder = folderManager->Get(mPath2UUID[path.parent_path()]);
    if (parentFolder != nullptr)
    {
        parentFolder->AddChild(asset);
    }
    mPath2UUID[path] = asset->GetID();
    mUUID2Path[asset->GetID()] = path;
    return asset;
}
std::shared_ptr<MFolder> AssetDatabase::LoadDatabase(const std::filesystem::path &directory,
                                                     const std::filesystem::path &parentDirectory)
{
    std::shared_ptr<MFolder> rootFolder{};
    for (auto &entry : std::filesystem::directory_iterator(directory))
    {
        auto asset = LoadAsset(entry.path());
        if (entry.is_directory())
        {
            rootFolder = std::dynamic_pointer_cast<MFolder>(asset);
            LoadDatabase(entry.path(), entry.path());
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
    auto meshManager = mResourceManager->GetManager<MMesh, IMMeshManager>();
    auto materialManager = mResourceManager->GetManager<MPBRMaterial, IMPBRMaterialManager>();
    auto modelManager = mResourceManager->GetManager<MModel, IMModelManager>();
    std::vector<std::shared_ptr<MMesh>> modelMeshes{};
    std::vector<std::shared_ptr<MMaterial>> modelMaterials{};
    std::vector<UUID> meshIDs{};
    std::vector<UUID> materialIDs{};
    std::function<std::unique_ptr<Node>(const aiNode *, Node *parent)> processNode;
    processNode = [&](const aiNode *node, Node *parent) {
        auto modelNode = std::make_unique<Node>();
        auto name = node->mName.C_Str();
        auto transform = node->mTransformation;
        modelNode->Name = name ? name : "Unnamed Node";
        modelNode->Transform =
            glm::mat4(transform.a1, transform.b1, transform.c1, transform.d1, transform.a2, transform.b2, transform.c2,
                      transform.d2, transform.a3, transform.b3, transform.c3, transform.d3, transform.a4, transform.b4,
                      transform.c4, transform.d4);
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
            modelNode->MeshIndex = static_cast<int>(modelMeshes.size());
            modelNode->MaterialIndex = static_cast<int>(modelMaterials.size());
            modelNode->Parent = parent;
            auto modelMesh = meshManager->Create(mesh->mName.C_Str(), vertices, indices, meshSetting);
            meshManager->CreateVulkanResources(modelMesh);
            meshManager->Write(modelMesh);

            modelMeshes.push_back(modelMesh);
            meshIDs.push_back(modelMesh->GetID());
            auto defaultMaterial = materialManager->CreateDefaultForwardOpaquePBRMaterial();
            modelMaterials.push_back(defaultMaterial);
            materialIDs.push_back(defaultMaterial->GetID());
        }
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            auto child = processNode(node->mChildren[i], modelNode.get());
            modelNode->Children.push_back(std::move(child));
        }
        return modelNode;
    };
    auto rootNoe = processNode(scene->mRootNode, nullptr);
    auto modelSetting = MModelSetting{};
    auto model = modelManager->Create(sceneName, meshIDs, materialIDs, std::move(rootNoe), modelSetting);
    return model;
}
std::shared_ptr<MTexture> AssetDatabase::LoadPNG(const std::filesystem::path &path)
{
    if (!std::filesystem::exists(path) || !std::filesystem::is_regular_file(path))
    {
        LogError("PNG file {} does not exist or is not a regular file.", path.string());
        return nullptr;
    }
    auto extension = path.extension().string();
    auto fileName = path.filename().stem().string();
    auto textureManager = mResourceManager->GetManager<MTexture, IMTextureManager>();
    auto &&[width, height, channels, imageData] = MEngine::Core::Utils::ImageUtil::LoadImage(path);
    auto textureSetting = MTextureSetting{};
    textureSetting.isShaderResource = true;
    auto texture = textureManager->Create(
        fileName, {static_cast<uint32_t>(width), static_cast<uint32_t>(height), static_cast<uint32_t>(channels)},
        imageData, textureSetting);
    textureManager->CreateVulkanResources(texture);
    textureManager->Write(texture);
    return texture;
}
} // namespace MEngine::Editor