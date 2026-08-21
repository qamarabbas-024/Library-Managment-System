#pragma once

#include "storage/UserRepository.h"
#include "storage/AuditLogger.h"
#include "models/User.h"
#include <memory>
#include <string>
#include <optional>

namespace LMS::Services {

struct AuthResult {
    bool success{false};
    std::string message;
    std::optional<Models::User> user;
};

class AuthService {
private:
    std::shared_ptr<Storage::UserRepository> m_userRepo;
    std::shared_ptr<Storage::AuditLogger> m_logger;
    std::optional<Models::User> m_currentUser;

public:
    AuthService(std::shared_ptr<Storage::UserRepository> userRepo,
                std::shared_ptr<Storage::AuditLogger> logger);

    AuthResult signup(const std::string& username, const std::string& password,
                      const std::string& email, const std::string& phone,
                      Models::MembershipType memType = Models::MembershipType::Student);

    AuthResult login(const std::string& username, const std::string& password);
    void logout();

    bool isLoggedIn() const { return m_currentUser.has_value(); }
    bool isAdmin() const { return m_currentUser.has_value() && m_currentUser->isAdmin(); }
    const std::optional<Models::User>& getCurrentUser() const { return m_currentUser; }

    bool resetPassword(const std::string& email, const std::string& newPassword);
    bool changePassword(const std::string& userId, const std::string& oldPassword, const std::string& newPassword);

    void refreshCurrentUser();
};

} // namespace LMS::Services
