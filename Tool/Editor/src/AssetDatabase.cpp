#include "AssetDatabase.hpp"
#include "ImageUtil.hpp"
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
#include <vulkan/vulkan_enums.hpp>

namespace MEngine::Editor
{
std::shared_ptr<MFolder> AssetDatabase::CreateFolder(const std::filesystem::path &parentPath,
                                                     const std::string &newFolderName, const MFolderSetting &setting)
{
    auto folderManager = mResourceManager->GetManager<MFolder, IMFolderManager>();
    auto uniquePath = GenerateUniqueAssetPath(parentPath / newFolderName);
    UUID parentID{};
    if (mPath2UUID.contains(uniquePath.parent_path()))
    {
        parentID = mPath2UUID[uniquePath.parent_path()];
    }
    auto parentFolder = folderManager->Get(parentID);
    parentID = parentFolder ? parentFolder->GetID() : UUID{};
    auto folder = folderManager->Create(uniquePath.filename().stem().string(), parentID, {}, setting);
    mPath2UUID[folder->GetPath()] = folder->GetID();
    return folder;
}
void AssetDatabase::SaveFolder(std::shared_ptr<MFolder> folder, const std::filesystem::path &directory)
{
    auto uniquePath = GenerateUniqueAssetPath(directory / folder->GetName());
    std::filesystem::create_directories(uniquePath);
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
    std::shared_ptr<MAsset> asset{};
    switch (assetType)
    {
    case Core::Asset::MAssetType::Texture: {
        auto textureManager = mResourceManager->GetManager<MTexture, IMTextureManager>();
        auto textureSetting = MTextureSetting{};
        j["setting"].get_to(textureSetting);
        auto texture = textureManager->Create("New Texture", {1, 1, 4}, textureManager->GetWhiteData(), textureSetting);
        j.get_to<MTexture>(*texture);
        textureManager->Update(texture);
        textureManager->CreateVulkanResources(texture);
        textureManager->Write(texture);
        asset = texture;
        break;
    }
    case Core::Asset::MAssetType::Material:
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
                // pbrMaterialTextures.AlbedoID =
                // textureManager->GetDefaultTexture(DefaultTextureType::Albedo)->GetID(); pbrMaterialTextures.NormalID
                // = textureManager->GetDefaultTexture(DefaultTextureType::Normal)->GetID(); pbrMaterialTextures.ARMID =
                // textureManager->GetDefaultTexture(DefaultTextureType::ARM)->GetID(); pbrMaterialTextures.EmissiveID =
                //     textureManager->GetDefaultTexture(DefaultTextureType::Emissive)->GetID();
                auto pbrMaterial = materialManager->Create(name, PipelineType::ForwardOpaquePBR, pbrMaterialProperties,
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
    auto folderManager = mResourceManager->GetManager<MFolder, IMFolderManager>();
    auto parentFolder = folderManager->Get(mPath2UUID[path.parent_path()]);
    if (parentFolder != nullptr)
    {
        parentFolder->AddChild(asset);
    }
    asset->SetPath(path);
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
    MFolderSetting folderSetting{};
    auto folder = CreateFolder(directory.parent_path(), directory.filename().string(), folderSetting);
    folder->SetPath(directory);
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
    model->SetPath(std::filesystem::path(path).parent_path() / path.filename().replace_extension(".masset"));
    mPath2UUID[model->GetPath()] = model->GetID();
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
    // stbi_set_flip_vertically_on_load(true);
    // int width, height, channels;
    // auto data = stbi_load(path.string().c_str(), &width, &height, &channels, STBI_rgb_alpha);
    // if (!data)
    // {
    //     LogError("Failed to load PNG image: {}", path.string());
    //     return nullptr;
    // }
    // std::vector<uint8_t> imageData(width * height * 4); // 使用RGBA格式
    // if (channels == 3)
    // { // 如果是RGB，手动转RGBA
    //     for (int i = 0; i < width * height; ++i)
    //     {
    //         imageData[i * 4 + 0] = data[i * 3 + 0]; // R
    //         imageData[i * 4 + 1] = data[i * 3 + 1]; // G
    //         imageData[i * 4 + 2] = data[i * 3 + 2]; // B
    //         imageData[i * 4 + 3] = 255;             // A (不透明)
    //     }
    //     channels = 4;
    // }
    auto &&[width, height, channels, imageData] = MEngine::Core::Utils::ImageUtil::LoadImage(path);
    auto textureSetting = MTextureSetting{};
    textureSetting.isShaderResource = true;
    auto texture = textureManager->Create(
        fileName, {static_cast<uint32_t>(width), static_cast<uint32_t>(height), static_cast<uint32_t>(channels)},
        imageData, textureSetting);
    textureManager->CreateVulkanResources(texture);
    textureManager->Write(texture);
    texture->SetPath(path);
    mPath2UUID[path] = texture->GetID();
    return texture;
}
} // namespace MEngine::Editor