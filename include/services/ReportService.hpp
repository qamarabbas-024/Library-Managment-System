#pragma once

#include "storage/BookRepository.hpp"
#include "storage/UserRepository.hpp"
#include "storage/LoanRepository.hpp"
#include "storage/AuditLogger.hpp"
#include "models/Loan.hpp"
#include <memory>
#include <string>
#include <vector>

namespace LMS::Services {

struct DashboardMetrics {
    size_t totalBooks{0};
    int totalBookCopies{0};
    int availableCopies{0};
    int borrowedCopies{0};
    size_t totalMembers{0};
    size_t activeMembers{0};
    size_t totalLoans{0};
    size_t activeLoans{0};
    size_t pendingRequests{0};
    size_t overdueLoans{0};
    double totalFinesIncurred{0.0};
    double totalFinesCollected{0.0};
    double pendingFines{0.0};
};

class ReportService {
private:
    std::shared_ptr<Storage::BookRepository> m_bookRepo;
    std::shared_ptr<Storage::UserRepository> m_userRepo;
    std::shared_ptr<Storage::LoanRepository> m_loanRepo;
    std::shared_ptr<Storage::AuditLogger> m_logger;

public:
    ReportService(std::shared_ptr<Storage::BookRepository> bookRepo,
                  std::shared_ptr<Storage::UserRepository> userRepo,
                  std::shared_ptr<Storage::LoanRepository> loanRepo,
                  std::shared_ptr<Storage::AuditLogger> logger);

    DashboardMetrics getDashboardMetrics() const;
    std::vector<Models::Book> getMostPopularBooks(size_t limit = 10) const;
    std::vector<std::string> getRecentAuditLogs(size_t limit = 50) const;
};

} // namespace LMS::Services
