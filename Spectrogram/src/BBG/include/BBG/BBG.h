#pragma once
#include <cstdint>

#include <glm/vec3.hpp>
#include <glad/glad.h>

#include <BBG/private/NonCopyable.h>

namespace BBG
{
    enum class AddressMode
    {
        ClampToEdge = GL_CLAMP_TO_EDGE,
        ClampToBorder = GL_CLAMP_TO_BORDER,
        Repeat = GL_REPEAT,
    };

    enum class FilterMode
    {
        Nearest = GL_NEAREST,
        Linear = GL_LINEAR,
    };

    enum class UploadPixelType
    {
        Float = GL_FLOAT,
    };

    enum class UploadPixelFormat
    {
        R = GL_RED,
        Rgba = GL_RGBA,
    };

    enum class InternalPixelFormat
    {
        R32_Float = GL_R32F,
        R8G8B8A8_Unorm = GL_RGBA8,
    };

    enum class ImageFormat
    {
        R8G8B8A8_Unorm = (int)InternalPixelFormat::R8G8B8A8_Unorm,
    };

    enum class Topology
    {
        LineStrip = GL_LINE_STRIP,
        Triangles = GL_TRIANGLES,
    };

    enum class ShaderType
    {
        Vertex = GL_VERTEX_SHADER,
        Fragment = GL_FRAGMENT_SHADER,
        Compute = GL_COMPUTE_SHADER,
    };

    enum class MemLocation
    {
        // The buffer resides in DEVICE memory
        DeviceLocal = 0,

        // The buffer resides in HOST memory
        HostLocal = GL_CLIENT_STORAGE_BIT,
    };

    enum class MemAccess
    {
        /// <summary>
        /// The buffer can not be written to from the HOST except at the time of creation.
        /// It can be read by using the Download functions.
        /// </summary>
        None = 0,

        /// <summary>
        /// The buffer must be written or read by to using the Upload/Download functions.
        /// Synchronization is taken care of by OpenGL.
        /// </summary>
        Synced = GL_DYNAMIC_STORAGE_BIT,
    };

    enum class MemoryBarrierFlags
    {
        None = GL_NONE,
        TextureFetch = GL_TEXTURE_FETCH_BARRIER_BIT,
        ShaderStorage = GL_SHADER_STORAGE_BARRIER_BIT,
    };

    struct DeviceCapabilities
    {
        uint32_t countMaxFramebufferDrawBuffers;
        uint32_t countMaxColorAttachments;
        uint32_t countMaxViewports;
        glm::uvec3 countMaxWorkGroups;
    };

    struct DeviceInfo
    {
        const char* gpuName;

        DeviceCapabilities capabilities;
    };

    void Initialize();

    const DeviceInfo& GetDeviceInfo();
}
