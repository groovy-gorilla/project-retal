#pragma once

#include "Graphics/Vulkan/Wrappers/Buffer.h"
#include "Math/Lina64.h"

struct ModelVertex {
    lina::fvec3 position;
    lina::fvec3 color;
};

class Model {
public:

    void CreateCube(
        VkDevice device,
        VkPhysicalDevice physicalDevice
    );

    void Destroy();

    Buffer& GetVertexBuffer() {
        return m_vertexBuffer;
    }

    Buffer& GetIndexBuffer() {
        return m_indexBuffer;
    }

    uint32_t GetIndexCount() const {
        return m_indexCount;
    }

private:

    Buffer m_vertexBuffer;
    Buffer m_indexBuffer;

    uint32_t m_indexCount = 0;
};