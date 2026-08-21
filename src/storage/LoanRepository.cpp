#include "storage/LoanRepository.hpp"
#include "storage/CsvEngine.hpp"
#include <iomanip>
#include <sstream>

namespace LMS::Storage {

LoanRepository::LoanRepository(std::string filepath)
    : m_filepath(std::move(filepath)) {
    reload();
}

void LoanRepository::rebuildIndex() {
    m_idIndex.clear();
    for (size_t i = 0; i < m_loans.size(); ++i) {
        m_idIndex[m_loans[i].getLoanId()] = i;
    }
}

void LoanRepository::reload() {
    m_loans.clear();
    auto rows = CsvEngine::read(m_filepath);
    for (const auto& row : rows) {
        if (!row.empty()) {
            m_loans.push_back(Models::Loan::fromCSV(row));
        }
    }
    rebuildIndex();
}

bool LoanRepository::flush() {
    std::vector<std::vector<std::string>> rows;
    rows.reserve(m_loans.size());
    for (const auto& l : m_loans) {
        rows.push_back(l.toCSV());
    }
    return CsvEngine::write(m_filepath, rows);
}

std::vector<Models::Loan> LoanRepository::findAll() const {
    return m_loans;
}

std::optional<Models::Loan> LoanRepository::findById(const std::string& id) const {
    auto it = m_idIndex.find(id);
    if (it != m_idIndex.end()) {
        return m_loans[it->second];
    }
    return std::nullopt;
}

bool LoanRepository::save(const Models::Loan& entity) {
    if (existsById(entity.getLoanId())) {
        return update(entity);
    }
    m_loans.push_back(entity);
    rebuildIndex();
    return flush();
}

bool LoanRepository::update(const Models::Loan& entity) {
    auto it = m_idIndex.find(entity.getLoanId());
    if (it == m_idIndex.end()) return false;
    m_loans[it->second] = entity;
    return flush();
}

bool LoanRepository::remove(const std::string& id) {
    auto it = m_idIndex.find(id);
    if (it == m_idIndex.end()) return false;
    m_loans.erase(m_loans.begin() + it->second);
    rebuildIndex();
    return flush();
}

bool LoanRepository::existsById(const std::string& id) const {
    return m_idIndex.find(id) != m_idIndex.end();
}

std::vector<Models::Loan> LoanRepository::findByUserId(const std::string& userId) const {
    std::vector<Models::Loan> results;
    for (const auto& l : m_loans) {
        if (l.getUserId() == userId) {
            results.push_back(l);
        }
    }
    return results;
}

std::vector<Models::Loan> LoanRepository::findActiveLoansByUserId(const std::string& userId) const {
    std::vector<Models::Loan> results;
    for (const auto& l : m_loans) {
        if (l.getUserId() == userId && 
            (l.getStatus() == Models::LoanStatus::Approved || 
             l.getStatus() == Models::LoanStatus::Issued ||
             l.getStatus() == Models::LoanStatus::Overdue)) {
            results.push_back(l);
        }
    }
    return results;
}

std::vector<Models::Loan> LoanRepository::findPendingLoans() const {
    std::vector<Models::Loan> results;
    for (const auto& l : m_loans) {
        if (l.getStatus() == Models::LoanStatus::Requested) {
            results.push_back(l);
        }
    }
    return results;
}

std::vector<Models::Loan> LoanRepository::findOverdueLoans(const Utils::Date& currentDate) const {
    std::vector<Models::Loan> results;
    for (const auto& l : m_loans) {
        if (l.isOverdue(currentDate)) {
            results.push_back(l);
        }
    }
    return results;
}

std::string LoanRepository::generateNextId() const {
    int maxNum = 0;
    for (const auto& l : m_loans) {
        try {
            std::string id = l.getLoanId();
            if (id.rfind("LN-", 0) == 0) {
                id = id.substr(3);
            } else if (id.rfind("L", 0) == 0) {
                id = id.substr(1);
            }
            int n = std::stoi(id);
            if (n > maxNum) maxNum = n;
        } catch (...) {}
    }
    std::ostringstream oss;
    oss << "LN-" << std::setfill('0') << std::setw(4) << (maxNum + 1);
    return oss.str();
}

} // namespace LMS::Storage
