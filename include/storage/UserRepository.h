#pragma once

#include "storage/IRepository.h"
#include "models/User.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace LMS::Storage {

class UserRepository : public IRepository<Models::User, std::string> {
private:
    std::string m_filepath;
    std::vector<Models::User> m_users;
    std::unordered_map<std::string, size_t> m_idIndex;
    std::unordered_map<std::string, size_t> m_usernameIndex;

    void rebuildIndex();

public:
    explicit UserRepository(std::string filepath = "data/users.csv");

    std::vector<Models::User> findAll() const override;
    std::optional<Models::User> findById(const std::string& id) const override;
    std::optional<Models::User> findByUsername(const std::string& username) const;
    std::optional<Models::User> findByEmail(const std::string& email) const;
    bool save(const Models::User& entity) override;
    bool update(const Models::User& entity) override;
    bool remove(const std::string& id) override;
    bool existsById(const std::string& id) const override;
    bool existsByUsername(const std::string& username) const;
    bool existsByEmail(const std::string& email) const;
    size_t count() const override { return m_users.size(); }
    void reload() override;
    bool flush() override;

    std::vector<Models::User> findMembers() const;
    std::string generateNextId() const;
};

} // namespace LMS::Storage
