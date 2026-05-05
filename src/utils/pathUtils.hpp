#pragma once

#include <filesystem>

namespace pathUtils {
    std::filesystem::path make_absolute_path(std::filesystem::path const& path, bool check_path_exists = true);
} // namespace pathUtils