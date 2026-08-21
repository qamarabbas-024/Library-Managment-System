#pragma once

#include "storage/BookRepository.hpp"
#include "storage/AuditLogger.hpp"
#include "models/Book.hpp"
#include <memory>
#include <string>
#include <vector>
#include <optional>

namespace LMS::Services {

struct InventoryStats {
    size_t uniqueTitles{0};
    int totalCopies{0};
    int availableCopies{0};
    int borrowedCopies{0};
    std::vector<std::string> topCategories;
};

class BookService {
private:
    std::shared_ptr<Storage::BookRepository> m_bookRepo;
    std::shared_ptr<Storage::AuditLogger> m_logger;

public:
    BookService(std::shared_ptr<Storage::BookRepository> bookRepo,
                std::shared_ptr<Storage::AuditLogger> logger);

    std::optional<Models::Book> addBook(const std::string& title, const std::string& author,
                                        const std::string& category, int year, int totalCopies,
                                        const std::string& isbn = "", const std::string& shelf = "General Stack",
                                        const std::string& adminId = "SYSTEM");

    bool updateBook(const std::string& id, const std::string& title, const std::string& author,
                    const std::string& category, int year, const std::string& shelf,
                    int totalCopies, const std::string& adminId = "SYSTEM");

    bool deleteBook(const std::string& id, const std::string& adminId = "SYSTEM");

    std::optional<Models::Book> getBookById(const std::string& id) const;
    std::optional<Models::Book> getBookByIsbn(const std::string& isbn) const;
    std::vector<Models::Book> getAllBooks(const std::string& sortBy = "title") const;
    std::vector<Models::Book> searchBooks(const std::string& query) const;
    std::vector<Models::Book> getBooksByCategory(const std::string& category) const;
    std::vector<Models::Book> getAvailableBooks() const;
    std::vector<std::string> getCategories() const;

    bool restockBook(const std::string& id, int additionalCopies, const std::string& adminId = "SYSTEM");
    InventoryStats getInventoryStats() const;
};

} // namespace LMS::Services
