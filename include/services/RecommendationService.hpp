#pragma once

#include "storage/BookRepository.hpp"
#include "storage/LoanRepository.hpp"
#include "models/Book.hpp"
#include <memory>
#include <vector>
#include <string>

namespace LMS::Services {

class RecommendationService {
private:
    std::shared_ptr<Storage::BookRepository> m_bookRepo;
    std::shared_ptr<Storage::LoanRepository> m_loanRepo;

public:
    RecommendationService(std::shared_ptr<Storage::BookRepository> bookRepo,
                          std::shared_ptr<Storage::LoanRepository> loanRepo);

    // Recommends books based on member reading history (category affinity & author matching)
    std::vector<Models::Book> getRecommendationsForUser(const std::string& userId, size_t limit = 5) const;

    // Returns trending / popular books in the library
    std::vector<Models::Book> getTrendingBooks(size_t limit = 5) const;
};

} // namespace LMS::Services
