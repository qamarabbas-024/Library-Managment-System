#include "services/AuthService.h"
#include "utils/Crypto.h"
#include "utils/StringUtils.h"
#include "utils/DateTime.h"

namespace LMS::Services {

AuthService::AuthService(std::shared_ptr<Storage::UserRepository> userRepo,
                         std::shared_ptr<Storage::AuditLogger> logger)
    : m_userRepo(std::move(userRepo)), m_logger(std::move(logger)) {}

AuthResult AuthService::signup(const std::string& username, const std::string& password,
                               const std::string& email, const std::string& phone,
                               Models::MembershipType memType) {
    std::string cleanUser = Utils::StringUtils::trim(username);
    std::string cleanPass = Utils::StringUtils::trim(password);
    std::string cleanEmail = Utils::StringUtils::trim(email);
    std::string cleanPhone = Utils::StringUtils::trim(phone);

    if (cleanUser.length() < 3) {
        return {false, "Username must be at least 3 characters long.", std::nullopt};
    }
    if (cleanPass.length() < 4) {
        return {false, "Password must be at least 4 characters long.", std::nullopt};
    }
    if (cleanEmail.empty() || cleanEmail.find('@') == std::string::npos) {
        return {false, "Please provide a valid email address.", std::nullopt};
    }

    if (m_userRepo->existsByUsername(cleanUser)) {
        return {false, "Username '" + cleanUser + "' is already registered.", std::nullopt};
    }
    if (m_userRepo->existsByEmail(cleanEmail)) {
        return {false, "Email '" + cleanEmail + "' is already in use.", std::nullopt};
    }

    std::string salt = Utils::Crypto::generateSalt();
    std::string hash = Utils::Crypto::hashPassword(cleanPass, salt);
    std::string newId = m_userRepo->generateNextId();
    std::string today = Utils::Date::today().toString();

    Models::User newUser(newId, cleanUser, hash, cleanEmail, cleanPhone,
                         Models::UserRole::Member, Models::UserStatus::Active,
                         memType, today, 0, salt);

    if (m_userRepo->save(newUser)) {
        if (m_logger) m_logger->audit("SIGNUP", newId, "New member registered: " + cleanUser);
        return {true, "Account registered successfully. You may now log in.", newUser};
    }

    return {false, "Failed to save user account. Please try again.", std::nullopt};
}

AuthResult AuthService::login(const std::string& username, const std::string& password) {
    std::string cleanUser = Utils::StringUtils::trim(username);
    auto userOpt = m_userRepo->findByUsername(cleanUser);

    if (!userOpt.has_value()) {
        if (m_logger) m_logger->warn("LOGIN_FAILED", cleanUser, "User not found");
        return {false, "Invalid username or password.", std::nullopt};
    }

    Models::User user = userOpt.value();
    bool validPass = Utils::Crypto::verifyPassword(password, user.getPasswordHash(), user.getSalt());

    if (!validPass) {
        if (m_logger) m_logger->warn("LOGIN_FAILED", user.getId(), "Incorrect password attempt");
        return {false, "Invalid username or password.", std::nullopt};
    }

    if (user.getStatus() == Models::UserStatus::Banned) {
        if (m_logger) m_logger->warn("LOGIN_BLOCKED", user.getId(), "Banned user attempted login");
        return {false, "This account has been banned. Please contact the library administrator.", std::nullopt};
    }

    if (user.getStatus() == Models::UserStatus::Suspended) {
        if (m_logger) m_logger->warn("LOGIN_BLOCKED", user.getId(), "Suspended user attempted login");
        return {false, "This account is currently suspended due to policy violations.", std::nullopt};
    }

    m_currentUser = user;
    if (m_logger) m_logger->audit("LOGIN_SUCCESS", user.getId(), "Logged in as " + user.getRoleString());
    return {true, "Login successful. Welcome, " + user.getUsername() + "!", user};
}

void AuthService::logout() {
    if (m_currentUser.has_value()) {
        if (m_logger) m_logger->audit("LOGOUT", m_currentUser->getId(), "User logged out");
        m_currentUser.reset();
    }
}

bool AuthService::resetPassword(const std::string& email, const std::string& newPassword) {
    std::string cleanEmail = Utils::StringUtils::trim(email);
    auto userOpt = m_userRepo->findByEmail(cleanEmail);
    if (!userOpt.has_value()) {
        return false;
    }

    Models::User user = userOpt.value();
    std::string salt = Utils::Crypto::generateSalt();
    std::string hash = Utils::Crypto::hashPassword(newPassword, salt);

    user.setPasswordHash(hash);
    if (m_userRepo->update(user)) {
        if (m_logger) m_logger->audit("PASSWORD_RESET", user.getId(), "Password reset requested via email");
        return true;
    }
    return false;
}

bool AuthService::changePassword(const std::string& userId, const std::string& oldPassword, const std::string& newPassword) {
    auto userOpt = m_userRepo->findById(userId);
    if (!userOpt.has_value()) return false;

    Models::User user = userOpt.value();
    if (!Utils::Crypto::verifyPassword(oldPassword, user.getPasswordHash(), user.getSalt())) {
        return false;
    }

    std::string salt = Utils::Crypto::generateSalt();
    std::string hash = Utils::Crypto::hashPassword(newPassword, salt);
    user.setPasswordHash(hash);

    if (m_userRepo->update(user)) {
        if (m_currentUser.has_value() && m_currentUser->getId() == userId) {
            m_currentUser = user;
        }
        if (m_logger) m_logger->audit("PASSWORD_CHANGED", userId, "User changed password successfully");
        return true;
    }
    return false;
}

void AuthService::refreshCurrentUser() {
    if (m_currentUser.has_value()) {
        auto refreshed = m_userRepo->findById(m_currentUser->getId());
        if (refreshed.has_value()) {
            m_currentUser = refreshed.value();
        }
    }
}

} // namespace LMS::Services
