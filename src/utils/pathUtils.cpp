#include "pathUtils.hpp"

#include <filesystem>
#include <iostream>
#include <stdexcept>

namespace pathUtils {
    std::filesystem::path make_absolute_path(std::filesystem::path const& path, bool check_path_exists)
    {
        auto const res {path.is_relative() ? CMAKE_SOURCE_DIR / path : path };

        if (check_path_exists && !std::filesystem::exists(res))
        {
            using namespace std::literals;
            auto const msg { "Path \""s + res.string() + "\" does not exist. Make sure you didn't misspell it or made it relative to the wrong folder. All relative paths will be relative to the directory containing your CMakeLists.txt file." };
            std::cerr << msg << '\n';
            throw std::runtime_error{msg};
        }

        return res;
    }
} // namespace pathUtils

