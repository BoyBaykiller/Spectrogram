#include <BBG/Rendering.h>

namespace BBG::Rendering
{
    static uint32_t GetFramebuffer()
    {
        static uint32_t fbo = 0;
        if (fbo == 0)
        {
            glCreateFramebuffers(1, &fbo);
        }
        
        return fbo;
    }


    void SetColorAttachments(std::span<ColorAttachment> colorAttachments)
    {
        const DeviceInfo& deviceInfo = GetDeviceInfo();
        assert(colorAttachments.size() < deviceInfo.capabilities.countMaxFramebufferDrawBuffers && "Number of draw buffers exceeded");
        assert(colorAttachments.size() < deviceInfo.capabilities.countMaxColorAttachments && "Number of color attachments exceeded");

        uint32_t fbo = GetFramebuffer();
        for (int i = 0; i < colorAttachments.size(); i++)
        {
            const auto& attachment = colorAttachments[i];

            glNamedFramebufferTexture(fbo, GL_COLOR_ATTACHMENT0 + i, attachment.texture.GetHandle(), attachment.level);
        }

        for (int i = 0; i < colorAttachments.size(); i++)
        {
            const auto& attachment = colorAttachments[i];

            switch (attachment.loadOp)
            {
            case AttachmentLoadOp::Load: break;

            case AttachmentLoadOp::Clear:
                glClearNamedFramebufferfv(fbo, GL_COLOR, i, attachment.clearColor.data());
                break;

            case AttachmentLoadOp::DontCare:
                GLenum attachmentType = GL_COLOR;
                glInvalidateNamedFramebufferData(fbo, 1, &attachmentType);
                break;
            }
        }

        GLenum drawBuffers[16] = {}; // big enough https://opengl.gpuinfo.org/displaycapability.php?gpuName=GL_MAX_DRAW_BUFFERS
        for (int i = 0; i < colorAttachments.size(); i++)
        {
            drawBuffers[i] = GL_COLOR_ATTACHMENT0 + i;
        }
        glNamedFramebufferDrawBuffers(fbo, colorAttachments.size(), drawBuffers);

        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    }

    void SetColorAttachments(ScreenColorAttachment colorAttachment)
    {
        constexpr uint32_t fbo = 0;
        glClearNamedFramebufferfv(fbo, GL_COLOR, 0, colorAttachment.clearColor.data());

        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    }

    void BindShaderProgram(const ShaderProgram& shaderProgram)
    {
        glUseProgram(shaderProgram.GetHandle());
    }

    void BindTexture(const BBG::Texture2D& texture, const BBG::Sampler& sampler, uint32_t unit)
    {
        glBindSampler(unit, sampler.GetHandle());
        glBindTextureUnit(unit, texture.GetHandle());
    }

    void BindImage(const BBG::Texture2D& texture, ImageFormat format, uint32_t unit, uint32_t level, uint32_t layer, bool layered)
    {
        glBindImageTexture(unit, texture.GetHandle(), level, layered, layer, GL_READ_WRITE, (GLenum)format);
    }

    void DrawNonIndexed(Topology topology, uint32_t first, uint32_t count, uint32_t instanceCount, uint32_t baseInstance)
    {
        glDrawArraysInstancedBaseInstance((GLenum)topology, first, count, instanceCount, baseInstance);
    }

    void DispatchThreads(glm::uvec3 threads, glm::uvec3 workgroupSize)
    {
        auto numWorkGroups = glm::uvec3(
            // Integer ceil
            (threads.x + workgroupSize.x - 1) / workgroupSize.x,
            (threads.y + workgroupSize.y - 1) / workgroupSize.y,
            (threads.z + workgroupSize.z - 1) / workgroupSize.z
        );

        const DeviceInfo& deviceInfo = GetDeviceInfo();
        assert(numWorkGroups.x < deviceInfo.capabilities.countMaxWorkGroups.x && "Number of workGroupsX exceeded");
        assert(numWorkGroups.y < deviceInfo.capabilities.countMaxWorkGroups.y && "Number of workGroupsY exceeded");
        assert(numWorkGroups.z < deviceInfo.capabilities.countMaxWorkGroups.z && "Number of workGroupsZ exceeded");

        glDispatchCompute(numWorkGroups.x, numWorkGroups.y, numWorkGroups.z);
    }

    void MemoryBarrier(BBG::MemoryBarrierFlags memoryBarrier)
    {
        glMemoryBarrier((GLbitfield)memoryBarrier);
    }

    void SetShaderStorageBlock(const Buffer& buffer, uint32_t index, uint64_t offset, uint64_t size)
    {
        if (size == BIND_WHOLE_BUFFER)
        {
            size = buffer.GetSize() - offset;
        }
        glBindBufferRange(GL_SHADER_STORAGE_BUFFER, index, buffer.GetHandle(), offset, size);
    }

    void SetUniformBlock(const Buffer& buffer, uint32_t index, uint64_t offset, uint64_t size)
    {
        if (size == BIND_WHOLE_BUFFER)
        {
            size = buffer.GetSize() - offset;
        }
        glBindBufferRange(GL_UNIFORM_BUFFER, index, buffer.GetHandle(), offset, size);
    }

    void SetViewport(glm::vec2 size, uint32_t viewportIndex)
    {
        SetViewport(size, { 0.0f, 0.0f }, viewportIndex);
    }

    void SetViewport(glm::vec2 size, glm::vec2 offset, uint32_t viewportIndex)
    {
        const DeviceInfo& deviceInfo = GetDeviceInfo();
        assert(viewportIndex < deviceInfo.capabilities.countMaxViewports && "Number of viewports exceeded");

        glViewportIndexedf(viewportIndex, offset.x, offset.y, size.x, size.y);
    }
}

