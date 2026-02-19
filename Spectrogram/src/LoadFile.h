#pragma once
#include <string_view>
#include <string>

std::string LoadFile(std::string_view path);
std::string LoadFileWithIncludes(std::string_view path);
