# Debug flags
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")

# AddressSanitizer (tylko dla Debug)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address -fno-omit-frame-pointer")
set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fsanitize=address")

cmake_minimum_required(VERSION 3.30)
project(IndigoEngine)

set(CMAKE_CXX_STANDARD 17)

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
        Engine/Core/Error/ErrorDialog.h
        Engine/Core/Error/ErrorDialog.cpp
        Engine/Core/Application.h
        Engine/Core/Application.cpp
        Engine/Core/Window.h
        Engine/Core/Window.cpp
        Engine/Core/Display.h
        Engine/Core/Display.cpp
        Graphics/Vulkan/Core/VulkanSurface.h
        Graphics/Vulkan/Core/VulkanSurface.cpp
        Graphics/Vulkan/Core/VulkanInstance.h
        Graphics/Vulkan/Core/VulkanInstance.cpp
        Graphics/Vulkan/Core/VulkanPhysicalDevice.h
        Graphics/Vulkan/Core/VulkanPhysicalDevice.cpp
        Graphics/Vulkan/Core/VulkanDevice.h
        Graphics/Vulkan/Core/VulkanDevice.cpp
        Graphics/Vulkan/Core/VulkanSwapchain.h
        Graphics/Vulkan/Core/VulkanSwapchain.cpp
        Graphics/Vulkan/SceneRendering/VulkanSceneRenderPass.h
        Graphics/Vulkan/SceneRendering/VulkanSceneRenderPass.cpp
        Graphics/Vulkan/VulkanRenderer.h
        Graphics/Vulkan/VulkanRenderer.cpp
        Graphics/Graphics.h
        Graphics/Graphics.cpp
        Graphics/Vulkan/Core/VulkanDebug.h
        Graphics/Vulkan/Core/VulkanDebug.cpp
        Graphics/Vulkan/SceneRendering/VulkanScenePipeline.h
        Graphics/Vulkan/SceneRendering/VulkanScenePipeline.cpp
        Engine/Core/ApplicationDesc.h
        Graphics/Vulkan/SceneRendering/VulkanSceneResources.h
        Graphics/Vulkan/SceneRendering/VulkanSceneResources.cpp
        Graphics/Vulkan/Utils/VulkanUtils.h
        Graphics/Vulkan/Utils/VulkanUtils.cpp
        Graphics/Vulkan/PostRendering/VulkanPostRenderPass.cpp
        Graphics/Vulkan/PostRendering/VulkanPostRenderPass.h
        Graphics/Vulkan/Core/VulkanSampler.h
        Graphics/Vulkan/Core/VulkanSampler.cpp
        Graphics/Vulkan/Core/VulkanValidation.h
        Graphics/Vulkan/Core/VulkanValidation.cpp
        Graphics/Vulkan/Resources/RenderTarget.h
        Graphics/Vulkan/PostRendering/VulkanPostResources.h
        Graphics/Vulkan/PostRendering/VulkanPostResources.cpp
        Graphics/Vulkan/Core/VulkanCommands.h
        Graphics/Vulkan/Core/VulkanCommands.cpp
        Graphics/Vulkan/Core/VulkanSync.h
        Graphics/Vulkan/Core/VulkanSync.cpp
        Graphics/Vulkan/Core/VulkanQueues.h
        Graphics/Vulkan/Core/VulkanQueues.cpp
)


# include
target_include_directories(IndigoEngine PRIVATE

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

# compile flags
target_compile_options(IndigoEngine PRIVATE -g -O0)


