#pragma once

#include "storage/IRepository.hpp"
#include "models/Loan.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace LMS::Storage {

class LoanRepository : public IRepository<Models::Loan, std::string> {
private:
    std::string m_filepath;
    std::vector<Models::Loan> m_loans;
    std::unordered_map<std::string, size_t> m_idIndex;

    void rebuildIndex();

public:
    explicit LoanRepository(std::string filepath = "data/loans.csv");

    std::vector<Models::Loan> findAll() const override;
    std::optional<Models::Loan> findById(const std::string& id) const override;
    bool save(const Models::Loan& entity) override;
    bool update(const Models::Loan& entity) override;
    bool remove(const std::string& id) override;
    bool existsById(const std::string& id) const override;
    size_t count() const override { return m_loans.size(); }
    void reload() override;
    bool flush() override;

    std::vector<Models::Loan> findByUserId(const std::string& userId) const;
    std::vector<Models::Loan> findActiveLoansByUserId(const std::string& userId) const;
    std::vector<Models::Loan> findPendingLoans() const;
    std::vector<Models::Loan> findOverdueLoans(const Utils::Date& currentDate = Utils::Date::today()) const;
    std::string generateNextId() const;
};

} // namespace LMS::Storage
