#pragma once

#include <string>
#include <vector>

namespace LMS::Storage {

enum class LogLevel {
    INFO,
    WARN,
    ERROR,
    AUDIT
};

class AuditLogger {
private:
    std::string m_filepath;

public:
    explicit AuditLogger(std::string filepath = "logs/audit.log");

    void log(LogLevel level, const std::string& action, const std::string& userId, const std::string& details);
    void info(const std::string& action, const std::string& userId, const std::string& details);
    void warn(const std::string& action, const std::string& userId, const std::string& details);
    void error(const std::string& action, const std::string& userId, const std::string& details);
    void audit(const std::string& action, const std::string& userId, const std::string& details);

    std::vector<std::string> getRecentLogs(size_t limit = 50) const;
};

} // namespace LMS::Storage
