#pragma once

#include "storage/UserRepository.h"
#include "storage/LoanRepository.h"
#include "storage/AuditLogger.h"
#include "models/User.h"
#include <memory>
#include <string>
#include <vector>
#include <optional>

namespace LMS::Services {

struct MemberStats {
    size_t totalMembers{0};
    size_t activeMembers{0};
    size_t bannedMembers{0};
    size_t suspendedMembers{0};
    size_t studentCount{0};
    size_t facultyCount{0};
    size_t generalCount{0};
};

class MemberService {
private:
    std::shared_ptr<Storage::UserRepository> m_userRepo;
    std::shared_ptr<Storage::LoanRepository> m_loanRepo;
    std::shared_ptr<Storage::AuditLogger> m_logger;

public:
    MemberService(std::shared_ptr<Storage::UserRepository> userRepo,
                  std::shared_ptr<Storage::LoanRepository> loanRepo,
                  std::shared_ptr<Storage::AuditLogger> logger);

    std::optional<Models::User> addMember(const std::string& username, const std::string& password,
                                          const std::string& email, const std::string& phone,
                                          Models::MembershipType memType, const std::string& adminId);

    bool updateProfile(const std::string& userId, const std::string& email,
                       const std::string& phone, const std::string& password = "");

    bool setMemberStatus(const std::string& userId, Models::UserStatus status, const std::string& adminId);
    bool banMember(const std::string& userId, const std::string& adminId);
    bool unbanMember(const std::string& userId, const std::string& adminId);
    bool suspendMember(const std::string& userId, const std::string& adminId);
    bool activateMember(const std::string& userId, const std::string& adminId);
    bool deleteMember(const std::string& userId, const std::string& adminId);

    std::optional<Models::User> getMemberById(const std::string& userId) const;
    std::vector<Models::User> getAllMembers() const;
    MemberStats getMemberStats() const;
};

} // namespace LMS::Services
