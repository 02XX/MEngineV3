#include "MModelManager.hpp"

namespace MEngine::Core::Manager
{

std::shared_ptr<MModel> MModelManager::Create(const MModelSetting &setting)
{
    return std::make_shared<MModel>(mUUIDGenerator->Create(), setting);
}

std::shared_ptr<MModel> MModelManager::CreateCube()
{
    // Cube vertices and indices
    // 立方体顶点数据（每个面4个顶点，共24个顶点）
    const std::vector<Vertex> vertices = {
        // 前面 (Z+)
        {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}}, // 0
        {{0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},  // 1
        {{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},   // 2
        {{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},  // 3

        // 后面 (Z-)
        {{0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}},  // 4
        {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}}, // 5
        {{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}},  // 6
        {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}},   // 7

        // 左面 (X-)
        {{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}}, // 8
        {{-0.5f, -0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},  // 9
        {{-0.5f, 0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},   // 10
        {{-0.5f, 0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},  // 11

        // 右面 (X+)
        {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},  // 12
        {{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}}, // 13
        {{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},  // 14
        {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},   // 15

        // 顶面 (Y+)
        {{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},  // 16
        {{0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},   // 17
        {{0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},  // 18
        {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}}, // 19

        // 底面 (Y-)
        {{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}}, // 20
        {{0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}},  // 21
        {{0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}},   // 22
        {{-0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}},  // 23
    };

    // 索引数据（每个面2个三角形，共36个索引，全部为逆时针顺序）
    const std::vector<uint32_t> indices = {// 前面 (Z+) 逆时针：0→1→2→3
                                           // 前面
                                           0, 1, 2, 0, 2, 3,
                                           // 后面
                                           4, 5, 6, 4, 6, 7,
                                           // 左面
                                           8, 9, 10, 8, 10, 11,
                                           // 右面
                                           12, 13, 14, 12, 14, 15,
                                           // 顶面
                                           16, 17, 18, 16, 18, 19,
                                           // 底面
                                           20, 21, 22, 20, 22, 23};
    auto mesh = mMeshManager->Create({});
    mMeshManager->Write(mesh, vertices, indices);
    auto pbrMaterial = mMaterialManager->Create({});
    auto model = std::make_shared<MModel>(MModel(mUUIDGenerator->Create(), {}));
    model->mMeshes.emplace_back(mesh);
    model->mMaterials.emplace_back(pbrMaterial);
    Node rootNode;
    rootNode.Children = {0}; // 只有一个子节点，即第一个网格
    rootNode.MeshIndex = 0;  // 第一个网格的索引
    model->mRootNode = rootNode;
    return model;
}
} // namespace MEngine::Core::Manager