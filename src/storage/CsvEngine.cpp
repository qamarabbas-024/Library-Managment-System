#include "storage/CsvEngine.hpp"
#include "utils/StringUtils.hpp"
#include <fstream>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

namespace LMS::Storage {

bool CsvEngine::ensureDirectoryExists(const std::string& filepath) {
    try {
        fs::path p(filepath);
        if (p.has_parent_path()) {
            fs::create_directories(p.parent_path());
        }
        return true;
    } catch (...) {
        return false;
    }
}

std::vector<std::vector<std::string>> CsvEngine::read(const std::string& filepath, bool skipHeader) {
    std::vector<std::vector<std::string>> rows;
    std::ifstream fin(filepath);
    if (!fin.is_open()) return rows;

    std::string line;
    bool isFirst = true;
    while (std::getline(fin, line)) {
        line = Utils::StringUtils::trim(line);
        if (line.empty() || line[0] == '#') continue; // Skip comments and empty lines

        if (isFirst && skipHeader) {
            isFirst = false;
            continue;
        }
        isFirst = false;

        auto parsed = Utils::StringUtils::parseCSVLine(line);
        if (!parsed.empty()) {
            rows.push_back(parsed);
        }
    }
    fin.close();
    return rows;
}

bool CsvEngine::write(const std::string& filepath, const std::vector<std::vector<std::string>>& rows, const std::vector<std::string>& header) {
    ensureDirectoryExists(filepath);
    std::string tempPath = filepath + ".tmp";

    std::ofstream fout(tempPath);
    if (!fout.is_open()) return false;

    if (!header.empty()) {
        for (size_t i = 0; i < header.size(); ++i) {
            fout << Utils::StringUtils::escapeCSVField(header[i]);
            if (i + 1 < header.size()) fout << ",";
        }
        fout << "\n";
    }

    for (const auto& row : rows) {
        for (size_t i = 0; i < row.size(); ++i) {
            fout << Utils::StringUtils::escapeCSVField(row[i]);
            if (i + 1 < row.size()) fout << ",";
        }
        fout << "\n";
    }
    fout.close();

    try {
        fs::rename(tempPath, filepath);
        return true;
    } catch (...) {
        // Fallback copy if atomic rename fails
        std::ifstream src(tempPath, std::ios::binary);
        std::ofstream dst(filepath, std::ios::binary);
        dst << src.rdbuf();
        src.close();
        dst.close();
        fs::remove(tempPath);
        return true;
    }
}

bool CsvEngine::appendRow(const std::string& filepath, const std::vector<std::string>& row) {
    ensureDirectoryExists(filepath);
    std::ofstream fout(filepath, std::ios::app);
    if (!fout.is_open()) return false;

    for (size_t i = 0; i < row.size(); ++i) {
        fout << Utils::StringUtils::escapeCSVField(row[i]);
        if (i + 1 < row.size()) fout << ",";
    }
    fout << "\n";
    fout.close();
    return true;
}

} // namespace LMS::Storage
