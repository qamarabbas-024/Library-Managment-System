#pragma once

#include <string>
#include <vector>

namespace LMS::Models {

enum class UserRole {
    Admin,
    Librarian,
    Member
};

enum class UserStatus {
    Active,
    Suspended,
    Banned
};

enum class MembershipType {
    Student,
    Faculty,
    General
};

class User {
private:
    std::string m_id;
    std::string m_username;
    std::string m_passwordHash;
    std::string m_salt{"LMS_SALT_2026"};
    std::string m_email;
    std::string m_phone;
    UserRole m_role{UserRole::Member};
    UserStatus m_status{UserStatus::Active};
    MembershipType m_membershipType{MembershipType::Student};
    std::string m_createdAt{"2026-01-01"};
    int m_activeLoans{0};

public:
    User() = default;
    User(std::string id, std::string username, std::string passwordHash, std::string email,
         std::string phone, UserRole role = UserRole::Member, UserStatus status = UserStatus::Active,
         MembershipType memType = MembershipType::Student, std::string createdAt = "2026-01-01",
         int activeLoans = 0, std::string salt = "LMS_SALT_2026");

    // Getters
    const std::string& getId() const { return m_id; }
    const std::string& getUsername() const { return m_username; }
    const std::string& getPasswordHash() const { return m_passwordHash; }
    const std::string& getSalt() const { return m_salt; }
    const std::string& getEmail() const { return m_email; }
    const std::string& getPhone() const { return m_phone; }
    UserRole getRole() const { return m_role; }
    UserStatus getStatus() const { return m_status; }
    MembershipType getMembershipType() const { return m_membershipType; }
    const std::string& getCreatedAt() const { return m_createdAt; }
    int getActiveLoans() const { return m_activeLoans; }

    // Helpers
    std::string getRoleString() const;
    std::string getStatusString() const;
    std::string getMembershipTypeString() const;
    int getMaxBorrowLimit() const;
    int getMaxLoanDays() const;

    // Setters
    void setUsername(const std::string& u) { m_username = u; }
    void setPasswordHash(const std::string& hash) { m_passwordHash = hash; }
    void setSalt(const std::string& salt) { m_salt = salt; }
    void setEmail(const std::string& email) { m_email = email; }
    void setPhone(const std::string& p) { m_phone = p; }
    void setStatus(UserStatus s) { m_status = s; }
    void setRole(UserRole r) { m_role = r; }
    void setMembershipType(MembershipType m) { m_membershipType = m; }

    // Domain business methods
    bool canBorrow() const;
    bool isAdmin() const { return m_role == UserRole::Admin || m_role == UserRole::Librarian; }
    bool isActive() const { return m_status == UserStatus::Active; }
    void incrementActiveLoans() { ++m_activeLoans; }
    void decrementActiveLoans() { if (m_activeLoans > 0) --m_activeLoans; }

    // Serialization
    std::vector<std::string> toCSV() const;
    static User fromCSV(const std::vector<std::string>& row);

    static UserRole parseRole(const std::string& str);
    static UserStatus parseStatus(const std::string& str);
    static MembershipType parseMembershipType(const std::string& str);
};

} // namespace LMS::Models
