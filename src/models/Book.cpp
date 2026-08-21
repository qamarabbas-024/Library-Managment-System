#include "models/Book.hpp"
#include <algorithm>

namespace LMS::Models {

Book::Book(std::string id, std::string title, std::string author, std::string category, 
           int year, int totalCopies, std::string isbn, std::string shelf, int availableCopies, int borrowCount)
    : m_id(std::move(id)),
      m_isbn(std::move(isbn)),
      m_title(std::move(title)),
      m_author(std::move(author)),
      m_category(std::move(category)),
      m_publicationYear(year),
      m_shelfLocation(std::move(shelf)),
      m_totalCopies(std::max(1, totalCopies)),
      m_availableCopies(availableCopies >= 0 ? availableCopies : totalCopies),
      m_borrowCount(std::max(0, borrowCount)) {}

void Book::setTotalCopies(int copies) {
    int diff = copies - m_totalCopies;
    m_totalCopies = std::max(0, copies);
    m_availableCopies = std::max(0, m_availableCopies + diff);
}

bool Book::borrowCopy() {
    if (m_availableCopies > 0) {
        --m_availableCopies;
        ++m_borrowCount;
        return true;
    }
    return false;
}

bool Book::returnCopy() {
    if (m_availableCopies < m_totalCopies) {
        ++m_availableCopies;
        return true;
    }
    return false;
}

void Book::addStock(int count) {
    if (count > 0) {
        m_totalCopies += count;
        m_availableCopies += count;
    }
}

bool Book::removeStock(int count) {
    if (count > 0 && m_availableCopies >= count) {
        m_totalCopies -= count;
        m_availableCopies -= count;
        return true;
    }
    return false;
}

std::vector<std::string> Book::toCSV() const {
    return {
        m_id,
        m_isbn,
        m_title,
        m_author,
        m_category,
        std::to_string(m_publicationYear),
        m_shelfLocation,
        std::to_string(m_totalCopies),
        std::to_string(m_availableCopies),
        std::to_string(m_borrowCount)
    };
}

Book Book::fromCSV(const std::vector<std::string>& row) {
    if (row.size() < 6) return Book();

    // Support legacy 6-column format: code,title,author,category,year,copies
    if (row.size() == 6) {
        std::string id = row[0];
        std::string title = row[1];
        std::string author = row[2];
        std::string category = row[3];
        int year = 0;
        int copies = 1;
        try { year = std::stoi(row[4]); } catch (...) {}
        try { copies = std::stoi(row[5]); } catch (...) {}
        return Book(id, title, author, category, year, copies, "ISBN-N/A", "Aisle 1", copies, 0);
    }

    // Modern 10-column format
    std::string id = row[0];
    std::string isbn = row[1];
    std::string title = row[2];
    std::string author = row[3];
    std::string category = row[4];
    int year = 0, totalCopies = 1, availCopies = 1, borrowCount = 0;
    std::string shelf = (row.size() > 6) ? row[6] : "General Stack";

    try { if (row.size() > 5) year = std::stoi(row[5]); } catch (...) {}
    try { if (row.size() > 7) totalCopies = std::stoi(row[7]); } catch (...) {}
    try { if (row.size() > 8) availCopies = std::stoi(row[8]); } catch (...) {}
    try { if (row.size() > 9) borrowCount = std::stoi(row[9]); } catch (...) {}

    return Book(id, title, author, category, year, totalCopies, isbn, shelf, availCopies, borrowCount);
}

} // namespace LMS::Models
