#include "services/RecommendationService.h"
#include "utils/StringUtils.h"
#include <map>
#include <set>
#include <algorithm>

namespace LMS::Services {

RecommendationService::RecommendationService(std::shared_ptr<Storage::BookRepository> bookRepo,
                                             std::shared_ptr<Storage::LoanRepository> loanRepo)
    : m_bookRepo(std::move(bookRepo)), m_loanRepo(std::move(loanRepo)) {}

std::vector<Models::Book> RecommendationService::getRecommendationsForUser(const std::string& userId, size_t limit) const {
    auto userLoans = m_loanRepo->findByUserId(userId);
    std::set<std::string> readBookIds;
    std::map<std::string, int> preferredCategories;
    std::map<std::string, int> preferredAuthors;

    for (const auto& l : userLoans) {
        readBookIds.insert(l.getBookId());
        auto bookOpt = m_bookRepo->findById(l.getBookId());
        if (bookOpt.has_value()) {
            preferredCategories[bookOpt->getCategory()] += 2;
            preferredAuthors[bookOpt->getAuthor()] += 3;
        }
    }

    // If user has no loan history, return most popular available books
    if (userLoans.empty()) {
        return getTrendingBooks(limit);
    }

    auto allBooks = m_bookRepo->findAll();
    struct ScoredBook {
        Models::Book book;
        int score{0};
    };

    std::vector<ScoredBook> scored;
    for (const auto& b : allBooks) {
        // Skip books the user already read
        if (readBookIds.find(b.getId()) != readBookIds.end()) continue;

        int score = 0;
        if (preferredCategories.find(b.getCategory()) != preferredCategories.end()) {
            score += preferredCategories[b.getCategory()];
        }
        if (preferredAuthors.find(b.getAuthor()) != preferredAuthors.end()) {
            score += preferredAuthors[b.getAuthor()];
        }
        score += b.getBorrowCount(); // Popularity boost

        if (score > 0) {
            scored.push_back({b, score});
        }
    }

    std::sort(scored.begin(), scored.end(), [](const ScoredBook& a, const ScoredBook& b) {
        return a.score > b.score;
    });

    std::vector<Models::Book> recommendations;
    for (size_t i = 0; i < std::min(limit, scored.size()); ++i) {
        recommendations.push_back(scored[i].book);
    }

    // Fallback if not enough scored
    if (recommendations.size() < limit) {
        auto trending = getTrendingBooks(limit);
        for (const auto& b : trending) {
            if (readBookIds.find(b.getId()) == readBookIds.end()) {
                bool alreadyIn = false;
                for (const auto& r : recommendations) {
                    if (r.getId() == b.getId()) { alreadyIn = true; break; }
                }
                if (!alreadyIn) {
                    recommendations.push_back(b);
                    if (recommendations.size() >= limit) break;
                }
            }
        }
    }

    return recommendations;
}

std::vector<Models::Book> RecommendationService::getTrendingBooks(size_t limit) const {
    auto books = m_bookRepo->findAll();
    std::sort(books.begin(), books.end(), [](const Models::Book& a, const Models::Book& b) {
        return a.getBorrowCount() > b.getBorrowCount();
    });
    if (books.size() > limit) books.resize(limit);
    return books;
}

} // namespace LMS::Services
