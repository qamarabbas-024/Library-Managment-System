#include "services/BookService.hpp"
#include "utils/StringUtils.hpp"
#include <algorithm>
#include <map>

namespace LMS::Services {

BookService::BookService(std::shared_ptr<Storage::BookRepository> bookRepo,
                         std::shared_ptr<Storage::AuditLogger> logger)
    : m_bookRepo(std::move(bookRepo)), m_logger(std::move(logger)) {}

std::optional<Models::Book> BookService::addBook(const std::string& title, const std::string& author,
                                                 const std::string& category, int year, int totalCopies,
                                                 const std::string& isbn, const std::string& shelf,
                                                 const std::string& adminId) {
    std::string cleanTitle = Utils::StringUtils::trim(title);
    std::string cleanAuthor = Utils::StringUtils::trim(author);
    if (cleanTitle.empty() || cleanAuthor.empty() || totalCopies <= 0) {
        if (m_logger) m_logger->warn("ADD_BOOK_FAILED", adminId, "Invalid title, author, or copy count");
        return std::nullopt;
    }

    std::string bookId = m_bookRepo->generateNextId();
    std::string cleanIsbn = isbn.empty() ? ("ISBN-" + bookId) : Utils::StringUtils::trim(isbn);
    std::string cleanCategory = category.empty() ? "General" : Utils::StringUtils::trim(category);
    std::string cleanShelf = shelf.empty() ? "General Stack" : Utils::StringUtils::trim(shelf);

    Models::Book newBook(bookId, cleanTitle, cleanAuthor, cleanCategory, year, totalCopies, cleanIsbn, cleanShelf);
    if (m_bookRepo->save(newBook)) {
        if (m_logger) m_logger->audit("ADD_BOOK", adminId, "Added book '" + cleanTitle + "' (" + bookId + ")");
        return newBook;
    }
    return std::nullopt;
}

bool BookService::updateBook(const std::string& id, const std::string& title, const std::string& author,
                             const std::string& category, int year, const std::string& shelf,
                             int totalCopies, const std::string& adminId) {
    auto bookOpt = m_bookRepo->findById(id);
    if (!bookOpt.has_value()) {
        if (m_logger) m_logger->warn("EDIT_BOOK_FAILED", adminId, "Book ID not found: " + id);
        return false;
    }

    Models::Book book = bookOpt.value();
    if (!title.empty()) book.setTitle(Utils::StringUtils::trim(title));
    if (!author.empty()) book.setAuthor(Utils::StringUtils::trim(author));
    if (!category.empty()) book.setCategory(Utils::StringUtils::trim(category));
    if (year > 0) book.setPublicationYear(year);
    if (!shelf.empty()) book.setShelfLocation(Utils::StringUtils::trim(shelf));
    if (totalCopies > 0) book.setTotalCopies(totalCopies);

    if (m_bookRepo->update(book)) {
        if (m_logger) m_logger->audit("UPDATE_BOOK", adminId, "Updated book details for ID: " + id);
        return true;
    }
    return false;
}

bool BookService::deleteBook(const std::string& id, const std::string& adminId) {
    auto bookOpt = m_bookRepo->findById(id);
    if (!bookOpt.has_value()) return false;

    // Check if any copies are currently borrowed
    if (bookOpt->getAvailableCopies() < bookOpt->getTotalCopies()) {
        if (m_logger) m_logger->warn("DELETE_BOOK_BLOCKED", adminId, "Cannot delete book with active loans: " + id);
        return false;
    }

    if (m_bookRepo->remove(id)) {
        if (m_logger) m_logger->audit("DELETE_BOOK", adminId, "Deleted book: " + id);
        return true;
    }
    return false;
}

std::optional<Models::Book> BookService::getBookById(const std::string& id) const {
    return m_bookRepo->findById(id);
}

std::optional<Models::Book> BookService::getBookByIsbn(const std::string& isbn) const {
    return m_bookRepo->findByIsbn(isbn);
}

std::vector<Models::Book> BookService::getAllBooks(const std::string& sortBy) const {
    auto books = m_bookRepo->findAll();
    if (sortBy == "title") {
        std::sort(books.begin(), books.end(), [](const Models::Book& a, const Models::Book& b) {
            return Utils::StringUtils::toLower(a.getTitle()) < Utils::StringUtils::toLower(b.getTitle());
        });
    } else if (sortBy == "author") {
        std::sort(books.begin(), books.end(), [](const Models::Book& a, const Models::Book& b) {
            return Utils::StringUtils::toLower(a.getAuthor()) < Utils::StringUtils::toLower(b.getAuthor());
        });
    } else if (sortBy == "year") {
        std::sort(books.begin(), books.end(), [](const Models::Book& a, const Models::Book& b) {
            return a.getPublicationYear() > b.getPublicationYear();
        });
    } else if (sortBy == "popularity") {
        std::sort(books.begin(), books.end(), [](const Models::Book& a, const Models::Book& b) {
            return a.getBorrowCount() > b.getBorrowCount();
        });
    }
    return books;
}

std::vector<Models::Book> BookService::searchBooks(const std::string& query) const {
    return m_bookRepo->search(query);
}

std::vector<Models::Book> BookService::getBooksByCategory(const std::string& category) const {
    return m_bookRepo->findByCategory(category);
}

std::vector<Models::Book> BookService::getAvailableBooks() const {
    std::vector<Models::Book> available;
    for (const auto& b : m_bookRepo->findAll()) {
        if (b.isAvailable()) {
            available.push_back(b);
        }
    }
    return available;
}

std::vector<std::string> BookService::getCategories() const {
    return m_bookRepo->getAllCategories();
}

bool BookService::restockBook(const std::string& id, int additionalCopies, const std::string& adminId) {
    if (additionalCopies <= 0) return false;
    auto bookOpt = m_bookRepo->findById(id);
    if (!bookOpt.has_value()) return false;

    Models::Book book = bookOpt.value();
    book.addStock(additionalCopies);
    if (m_bookRepo->update(book)) {
        if (m_logger) m_logger->audit("RESTOCK_BOOK", adminId, "Restocked " + std::to_string(additionalCopies) + " copies of book: " + id);
        return true;
    }
    return false;
}

InventoryStats BookService::getInventoryStats() const {
    InventoryStats stats;
    auto allBooks = m_bookRepo->findAll();
    stats.uniqueTitles = allBooks.size();
    
    std::map<std::string, int> catCounts;
    for (const auto& b : allBooks) {
        stats.totalCopies += b.getTotalCopies();
        stats.availableCopies += b.getAvailableCopies();
        stats.borrowedCopies += (b.getTotalCopies() - b.getAvailableCopies());
        catCounts[b.getCategory()]++;
    }

    std::vector<std::pair<std::string, int>> sortedCats(catCounts.begin(), catCounts.end());
    std::sort(sortedCats.begin(), sortedCats.end(), [](const auto& a, const auto& b) {
        return a.second > b.second;
    });

    for (size_t i = 0; i < std::min(size_t(5), sortedCats.size()); ++i) {
        stats.topCategories.push_back(sortedCats[i].first + " (" + std::to_string(sortedCats[i].second) + ")");
    }

    return stats;
}

} // namespace LMS::Services
