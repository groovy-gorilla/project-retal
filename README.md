# Indigo Engine

Indigo Engine is a custom 3D graphics engine written from scratch in C++ and powered by the Vulkan API. The project is being developed as the technological foundation for a modern remake of TFX (1993), one of the most influential combat flight simulators of the golden age of PC gaming.

The engine focuses on modern rendering techniques while maintaining the spirit of classic polygon-based 3D graphics that defined flight simulators of the 1990s. Indigo Engine aims to combine contemporary visual quality, performance, and scalability with the gameplay depth and atmosphere that made the genre memorable.

Project Goals

- Build a fully custom rendering engine without relying on existing game engine frameworks.
- Develop a modern Vulkan-based rendering pipeline from the ground up.
- Create a solid foundation for a next-generation remake of TFX.
- Preserve the visual identity and gameplay philosophy of classic combat flight simulators.
- Maintain a clean, modular, and maintainable codebase suitable for long-term development.

Current Features

Rendering

- Vulkan-based renderer
- High Dynamic Range (HDR) rendering pipeline
- Multi-pass rendering architecture
- Dynamic viewport and scissor states
- Fullscreen post-processing framework
- Custom render target system
- Custom graphics pipeline abstraction

Anti-Aliasing

- MSAA (Multisample Anti-Aliasing)
- SSAA (Supersample Anti-Aliasing)
- SMAA (Subpixel Morphological Anti-Aliasing)
- Combined anti-aliasing modes

Graphics Features

- Polygonal 3D rendering
- HDR color pipeline
- Post-processing framework
- Multiple render passes
- Texture and descriptor management
- Swapchain recreation and dynamic resizing support

Technology

- Language: C++
- Graphics API: Vulkan
- Platform: Linux, Windows
- Build System: CMake
- Windowing/Input: SDL

Vision

Indigo Engine is inspired by the golden era of military flight simulators from the 1990s. The long-term objective is to provide a modern technical foundation for recreating the experience of classic titles while taking advantage of current graphics hardware and rendering techniques.

The project emphasizes low-level control, performance, and a deep understanding of graphics programming, with the majority of systems implemented directly on top of Vulkan rather than relying on external engine frameworks.


## Status

Work in progress.