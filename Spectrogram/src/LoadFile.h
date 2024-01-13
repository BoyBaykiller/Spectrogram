#pragma once
#include <string_view>

std::string LoadFile(std::string_view path);
std::string LoadFileWithIncludes(std::string_view path);
