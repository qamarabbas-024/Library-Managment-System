#include "storage/UserRepository.h"
#include "storage/CsvEngine.h"
#include "utils/StringUtils.h"
#include <iomanip>
#include <sstream>

namespace LMS::Storage {

UserRepository::UserRepository(std::string filepath)
    : m_filepath(std::move(filepath)) {
    reload();
}

void UserRepository::rebuildIndex() {
    m_idIndex.clear();
    m_usernameIndex.clear();
    for (size_t i = 0; i < m_users.size(); ++i) {
        m_idIndex[m_users[i].getId()] = i;
        m_usernameIndex[Utils::StringUtils::toLower(m_users[i].getUsername())] = i;
    }
}

void UserRepository::reload() {
    m_users.clear();
    auto rows = CsvEngine::read(m_filepath);
    for (const auto& row : rows) {
        if (!row.empty()) {
            m_users.push_back(Models::User::fromCSV(row));
        }
    }
    rebuildIndex();
}

bool UserRepository::flush() {
    std::vector<std::vector<std::string>> rows;
    rows.reserve(m_users.size());
    for (const auto& u : m_users) {
        rows.push_back(u.toCSV());
    }
    return CsvEngine::write(m_filepath, rows);
}

std::vector<Models::User> UserRepository::findAll() const {
    return m_users;
}

std::optional<Models::User> UserRepository::findById(const std::string& id) const {
    auto it = m_idIndex.find(id);
    if (it != m_idIndex.end()) {
        return m_users[it->second];
    }
    return std::nullopt;
}

std::optional<Models::User> UserRepository::findByUsername(const std::string& username) const {
    std::string lower = Utils::StringUtils::toLower(username);
    auto it = m_usernameIndex.find(lower);
    if (it != m_usernameIndex.end()) {
        return m_users[it->second];
    }
    return std::nullopt;
}

std::optional<Models::User> UserRepository::findByEmail(const std::string& email) const {
    std::string lower = Utils::StringUtils::toLower(email);
    for (const auto& u : m_users) {
        if (Utils::StringUtils::toLower(u.getEmail()) == lower) {
            return u;
        }
    }
    return std::nullopt;
}

bool UserRepository::save(const Models::User& entity) {
    if (existsById(entity.getId())) {
        return update(entity);
    }
    m_users.push_back(entity);
    rebuildIndex();
    return flush();
}

bool UserRepository::update(const Models::User& entity) {
    auto it = m_idIndex.find(entity.getId());
    if (it == m_idIndex.end()) return false;
    m_users[it->second] = entity;
    rebuildIndex();
    return flush();
}

bool UserRepository::remove(const std::string& id) {
    auto it = m_idIndex.find(id);
    if (it == m_idIndex.end()) return false;
    m_users.erase(m_users.begin() + it->second);
    rebuildIndex();
    return flush();
}

bool UserRepository::existsById(const std::string& id) const {
    return m_idIndex.find(id) != m_idIndex.end();
}

bool UserRepository::existsByUsername(const std::string& username) const {
    return m_usernameIndex.find(Utils::StringUtils::toLower(username)) != m_usernameIndex.end();
}

bool UserRepository::existsByEmail(const std::string& email) const {
    return findByEmail(email).has_value();
}

std::vector<Models::User> UserRepository::findMembers() const {
    std::vector<Models::User> members;
    for (const auto& u : m_users) {
        if (u.getRole() == Models::UserRole::Member) {
            members.push_back(u);
        }
    }
    return members;
}

std::string UserRepository::generateNextId() const {
    int maxNum = 0;
    for (const auto& u : m_users) {
        try {
            std::string id = u.getId();
            if (id.rfind("UID", 0) == 0) {
                id = id.substr(3);
            } else if (id.rfind("USR-", 0) == 0) {
                id = id.substr(4);
            }
            int n = std::stoi(id);
            if (n > maxNum) maxNum = n;
        } catch (...) {}
    }
    std::ostringstream oss;
    oss << "USR-" << std::setfill('0') << std::setw(3) << (maxNum + 1);
    return oss.str();
}

} // namespace LMS::Storage
