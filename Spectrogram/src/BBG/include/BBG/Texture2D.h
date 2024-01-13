#pragma once
#include <BBG/BBG.h>

#include <glm/vec2.hpp>

namespace BBG
{
	class Texture2D
	{
	public:
		static constexpr glm::uvec2 WHOLE_TEXTURE = glm::uvec2(UINT32_MAX);

		struct CreateInfo
		{
			glm::uvec2 size = {};
			InternalPixelFormat format = {};
			uint32_t mipmapLevels = 1;
		};

		struct UploadInfo
		{
			UploadPixelFormat format = {};
			UploadPixelType type = {};
			glm::uvec2 size = WHOLE_TEXTURE;
			uint32_t xOffset = 0;
			uint32_t yOffset = 0;
			uint32_t level = 0;
		};

		using ClearInfo = UploadInfo;

	public:
		explicit Texture2D(const CreateInfo& info) noexcept;
		~Texture2D() noexcept;
		Texture2D(Texture2D&& old) noexcept;
		Texture2D& operator=(Texture2D&& old) noexcept;
		BBG_NON_COPYABLE(Texture2D);

		void SetSwizzle(GLint swizzleR, GLint swizzleG, GLint swizzleB, GLint swizzleA) const;
		void UploadPixels(const UploadInfo& info, const void* data) const;
		void ClearPixels(const ClearInfo& info, const void* data) const;
		
		const CreateInfo& GetCreateInfo() const
		{
			return createInfo_;
		}

		uint32_t GetHandle() const
		{
			return handle_;
		}

	private:
		uint32_t handle_ = 0;
		CreateInfo createInfo_;
	};
}
