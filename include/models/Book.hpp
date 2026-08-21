#pragma once

#include <string>
#include <vector>

namespace LMS::Models {

class Book {
private:
    std::string m_id;
    std::string m_isbn;
    std::string m_title;
    std::string m_author;
    std::string m_category;
    int m_publicationYear{0};
    std::string m_shelfLocation{"General Stack"};
    int m_totalCopies{1};
    int m_availableCopies{1};
    int m_borrowCount{0};

public:
    Book() = default;
    Book(std::string id, std::string title, std::string author, std::string category, 
         int year, int totalCopies, std::string isbn = "", std::string shelf = "General Stack", int availableCopies = -1, int borrowCount = 0);

    // Getters
    const std::string& getId() const { return m_id; }
    const std::string& getIsbn() const { return m_isbn; }
    const std::string& getTitle() const { return m_title; }
    const std::string& getAuthor() const { return m_author; }
    const std::string& getCategory() const { return m_category; }
    int getPublicationYear() const { return m_publicationYear; }
    const std::string& getShelfLocation() const { return m_shelfLocation; }
    int getTotalCopies() const { return m_totalCopies; }
    int getAvailableCopies() const { return m_availableCopies; }
    int getBorrowCount() const { return m_borrowCount; }

    // Setters
    void setTitle(const std::string& title) { m_title = title; }
    void setAuthor(const std::string& author) { m_author = author; }
    void setCategory(const std::string& category) { m_category = category; }
    void setIsbn(const std::string& isbn) { m_isbn = isbn; }
    void setPublicationYear(int year) { m_publicationYear = year; }
    void setShelfLocation(const std::string& shelf) { m_shelfLocation = shelf; }
    void setTotalCopies(int copies);

    // Domain methods
    bool isAvailable() const { return m_availableCopies > 0; }
    bool borrowCopy();
    bool returnCopy();
    void addStock(int count);
    bool removeStock(int count);

    // Serialization
    std::vector<std::string> toCSV() const;
    static Book fromCSV(const std::vector<std::string>& row);
};

} // namespace LMS::Models
