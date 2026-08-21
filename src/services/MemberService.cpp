#include "services/MemberService.hpp"
#include "utils/Crypto.hpp"
#include "utils/StringUtils.hpp"
#include "utils/DateTime.hpp"

namespace LMS::Services {

MemberService::MemberService(std::shared_ptr<Storage::UserRepository> userRepo,
                             std::shared_ptr<Storage::LoanRepository> loanRepo,
                             std::shared_ptr<Storage::AuditLogger> logger)
    : m_userRepo(std::move(userRepo)),
      m_loanRepo(std::move(loanRepo)),
      m_logger(std::move(logger)) {}

std::optional<Models::User> MemberService::addMember(const std::string& username, const std::string& password,
                                                     const std::string& email, const std::string& phone,
                                                     Models::MembershipType memType, const std::string& adminId) {
    std::string cleanUser = Utils::StringUtils::trim(username);
    std::string cleanPass = Utils::StringUtils::trim(password);
    std::string cleanEmail = Utils::StringUtils::trim(email);
    std::string cleanPhone = Utils::StringUtils::trim(phone);

    if (cleanUser.empty() || cleanPass.empty() || cleanEmail.empty()) return std::nullopt;
    if (m_userRepo->existsByUsername(cleanUser) || m_userRepo->existsByEmail(cleanEmail)) return std::nullopt;

    std::string salt = Utils::Crypto::generateSalt();
    std::string hash = Utils::Crypto::hashPassword(cleanPass, salt);
    std::string newId = m_userRepo->generateNextId();
    std::string today = Utils::Date::today().toString();

    Models::User member(newId, cleanUser, hash, cleanEmail, cleanPhone,
                        Models::UserRole::Member, Models::UserStatus::Active,
                        memType, today, 0, salt);

    if (m_userRepo->save(member)) {
        if (m_logger) m_logger->audit("ADMIN_ADD_MEMBER", adminId, "Created member: " + cleanUser + " (" + newId + ")");
        return member;
    }
    return std::nullopt;
}

bool MemberService::updateProfile(const std::string& userId, const std::string& email,
                                  const std::string& phone, const std::string& password) {
    auto userOpt = m_userRepo->findById(userId);
    if (!userOpt.has_value()) return false;

    Models::User user = userOpt.value();
    if (!email.empty()) {
        std::string cleanEmail = Utils::StringUtils::trim(email);
        auto existing = m_userRepo->findByEmail(cleanEmail);
        if (existing.has_value() && existing->getId() != userId) {
            return false; // Email collision
        }
        user.setEmail(cleanEmail);
    }
    if (!phone.empty()) user.setPhone(Utils::StringUtils::trim(phone));
    if (!password.empty()) {
        std::string salt = Utils::Crypto::generateSalt();
        user.setPasswordHash(Utils::Crypto::hashPassword(password, salt));
    }

    if (m_userRepo->update(user)) {
        if (m_logger) m_logger->audit("UPDATE_PROFILE", userId, "User profile updated");
        return true;
    }
    return false;
}

bool MemberService::setMemberStatus(const std::string& userId, Models::UserStatus status, const std::string& adminId) {
    auto userOpt = m_userRepo->findById(userId);
    if (!userOpt.has_value()) return false;

    Models::User user = userOpt.value();
    user.setStatus(status);
    if (m_userRepo->update(user)) {
        if (m_logger) m_logger->audit("SET_MEMBER_STATUS", adminId, "Status for user " + userId + " set to " + user.getStatusString());
        return true;
    }
    return false;
}

bool MemberService::banMember(const std::string& userId, const std::string& adminId) {
    return setMemberStatus(userId, Models::UserStatus::Banned, adminId);
}

bool MemberService::unbanMember(const std::string& userId, const std::string& adminId) {
    return setMemberStatus(userId, Models::UserStatus::Active, adminId);
}

bool MemberService::suspendMember(const std::string& userId, const std::string& adminId) {
    return setMemberStatus(userId, Models::UserStatus::Suspended, adminId);
}

bool MemberService::activateMember(const std::string& userId, const std::string& adminId) {
    return setMemberStatus(userId, Models::UserStatus::Active, adminId);
}

bool MemberService::deleteMember(const std::string& userId, const std::string& adminId) {
    auto activeLoans = m_loanRepo->findActiveLoansByUserId(userId);
    if (!activeLoans.empty()) {
        if (m_logger) m_logger->warn("DELETE_MEMBER_BLOCKED", adminId, "Cannot delete member with active loans: " + userId);
        return false;
    }

    if (m_userRepo->remove(userId)) {
        if (m_logger) m_logger->audit("DELETE_MEMBER", adminId, "Deleted member account: " + userId);
        return true;
    }
    return false;
}

std::optional<Models::User> MemberService::getMemberById(const std::string& userId) const {
    return m_userRepo->findById(userId);
}

std::vector<Models::User> MemberService::getAllMembers() const {
    return m_userRepo->findMembers();
}

MemberStats MemberService::getMemberStats() const {
    MemberStats stats;
    auto members = m_userRepo->findMembers();
    stats.totalMembers = members.size();

    for (const auto& m : members) {
        if (m.getStatus() == Models::UserStatus::Active) stats.activeMembers++;
        else if (m.getStatus() == Models::UserStatus::Banned) stats.bannedMembers++;
        else if (m.getStatus() == Models::UserStatus::Suspended) stats.suspendedMembers++;

        if (m.getMembershipType() == Models::MembershipType::Student) stats.studentCount++;
        else if (m.getMembershipType() == Models::MembershipType::Faculty) stats.facultyCount++;
        else if (m.getMembershipType() == Models::MembershipType::General) stats.generalCount++;
    }
    return stats;
}

} // namespace LMS::Services
