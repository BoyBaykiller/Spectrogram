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
		UnsignedByte = GL_UNSIGNED_BYTE,
	};

	enum class UploadPixelFormat
	{
		R = GL_RED,
		Rgba = GL_RGBA,
		Rgb = GL_RGB,
	};

	enum class InternalPixelFormat
	{
		R32_Float = GL_R32F,
		R8G8B8_Unorm = GL_RGB8,
		R8G8B8A8_Unorm = GL_RGBA8,
		R16G16B16A16_Float = GL_RGBA16F,
		R32G32B32A32_Float = GL_RGBA32F,
	};

	enum class ImageFormat
	{
		R8G8B8A8_Unorm = (int)InternalPixelFormat::R8G8B8A8_Unorm,
		R16G16B16A16_Float = (int)InternalPixelFormat::R16G16B16A16_Float,
		R32G32B32A32_Float = (int)InternalPixelFormat::R32G32B32A32_Float,
	};

	enum class Topology
	{
		Points = GL_POINTS,
		Lines = GL_LINES,
		LineStrip = GL_LINE_STRIP,
		Triangles = GL_TRIANGLES,
	};

	enum class ShaderType
	{
		Vertex = GL_VERTEX_SHADER,
		Fragment = GL_FRAGMENT_SHADER,
		Compute = GL_COMPUTE_SHADER,
	};

	enum class BufferStorageFlag
	{
		// The buffer can only be filled once at the time of buffer creation
		None = GL_NONE,
		
		// The buffer may be updated or downloaded with Buffer::UploadData or Buffer::DownloadData.
		// Synchronization is taken care of by OpenGL.
		DynamicStorage = GL_DYNAMIC_STORAGE_BIT,

		// The buffer can be read & write by using the mapped memory pointer. Buffer::UploadData no longer works.
		// Writes by the DEVICE only become visible to the HOST after a call to glFenceSync(SYNC_GPU_COMMANDS_COMPLETE, 0).
		MappedStorage = GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT | GL_MAP_READ_BIT | GL_MAP_WRITE_BIT,

		// The buffer can be read & write by using the mapped memory pointer. Buffer::UploadData no longer works.
		// Writes by the HOST only become visible to the DEVICE after a call to glFlushMappedBufferRange.
		// Writes by the DEVICE only become visible to the HOST after a call to glMemoryBarrier(CLIENT_MAPPED_BUFFER_BARRIER_BIT)
		// followed by glFenceSync(SYNC_GPU_COMMANDS_COMPLETE, 0)
		MappedStorageNoSync = GL_MAP_PERSISTENT_BIT | GL_MAP_FLUSH_EXPLICIT_BIT | GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT
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
