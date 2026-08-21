#pragma once

#include "utils/DateTime.hpp"
#include <string>
#include <vector>

namespace LMS::Models {

enum class LoanStatus {
    Requested,
    Approved,
    Issued,
    Returned,
    Rejected,
    Overdue,
    Lost
};

class Loan {
private:
    std::string m_loanId;
    std::string m_userId;
    std::string m_bookId;
    std::string m_bookTitle;
    Utils::Date m_requestDate;
    Utils::Date m_issueDate;
    Utils::Date m_dueDate;
    Utils::Date m_returnDate;
    LoanStatus m_status{LoanStatus::Requested};
    double m_fineAmount{0.0};
    bool m_finePaid{false};
    int m_renewalCount{0};

public:
    Loan() = default;
    Loan(std::string loanId, std::string userId, std::string bookId, std::string bookTitle,
         Utils::Date reqDate, Utils::Date issueDate, Utils::Date dueDate, Utils::Date returnDate,
         LoanStatus status = LoanStatus::Requested, double fine = 0.0, bool finePaid = false, int renewals = 0);

    // Getters
    const std::string& getLoanId() const { return m_loanId; }
    const std::string& getUserId() const { return m_userId; }
    const std::string& getBookId() const { return m_bookId; }
    const std::string& getBookTitle() const { return m_bookTitle; }
    const Utils::Date& getRequestDate() const { return m_requestDate; }
    const Utils::Date& getIssueDate() const { return m_issueDate; }
    const Utils::Date& getDueDate() const { return m_dueDate; }
    const Utils::Date& getReturnDate() const { return m_returnDate; }
    LoanStatus getStatus() const { return m_status; }
    double getFineAmount() const { return m_fineAmount; }
    bool isFinePaid() const { return m_finePaid; }
    int getRenewalCount() const { return m_renewalCount; }

    std::string getStatusString() const;

    // Setters
    void setStatus(LoanStatus status) { m_status = status; }
    void setIssueDate(const Utils::Date& d) { m_issueDate = d; }
    void setDueDate(const Utils::Date& d) { m_dueDate = d; }
    void setReturnDate(const Utils::Date& d) { m_returnDate = d; }
    void setBookTitle(const std::string& t) { m_bookTitle = t; }
    void setFineAmount(double f) { m_fineAmount = f; }
    void setFinePaid(bool p) { m_finePaid = p; }

    // Domain operations
    double updateFine(const Utils::Date& currentDate = Utils::Date::today(), double dailyRate = 0.50);
    bool isOverdue(const Utils::Date& currentDate = Utils::Date::today()) const;
    bool canRenew(int maxRenewals = 2) const;
    bool renew(int extraDays = 14, int maxRenewals = 2);
    void approve(int loanPeriodDays = 14);
    void reject();
    void markReturned(const Utils::Date& returnDate = Utils::Date::today());

    // Serialization
    std::vector<std::string> toCSV() const;
    static Loan fromCSV(const std::vector<std::string>& row);
    static LoanStatus parseStatus(const std::string& str);
};

} // namespace LMS::Models
