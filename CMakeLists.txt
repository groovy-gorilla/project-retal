# Debug flags
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")

# AddressSanitizer (tylko dla Debug)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address -fno-omit-frame-pointer")
set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fsanitize=address")

cmake_minimum_required(VERSION 3.30)
project(IndigoEngine)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(EXECUTABLE_NAME IndigoEngine)

find_package(Vulkan REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(SDL3 REQUIRED)

add_executable(IndigoEngine main.cpp
        Engine/Input/Input.h
        Engine/Input/Input.cpp
        Engine/Platform/SDL/SDLInput.h
        Engine/Platform/SDL/SDLInput.cpp
        Engine/Input/KeyCodes.h
        Engine/Input/InputMapping.h
        Engine/Input/InputMapping.cpp
        Engine/Debug/ErrorDialog.h
        Engine/Debug/ErrorDialog.cpp
        Engine/Core/Application.h
        Engine/Core/Application.cpp
        Engine/Core/Window.h
        Engine/Core/Window.cpp
        Engine/Core/Display.h
        Engine/Core/Display.cpp
        Engine/Graphics/Vulkan/Core/VulkanSurface.h
        Engine/Graphics/Vulkan/Core/VulkanSurface.cpp
        Engine/Graphics/Vulkan/Core/VulkanInstance.h
        Engine/Graphics/Vulkan/Core/VulkanInstance.cpp
        Engine/Graphics/Vulkan/Core/VulkanPhysicalDevice.h
        Engine/Graphics/Vulkan/Core/VulkanPhysicalDevice.cpp
        Engine/Graphics/Vulkan/Core/VulkanDevice.h
        Engine/Graphics/Vulkan/Core/VulkanDevice.cpp
        Engine/Graphics/Vulkan/Core/VulkanSwapchain.h
        Engine/Graphics/Vulkan/Core/VulkanSwapchain.cpp
        Engine/Graphics/Vulkan/SceneRenderPass/VulkanSceneRenderPass.h
        Engine/Graphics/Vulkan/SceneRenderPass/VulkanSceneRenderPass.cpp
        Engine/Graphics/Vulkan/VulkanRenderer.h
        Engine/Graphics/Vulkan/VulkanRenderer.cpp
        Engine/Graphics/Graphics.h
        Engine/Graphics/Graphics.cpp
        Engine/Graphics/Vulkan/Core/VulkanDebug.h
        Engine/Graphics/Vulkan/Core/VulkanDebug.cpp
        Engine/Core/ApplicationDesc.h
        Engine/Graphics/Vulkan/Utils/VulkanUtils.h
        Engine/Graphics/Vulkan/Utils/VulkanUtils.cpp
        Engine/Graphics/Vulkan/PostProcessRenderPass/VulkanPostRenderPass.cpp
        Engine/Graphics/Vulkan/PostProcessRenderPass/VulkanPostRenderPass.h
        Engine/Graphics/Vulkan/Core/VulkanValidation.h
        Engine/Graphics/Vulkan/Core/VulkanValidation.cpp
        Engine/Graphics/Vulkan/Wrappers/RenderTarget.h
        Engine/Graphics/Vulkan/Core/VulkanCommands.h
        Engine/Graphics/Vulkan/Core/VulkanCommands.cpp
        Engine/Graphics/Vulkan/Core/VulkanSync.h
        Engine/Graphics/Vulkan/Core/VulkanSync.cpp
        Engine/Graphics/Vulkan/Core/VulkanQueues.h
        Engine/Graphics/Vulkan/Core/VulkanQueues.cpp
        Engine/Graphics/Vulkan/Wrappers/Descriptor.h
        Engine/Graphics/Vulkan/Wrappers/Descriptor.cpp
        Engine/Graphics/Vulkan/SSAARenderPass/VulkanSSAARenderPass.h
        Engine/Graphics/Vulkan/SSAARenderPass/VulkanSSAARenderPass.cpp
        ThirdParty/stb/stb_image_write.h
        Engine/pch.h
        Engine/pch.cpp
        Engine/Graphics/Vulkan/SMAARenderPass/VulkanSMAARenderPass.cpp
        Engine/Graphics/Vulkan/SMAARenderPass/VulkanSMAARenderPass.h
        Engine/Graphics/Vulkan/Wrappers/RenderTarget.cpp
        Engine/Core/Timer.h
        Engine/Core/Timer.cpp
        Engine/Graphics/Texture/Texture.h
        Engine/Graphics/Texture/Texture.cpp
        Engine/Math/Lina64.h
        Engine/Graphics/Vulkan/Wrappers/Pipeline.h
        Engine/Graphics/Vulkan/Wrappers/Pipeline.cpp
)


# include
target_include_directories(IndigoEngine PRIVATE
    ${CMAKE_SOURCE_DIR}/Engine
    ${CMAKE_SOURCE_DIR}/
)

# link dirs
target_link_directories(IndigoEngine PRIVATE

)

# link libs
target_link_libraries(IndigoEngine PRIVATE
        Vulkan::Vulkan
        SDL3::SDL3
        ktx
        udev
)

target_precompile_headers(IndigoEngine PRIVATE
    Engine/pch.h
)

# compile flags
target_compile_options(IndigoEngine PRIVATE -g -O0 -mavx -mfma)


