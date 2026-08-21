#include "models/Loan.h"
#include "utils/StringUtils.h"
#include <algorithm>

namespace LMS::Models {

Loan::Loan(std::string loanId, std::string userId, std::string bookId, std::string bookTitle,
           Utils::Date reqDate, Utils::Date issueDate, Utils::Date dueDate, Utils::Date returnDate,
           LoanStatus status, double fine, bool finePaid, int renewals)
    : m_loanId(std::move(loanId)),
      m_userId(std::move(userId)),
      m_bookId(std::move(bookId)),
      m_bookTitle(std::move(bookTitle)),
      m_requestDate(reqDate),
      m_issueDate(issueDate),
      m_dueDate(dueDate),
      m_returnDate(returnDate),
      m_status(status),
      m_fineAmount(fine),
      m_finePaid(finePaid),
      m_renewalCount(renewals) {}

std::string Loan::getStatusString() const {
    switch (m_status) {
        case LoanStatus::Requested: return "pending";
        case LoanStatus::Approved:  return "approved";
        case LoanStatus::Issued:    return "issued";
        case LoanStatus::Returned:  return "returned";
        case LoanStatus::Rejected:  return "rejected";
        case LoanStatus::Overdue:   return "overdue";
        case LoanStatus::Lost:      return "lost";
    }
    return "pending";
}

LoanStatus Loan::parseStatus(const std::string& str) {
    std::string s = Utils::StringUtils::toLower(str);
    if (s == "approved") return LoanStatus::Approved;
    if (s == "issued") return LoanStatus::Issued;
    if (s == "returned") return LoanStatus::Returned;
    if (s == "rejected") return LoanStatus::Rejected;
    if (s == "overdue") return LoanStatus::Overdue;
    if (s == "lost") return LoanStatus::Lost;
    return LoanStatus::Requested;
}

double Loan::updateFine(const Utils::Date& currentDate, double dailyRate) {
    if (m_finePaid) return 0.0;
    if (m_status == LoanStatus::Returned) {
        if (m_returnDate > m_dueDate) {
            int overdueDays = m_dueDate.daysUntil(m_returnDate);
            if (overdueDays > 0) {
                m_fineAmount = overdueDays * dailyRate;
            }
        }
        return m_fineAmount;
    }

    if (m_status == LoanStatus::Approved || m_status == LoanStatus::Issued || m_status == LoanStatus::Overdue) {
        if (currentDate > m_dueDate) {
            int overdueDays = m_dueDate.daysUntil(currentDate);
            if (overdueDays > 0) {
                m_fineAmount = overdueDays * dailyRate;
                m_status = LoanStatus::Overdue;
            }
        }
    }
    return m_fineAmount;
}

bool Loan::isOverdue(const Utils::Date& currentDate) const {
    if (m_status == LoanStatus::Returned || m_status == LoanStatus::Rejected || m_status == LoanStatus::Requested) {
        return false;
    }
    return currentDate > m_dueDate;
}

bool Loan::canRenew(int maxRenewals) const {
    return (m_status == LoanStatus::Approved || m_status == LoanStatus::Issued) &&
           (m_renewalCount < maxRenewals) &&
           (!isOverdue());
}

bool Loan::renew(int extraDays, int maxRenewals) {
    if (canRenew(maxRenewals)) {
        m_dueDate = m_dueDate.addDays(extraDays);
        ++m_renewalCount;
        return true;
    }
    return false;
}

void Loan::approve(int loanPeriodDays) {
    m_status = LoanStatus::Approved;
    m_issueDate = Utils::Date::today();
    m_dueDate = m_issueDate.addDays(loanPeriodDays);
}

void Loan::reject() {
    m_status = LoanStatus::Rejected;
}

void Loan::markReturned(const Utils::Date& returnDate) {
    m_status = LoanStatus::Returned;
    m_returnDate = returnDate;
    updateFine(returnDate);
}

std::vector<std::string> Loan::toCSV() const {
    return {
        m_loanId,
        m_userId,
        m_bookId,
        m_bookTitle,
        m_requestDate.toString(),
        m_issueDate.toString(),
        m_dueDate.toString(),
        m_returnDate.toString(),
        getStatusString(),
        std::to_string(m_fineAmount),
        m_finePaid ? "1" : "0",
        std::to_string(m_renewalCount)
    };
}

Loan Loan::fromCSV(const std::vector<std::string>& row) {
    if (row.size() < 4) return Loan();

    // Legacy format: loanID, uid, bookID, status
    if (row.size() == 4) {
        Utils::Date today = Utils::Date::today();
        return Loan(
            row[0], row[1], row[2], "",
            today, today, today.addDays(14), today,
            parseStatus(row[3]), 0.0, false, 0
        );
    }

    // Modern format: 12 fields
    std::string loanId = row[0];
    std::string uid = row[1];
    std::string bookId = row[2];
    std::string title = (row.size() > 3) ? row[3] : "";
    Utils::Date reqDate = (row.size() > 4) ? Utils::Date::fromString(row[4]) : Utils::Date::today();
    Utils::Date issueDate = (row.size() > 5) ? Utils::Date::fromString(row[5]) : reqDate;
    Utils::Date dueDate = (row.size() > 6) ? Utils::Date::fromString(row[6]) : issueDate.addDays(14);
    Utils::Date retDate = (row.size() > 7) ? Utils::Date::fromString(row[7]) : Utils::Date();
    LoanStatus status = (row.size() > 8) ? parseStatus(row[8]) : LoanStatus::Requested;
    double fine = 0.0;
    bool finePaid = false;
    int renewals = 0;

    try { if (row.size() > 9) fine = std::stod(row[9]); } catch (...) {}
    try { if (row.size() > 10) finePaid = (row[10] == "1" || row[10] == "true"); } catch (...) {}
    try { if (row.size() > 11) renewals = std::stoi(row[11]); } catch (...) {}

    return Loan(loanId, uid, bookId, title, reqDate, issueDate, dueDate, retDate, status, fine, finePaid, renewals);
}

} // namespace LMS::Models
