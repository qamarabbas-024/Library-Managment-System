#pragma once

#include "storage/IRepository.h"
#include "models/Book.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace LMS::Storage {

class BookRepository : public IRepository<Models::Book, std::string> {
private:
    std::string m_filepath;
    std::vector<Models::Book> m_books;
    std::unordered_map<std::string, size_t> m_idIndex;

    void rebuildIndex();

public:
    explicit BookRepository(std::string filepath = "data/books.csv");

    std::vector<Models::Book> findAll() const override;
    std::optional<Models::Book> findById(const std::string& id) const override;
    std::optional<Models::Book> findByIsbn(const std::string& isbn) const;
    bool save(const Models::Book& entity) override;
    bool update(const Models::Book& entity) override;
    bool remove(const std::string& id) override;
    bool existsById(const std::string& id) const override;
    size_t count() const override { return m_books.size(); }
    void reload() override;
    bool flush() override;

    std::vector<Models::Book> search(const std::string& query) const;
    std::vector<Models::Book> findByCategory(const std::string& category) const;
    std::vector<std::string> getAllCategories() const;
    std::string generateNextId() const;
};

} // namespace LMS::Storage
