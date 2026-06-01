#pragma once
#include "../Texture/Texture.h"
#include "Math/Lina64.h"

using namespace lina;

class Sprite {
public:
    void Create(
        VulkanContext context,
        const std::filesystem::path& path,
        uint32_t framesInFlight);

    void Shutdown();

    void SetPosition(uint32_t width, uint32_t height);
    void SetSize(uint32_t width, uint32_t height);

    const fvec2& GetPosition() const;
    const fvec2& GetSize() const;

    [[nodiscard]] const Texture& GetTexture() const { return m_texture; }
    [[nodiscard]] VkDescriptorSet GetDescriptorSet(uint32_t frameIndex) const { return m_descriptor.GetSet(frameIndex); }


private:
    Texture m_texture;
    Descriptor m_descriptor;
    fvec2 m_position;
    fvec2 m_size;

};
