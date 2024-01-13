#define STB_INCLUDE_IMPLEMENTATION
#define STB_INCLUDE_LINE_GLSL
#include <stb_include.h>
#include <filesystem>
#include <fstream>

#include "LoadFile.h"

std::string LoadFile(std::string_view path)
{
	auto file = std::ifstream(path.data(), std::ios::in | std::ios::binary);
	auto data = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
	return data;
}

std::string LoadFileWithIncludes(std::string_view path)
{
	auto fileContent = LoadFile(path);
	auto fileDir = std::filesystem::path(path).remove_filename().string();

	char error[256] = {};
	char* cProcessedFile = stb_include_string(fileContent.data(), nullptr, fileDir.data(), "", error);
	printf(error);

	std::string processedDataFile = cProcessedFile;
	free(cProcessedFile);

	return processedDataFile;
}
