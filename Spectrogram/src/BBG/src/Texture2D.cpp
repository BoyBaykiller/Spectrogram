#include <utility>

#include <BBG/Texture2D.h>

namespace BBG
{
    Texture2D::Texture2D(const CreateInfo& createInfo) noexcept
        : createInfo_(createInfo)
    {
        glCreateTextures(GL_TEXTURE_2D, 1, &handle_);
        glTextureStorage2D(handle_, createInfo.mipmapLevels, (GLenum)createInfo.format, createInfo.size.x, createInfo.size.y);
    }

    Texture2D::~Texture2D() noexcept
    {
        glDeleteTextures(1, &handle_);
    }

    Texture2D::Texture2D(Texture2D&& old) noexcept
        : handle_(std::exchange(old.handle_, 0)),
        createInfo_(std::exchange(old.createInfo_, {}))
    {
    }

    Texture2D& Texture2D::operator=(Texture2D&& old) noexcept
    {
        if (&old == this)
        {
            return *this;
        }
        this->~Texture2D();
        return *(new (this) Texture2D(std::move(old)));
    }

    void Texture2D::SetSwizzle(GLint swizzleR, GLint swizzleG, GLint swizzleB, GLint swizzleA) const
    {
        GLint swizzleMask[] = { swizzleR, swizzleG, swizzleB, swizzleA };
        glTextureParameteriv(handle_, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
    }

    void Texture2D::UploadPixels(const UploadInfo& info, const void* data) const
    {
        auto derivedSize = info.size;
        if (derivedSize == WHOLE_TEXTURE)
        {
            derivedSize = createInfo_.size;
        }

        glTextureSubImage2D(
            handle_,
            info.level,
            info.xOffset,
            info.yOffset,
            derivedSize.x,
            derivedSize.y,
            (GLenum)info.format,
            (GLenum)info.type,
            data
        );
    }

    void Texture2D::ClearPixels(const ClearInfo& info, const void* data) const
    {
        auto derivedSize = info.size;
        if (derivedSize == WHOLE_TEXTURE)
        {
            derivedSize = createInfo_.size;
        }
        glClearTexSubImage(handle_, info.level, info.xOffset, info.yOffset, 0, derivedSize.x, derivedSize.y, 1, (GLenum)info.format, (GLenum)info.type, data);
    }
}
