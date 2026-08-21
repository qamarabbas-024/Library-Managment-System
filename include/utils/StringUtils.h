#pragma once

#include <string>
#include <vector>

namespace LMS::Utils {

class StringUtils {
public:
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string toUpper(const std::string& str);
    static std::vector<std::string> split(const std::string& str, char delimiter);
    static std::vector<std::string> parseCSVLine(const std::string& line);
    static std::string escapeCSVField(const std::string& field);
    static bool containsIgnoreCase(const std::string& haystack, const std::string& needle);
    static std::string padRight(const std::string& str, size_t width, char padChar = ' ');
    static std::string padLeft(const std::string& str, size_t width, char padChar = ' ');
    static std::string truncate(const std::string& str, size_t maxLen, const std::string& suffix = "...");
};

} // namespace LMS::Utils
