#include "models/User.hpp"
#include "utils/StringUtils.hpp"
#include <algorithm>

namespace LMS::Models {

User::User(std::string id, std::string username, std::string passwordHash, std::string email,
           std::string phone, UserRole role, UserStatus status,
           MembershipType memType, std::string createdAt,
           int activeLoans, std::string salt)
    : m_id(std::move(id)),
      m_username(std::move(username)),
      m_passwordHash(std::move(passwordHash)),
      m_salt(std::move(salt)),
      m_email(std::move(email)),
      m_phone(std::move(phone)),
      m_role(role),
      m_status(status),
      m_membershipType(memType),
      m_createdAt(std::move(createdAt)),
      m_activeLoans(activeLoans) {}

std::string User::getRoleString() const {
    switch (m_role) {
        case UserRole::Admin: return "admin";
        case UserRole::Librarian: return "librarian";
        case UserRole::Member: return "member";
    }
    return "member";
}

std::string User::getStatusString() const {
    switch (m_status) {
        case UserStatus::Active: return "active";
        case UserStatus::Suspended: return "suspended";
        case UserStatus::Banned: return "banned";
    }
    return "active";
}

std::string User::getMembershipTypeString() const {
    switch (m_membershipType) {
        case MembershipType::Student: return "student";
        case MembershipType::Faculty: return "faculty";
        case MembershipType::General: return "general";
    }
    return "student";
}

int User::getMaxBorrowLimit() const {
    if (isAdmin()) return 20;
    switch (m_membershipType) {
        case MembershipType::Faculty: return 8;
        case MembershipType::Student: return 4;
        case MembershipType::General: return 2;
    }
    return 3;
}

int User::getMaxLoanDays() const {
    switch (m_membershipType) {
        case MembershipType::Faculty: return 30;
        case MembershipType::Student: return 14;
        case MembershipType::General: return 7;
    }
    return 14;
}

bool User::canBorrow() const {
    return m_status == UserStatus::Active && m_activeLoans < getMaxBorrowLimit();
}

std::vector<std::string> User::toCSV() const {
    return {
        m_id,
        m_username,
        m_passwordHash,
        m_email,
        m_phone,
        getRoleString(),
        getStatusString(),
        getMembershipTypeString(),
        m_createdAt,
        std::to_string(m_activeLoans),
        m_salt
    };
}

UserRole User::parseRole(const std::string& str) {
    std::string s = Utils::StringUtils::toLower(str);
    if (s == "admin") return UserRole::Admin;
    if (s == "librarian") return UserRole::Librarian;
    return UserRole::Member;
}

UserStatus User::parseStatus(const std::string& str) {
    std::string s = Utils::StringUtils::toLower(str);
    if (s == "banned") return UserStatus::Banned;
    if (s == "suspended") return UserStatus::Suspended;
    return UserStatus::Active;
}

MembershipType User::parseMembershipType(const std::string& str) {
    std::string s = Utils::StringUtils::toLower(str);
    if (s == "faculty") return MembershipType::Faculty;
    if (s == "general") return MembershipType::General;
    return MembershipType::Student;
}

User User::fromCSV(const std::vector<std::string>& row) {
    if (row.size() < 6) return User();

    // Legacy format: UID, username, password, email, phone, role, status
    if (row.size() == 7) {
        return User(
            row[0], row[1], row[2], row[3], row[4],
            parseRole(row[5]), parseStatus(row[6]),
            MembershipType::Student, "2026-01-01", 0, "LMS_SALT_2026"
        );
    }

    // Modern format
    std::string id = row[0];
    std::string username = row[1];
    std::string pass = row[2];
    std::string email = (row.size() > 3) ? row[3] : "";
    std::string phone = (row.size() > 4) ? row[4] : "";
    UserRole role = (row.size() > 5) ? parseRole(row[5]) : UserRole::Member;
    UserStatus status = (row.size() > 6) ? parseStatus(row[6]) : UserStatus::Active;
    MembershipType memType = (row.size() > 7) ? parseMembershipType(row[7]) : MembershipType::Student;
    std::string created = (row.size() > 8) ? row[8] : "2026-01-01";
    int activeLoans = 0;
    try { if (row.size() > 9) activeLoans = std::stoi(row[9]); } catch (...) {}
    std::string salt = (row.size() > 10) ? row[10] : "LMS_SALT_2026";

    return User(id, username, pass, email, phone, role, status, memType, created, activeLoans, salt);
}

} // namespace LMS::Models
