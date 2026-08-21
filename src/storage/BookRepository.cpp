#include "storage/BookRepository.hpp"
#include "storage/CsvEngine.hpp"
#include "utils/StringUtils.hpp"
#include <iomanip>
#include <sstream>
#include <set>

namespace LMS::Storage {

BookRepository::BookRepository(std::string filepath)
    : m_filepath(std::move(filepath)) {
    reload();
}

void BookRepository::rebuildIndex() {
    m_idIndex.clear();
    for (size_t i = 0; i < m_books.size(); ++i) {
        m_idIndex[m_books[i].getId()] = i;
    }
}

void BookRepository::reload() {
    m_books.clear();
    auto rows = CsvEngine::read(m_filepath);
    for (const auto& row : rows) {
        if (!row.empty()) {
            m_books.push_back(Models::Book::fromCSV(row));
        }
    }
    rebuildIndex();
}

bool BookRepository::flush() {
    std::vector<std::vector<std::string>> rows;
    rows.reserve(m_books.size());
    for (const auto& b : m_books) {
        rows.push_back(b.toCSV());
    }
    return CsvEngine::write(m_filepath, rows);
}

std::vector<Models::Book> BookRepository::findAll() const {
    return m_books;
}

std::optional<Models::Book> BookRepository::findById(const std::string& id) const {
    auto it = m_idIndex.find(id);
    if (it != m_idIndex.end()) {
        return m_books[it->second];
    }
    return std::nullopt;
}

std::optional<Models::Book> BookRepository::findByIsbn(const std::string& isbn) const {
    if (isbn.empty() || isbn == "ISBN-N/A") return std::nullopt;
    for (const auto& b : m_books) {
        if (b.getIsbn() == isbn) return b;
    }
    return std::nullopt;
}

bool BookRepository::save(const Models::Book& entity) {
    if (existsById(entity.getId())) {
        return update(entity);
    }
    m_books.push_back(entity);
    rebuildIndex();
    return flush();
}

bool BookRepository::update(const Models::Book& entity) {
    auto it = m_idIndex.find(entity.getId());
    if (it == m_idIndex.end()) return false;
    m_books[it->second] = entity;
    return flush();
}

bool BookRepository::remove(const std::string& id) {
    auto it = m_idIndex.find(id);
    if (it == m_idIndex.end()) return false;
    m_books.erase(m_books.begin() + it->second);
    rebuildIndex();
    return flush();
}

bool BookRepository::existsById(const std::string& id) const {
    return m_idIndex.find(id) != m_idIndex.end();
}

std::vector<Models::Book> BookRepository::search(const std::string& query) const {
    if (query.empty()) return m_books;
    std::vector<Models::Book> results;
    for (const auto& b : m_books) {
        if (Utils::StringUtils::fuzzyMatch(b.getTitle(), query) ||
            Utils::StringUtils::fuzzyMatch(b.getAuthor(), query) ||
            Utils::StringUtils::fuzzyMatch(b.getCategory(), query) ||
            Utils::StringUtils::containsIgnoreCase(b.getId(), query) ||
            Utils::StringUtils::containsIgnoreCase(b.getIsbn(), query)) {
            results.push_back(b);
        }
    }
    return results;
}

std::vector<Models::Book> BookRepository::findByCategory(const std::string& category) const {
    std::vector<Models::Book> results;
    for (const auto& b : m_books) {
        if (Utils::StringUtils::containsIgnoreCase(b.getCategory(), category)) {
            results.push_back(b);
        }
    }
    return results;
}

std::vector<std::string> BookRepository::getAllCategories() const {
    std::set<std::string> cats;
    for (const auto& b : m_books) {
        if (!b.getCategory().empty()) {
            cats.insert(b.getCategory());
        }
    }
    return std::vector<std::string>(cats.begin(), cats.end());
}

std::string BookRepository::generateNextId() const {
    int maxNum = 0;
    for (const auto& b : m_books) {
        try {
            std::string id = b.getId();
            if (id.rfind("BK-", 0) == 0) {
                id = id.substr(3);
            }
            int n = std::stoi(id);
            if (n > maxNum) maxNum = n;
        } catch (...) {}
    }
    std::ostringstream oss;
    oss << "BK-" << std::setfill('0') << std::setw(3) << (maxNum + 1);
    return oss.str();
}

} // namespace LMS::Storage
