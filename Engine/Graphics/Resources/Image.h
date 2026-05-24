#pragma once
#include "Texture.h"
#include "Math/Lina64.h"

using namespace lina;

class Image {
public:
    void Create(
        const VulkanContext& context,
        const std::filesystem::path& path);

    void Shutdown();

    void SetPosition(const fvec2& position);
    void SetSize(const fvec2& size);

    const fvec2& GetPosition() const;
    const fvec2& GetSize() const;

    [[nodiscard]] const Texture& GetTexture() const { return m_texture; }
    [[nodiscard]] VkDescriptorSet GetDescriptorSet() const;


private:
    Texture m_texture;
    fvec2 m_position;
    fvec2 m_size;

};
