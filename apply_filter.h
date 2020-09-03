#pragma once
#include <filesystem>
#include <string>

std::filesystem::path apply_filter(const std::string& filter,
                                   const std::filesystem::path& filepath);
