#include "services/ReportService.h"
#include "utils/DateTime.h"
#include <algorithm>

namespace LMS::Services {

ReportService::ReportService(std::shared_ptr<Storage::BookRepository> bookRepo,
                             std::shared_ptr<Storage::UserRepository> userRepo,
                             std::shared_ptr<Storage::LoanRepository> loanRepo,
                             std::shared_ptr<Storage::AuditLogger> logger)
    : m_bookRepo(std::move(bookRepo)),
      m_userRepo(std::move(userRepo)),
      m_loanRepo(std::move(loanRepo)),
      m_logger(std::move(logger)) {}

DashboardMetrics ReportService::getDashboardMetrics() const {
    DashboardMetrics m;
    Utils::Date today = Utils::Date::today();

    auto books = m_bookRepo->findAll();
    m.totalBooks = books.size();
    for (const auto& b : books) {
        m.totalBookCopies += b.getTotalCopies();
        m.availableCopies += b.getAvailableCopies();
        m.borrowedCopies += (b.getTotalCopies() - b.getAvailableCopies());
    }

    auto members = m_userRepo->findMembers();
    m.totalMembers = members.size();
    for (const auto& u : members) {
        if (u.getStatus() == Models::UserStatus::Active) m.activeMembers++;
    }

    auto loans = m_loanRepo->findAll();
    m.totalLoans = loans.size();
    for (auto& l : loans) {
        auto status = l.getStatus();
        if (status == Models::LoanStatus::Approved ||
            status == Models::LoanStatus::Issued ||
            status == Models::LoanStatus::Overdue) {
            m.activeLoans++;
        }
        if (status == Models::LoanStatus::Requested) m.pendingRequests++;
        if (l.isOverdue(today)) m.overdueLoans++;

        if (l.getFineAmount() > 0) {
            m.totalFinesIncurred += l.getFineAmount();
            if (l.isFinePaid()) m.totalFinesCollected += l.getFineAmount();
            else m.pendingFines += l.getFineAmount();
        }
    }

    return m;
}

std::vector<Models::Book> ReportService::getMostPopularBooks(size_t limit) const {
    auto books = m_bookRepo->findAll();
    std::sort(books.begin(), books.end(), [](const Models::Book& a, const Models::Book& b) {
        return a.getBorrowCount() > b.getBorrowCount();
    });
    if (books.size() > limit) books.resize(limit);
    return books;
}

std::vector<std::string> ReportService::getRecentAuditLogs(size_t limit) const {
    if (m_logger) return m_logger->getRecentLogs(limit);
    return {};
}

} // namespace LMS::Services
