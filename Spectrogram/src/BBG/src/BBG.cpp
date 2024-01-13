#include <BBG/BBG.h>

namespace BBG
{
    static DeviceInfo deviceInfo = {};

    void BBG::Initialize()
    {
        // get device capabilities
        deviceInfo.gpuName = (const char*)glGetString(GL_RENDERER);
        glGetIntegerv(GL_MAX_DRAW_BUFFERS, (int32_t*)&deviceInfo.capabilities.countMaxFramebufferDrawBuffers);
        glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, (int32_t*)&deviceInfo.capabilities.countMaxColorAttachments);
        glGetIntegerv(GL_MAX_VIEWPORTS, (int32_t*)&deviceInfo.capabilities.countMaxViewports);
        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, (int32_t*)&deviceInfo.capabilities.countMaxWorkGroups.x);
        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, (int32_t*)&deviceInfo.capabilities.countMaxWorkGroups.y);
        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, (int32_t*)&deviceInfo.capabilities.countMaxWorkGroups.z);

        // create and bind dummy VAO, because drawing without one is not allowed since OpenGL 3.3
        uint32_t dummyVao;
        glCreateVertexArrays(1, &dummyVao);
        glBindVertexArray(dummyVao);

        // fix default settings
        glDisable(GL_MULTISAMPLE);
        glDisable(GL_DITHER);
        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    }

    const DeviceInfo& GetDeviceInfo()
    {
        return deviceInfo;
    }
}
