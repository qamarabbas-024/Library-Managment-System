#include "utils/StringUtils.h"
#include <cctype>

namespace LMS::Utils {

std::string StringUtils::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

std::string StringUtils::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return result;
}

std::string StringUtils::toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) {
        return static_cast<char>(std::toupper(c));
    });
    return result;
}

bool StringUtils::containsIgnoreCase(const std::string& haystack, const std::string& needle) {
    if (needle.empty()) return true;
    return toLower(haystack).find(toLower(needle)) != std::string::npos;
}

std::vector<std::string> StringUtils::split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

std::vector<std::string> StringUtils::parseCSVLine(const std::string& line) {
    std::vector<std::string> fields;
    std::string field;
    bool inQuotes = false;

    for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];
        if (c == '"') {
            if (inQuotes && i + 1 < line.size() && line[i + 1] == '"') {
                field += '"';
                ++i; // Skip escaped quote
            } else {
                inQuotes = !inQuotes;
            }
        } else if (c == ',' && !inQuotes) {
            fields.push_back(trim(field));
            field.clear();
        } else {
            field += c;
        }
    }
    fields.push_back(trim(field));
    return fields;
}

std::string StringUtils::escapeCSVField(const std::string& field) {
    bool needsQuotes = (field.find(',') != std::string::npos ||
                        field.find('"') != std::string::npos ||
                        field.find('\n') != std::string::npos ||
                        field.find('\r') != std::string::npos);

    if (!needsQuotes) return field;

    std::string escaped = "\"";
    for (char c : field) {
        if (c == '"') escaped += "\"\"";
        else escaped += c;
    }
    escaped += "\"";
    return escaped;
}

std::string StringUtils::padRight(const std::string& str, size_t width, char padChar) {
    if (str.length() >= width) return str;
    return str + std::string(width - str.length(), padChar);
}

std::string StringUtils::padLeft(const std::string& str, size_t width, char padChar) {
    if (str.length() >= width) return str;
    return std::string(width - str.length(), padChar) + str;
}

std::string StringUtils::truncate(const std::string& str, size_t maxLen, const std::string& suffix) {
    if (str.length() <= maxLen) return str;
    if (maxLen <= suffix.length()) return str.substr(0, maxLen);
    return str.substr(0, maxLen - suffix.length()) + suffix;
}

size_t StringUtils::levenshteinDistance(const std::string& s1, const std::string& s2) {
    std::string str1 = toLower(s1);
    std::string str2 = toLower(s2);
    const size_t m = str1.size();
    const size_t n = str2.size();

    if (m == 0) return n;
    if (n == 0) return m;

    std::vector<size_t> prev(n + 1);
    std::vector<size_t> curr(n + 1);

    for (size_t j = 0; j <= n; ++j) prev[j] = j;

    for (size_t i = 1; i <= m; ++i) {
        curr[0] = i;
        for (size_t j = 1; j <= n; ++j) {
            size_t cost = (str1[i - 1] == str2[j - 1]) ? 0 : 1;
            curr[j] = std::min({prev[j] + 1, curr[j - 1] + 1, prev[j - 1] + cost});
        }
        prev = curr;
    }
    return prev[n];
}

double StringUtils::similarity(const std::string& s1, const std::string& s2) {
    size_t maxLen = std::max(s1.length(), s2.length());
    if (maxLen == 0) return 1.0;
    size_t dist = levenshteinDistance(s1, s2);
    return 1.0 - (static_cast<double>(dist) / static_cast<double>(maxLen));
}

bool StringUtils::fuzzyMatch(const std::string& text, const std::string& pattern, double threshold) {
    if (pattern.empty()) return true;
    if (containsIgnoreCase(text, pattern)) return true;

    // Check whole string similarity
    if (similarity(text, pattern) >= threshold) return true;

    // Check individual word similarities
    auto words = split(text, ' ');
    for (const auto& w : words) {
        if (similarity(w, pattern) >= threshold) return true;
    }
    return false;
}

} // namespace LMS::Utils
