#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace LMS::Utils {

class Crypto {
public:
    static std::string sha256(const std::string& input);
    static std::string hashPassword(const std::string& password, const std::string& salt = "LMS_SECURE_SALT_2026");
    static bool verifyPassword(const std::string& password, const std::string& hash, const std::string& salt = "LMS_SECURE_SALT_2026");
    static std::string generateSalt(size_t length = 16);
};

} // namespace LMS::Utils
