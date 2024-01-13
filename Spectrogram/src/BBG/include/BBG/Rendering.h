#pragma once
#include <span>

#include <glm/vec3.hpp>

#include <BBG/BBG.h>
#include <BBG/Buffer.h>
#include <BBG/Sampler.h>
#include <BBG/Texture2D.h>
#include <BBG/ShaderProgram.h>

namespace BBG::Rendering
{
    constexpr uint64_t BIND_WHOLE_BUFFER = UINT64_MAX;

    enum class AttachmentLoadOp
    {
        // On tiler GPUs this tells the driver it does not have to load the image into tile memory before rendering. 
        // That means tile memory will start out undefined as it is assumed nothing reads from it. Elsewhere this has no obvious benefits.
        DontCare,

        // Clears the image. On tiler GPUs this might or might not have the same cost as DontCare flag.
        // On Mali for example image Clear work directly on tile memory and are free (as effiecent as DontCare).
        Clear,

        // On tiler GPUs this corresponds to a full screen copy from image memory to tile memory which is bad. Elsewhere this has no effect.
        Load,
    };

    struct ColorAttachment
    {
        const Texture2D& texture;
        std::array<float, 4> clearColor;
        AttachmentLoadOp loadOp;
        uint32_t level = 0;
    };

    struct ScreenColorAttachment
    {
        std::array<float, 4> clearColor;
    };

    void SetColorAttachments(std::span<ColorAttachment> colorAttachments);
    void SetColorAttachments(ScreenColorAttachment colorAttachment);

    void BindShaderProgram(const ShaderProgram& shaderProgram);

    void BindTexture(const BBG::Texture2D& texture, const BBG::Sampler& sampler, uint32_t unit);
    void BindImage(const BBG::Texture2D& texture, ImageFormat format, uint32_t unit, uint32_t level = 0, uint32_t layer = 0, bool layered = false);

    void DrawNonIndexed(Topology topology, uint32_t first, uint32_t count, uint32_t instanceCount, uint32_t baseInstance);
    void DispatchThreads(glm::uvec3 threads, glm::uvec3 workgroupSize);

    void MemoryBarrier(BBG::MemoryBarrierFlags memoryBarrier);

    void SetShaderStorageBlock(const Buffer& buffer, uint32_t index, uint64_t offset = 0, uint64_t size = BIND_WHOLE_BUFFER);
    void SetUniformBlock(const Buffer& buffer, uint32_t index, uint64_t offset = 0, uint64_t size = BIND_WHOLE_BUFFER);

    void SetViewport(glm::vec2 size, uint32_t viewportIndex = 0);
    void SetViewport(glm::vec2 size, glm::vec2 offset, uint32_t viewportIndex = 0);
}