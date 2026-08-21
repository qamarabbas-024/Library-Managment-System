#include "storage/AuditLogger.hpp"
#include "storage/CsvEngine.hpp"
#include "utils/DateTime.hpp"
#include <fstream>
#include <iostream>
#include <deque>

namespace LMS::Storage {

AuditLogger::AuditLogger(std::string filepath)
    : m_filepath(std::move(filepath)) {
    CsvEngine::ensureDirectoryExists(m_filepath);
}

void AuditLogger::log(LogLevel level, const std::string& action, const std::string& userId, const std::string& details) {
    CsvEngine::ensureDirectoryExists(m_filepath);
    std::ofstream fout(m_filepath, std::ios::app);
    if (!fout.is_open()) return;

    std::string lvlStr = "INFO";
    switch (level) {
        case LogLevel::INFO:  lvlStr = "INFO";  break;
        case LogLevel::WARN:  lvlStr = "WARN";  break;
        case LogLevel::ERROR: lvlStr = "ERROR"; break;
        case LogLevel::AUDIT: lvlStr = "AUDIT"; break;
    }

    std::string timestamp = Utils::DateTime::now().toString();
    fout << "[" << timestamp << "] [" << lvlStr << "] [USER: " << (userId.empty() ? "SYSTEM" : userId)
         << "] [ACTION: " << action << "] " << details << "\n";
    fout.close();
}

void AuditLogger::info(const std::string& action, const std::string& userId, const std::string& details) {
    log(LogLevel::INFO, action, userId, details);
}

void AuditLogger::warn(const std::string& action, const std::string& userId, const std::string& details) {
    log(LogLevel::WARN, action, userId, details);
}

void AuditLogger::error(const std::string& action, const std::string& userId, const std::string& details) {
    log(LogLevel::ERROR, action, userId, details);
}

void AuditLogger::audit(const std::string& action, const std::string& userId, const std::string& details) {
    log(LogLevel::AUDIT, action, userId, details);
}

std::vector<std::string> AuditLogger::getRecentLogs(size_t limit) const {
    std::vector<std::string> logs;
    std::ifstream fin(m_filepath);
    if (!fin.is_open()) return logs;

    std::deque<std::string> lines;
    std::string line;
    while (std::getline(fin, line)) {
        if (!line.empty()) {
            lines.push_back(line);
            if (lines.size() > limit) {
                lines.pop_front();
            }
        }
    }
    fin.close();
    return std::vector<std::string>(lines.begin(), lines.end());
}

} // namespace LMS::Storage
