#pragma once

#include <string>
#include <vector>

namespace LMS::Storage {

class CsvEngine {
public:
    static bool ensureDirectoryExists(const std::string& filepath);
    static std::vector<std::vector<std::string>> read(const std::string& filepath, bool skipHeader = false);
    static bool write(const std::string& filepath, const std::vector<std::vector<std::string>>& rows, const std::vector<std::string>& header = {});
    static bool appendRow(const std::string& filepath, const std::vector<std::string>& row);
};

} // namespace LMS::Storage
