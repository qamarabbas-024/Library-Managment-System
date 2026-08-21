#pragma once

#include "storage/BookRepository.h"
#include "storage/UserRepository.h"
#include "storage/LoanRepository.h"
#include "storage/ReservationRepository.h"
#include "storage/AuditLogger.h"
#include "models/Loan.h"
#include "models/Reservation.h"
#include <memory>
#include <string>
#include <vector>
#include <optional>

namespace LMS::Services {

struct CirculationResult {
    bool success{false};
    std::string message;
    std::optional<std::string> id;
};

class CirculationService {
private:
    std::shared_ptr<Storage::BookRepository> m_bookRepo;
    std::shared_ptr<Storage::UserRepository> m_userRepo;
    std::shared_ptr<Storage::LoanRepository> m_loanRepo;
    std::shared_ptr<Storage::ReservationRepository> m_reservationRepo;
    std::shared_ptr<Storage::AuditLogger> m_logger;

public:
    CirculationService(std::shared_ptr<Storage::BookRepository> bookRepo,
                       std::shared_ptr<Storage::UserRepository> userRepo,
                       std::shared_ptr<Storage::LoanRepository> loanRepo,
                       std::shared_ptr<Storage::ReservationRepository> reservationRepo,
                       std::shared_ptr<Storage::AuditLogger> logger);

    CirculationResult requestLoan(const std::string& userId, const std::string& bookId);
    CirculationResult approveLoan(const std::string& loanId, const std::string& adminId);
    CirculationResult rejectLoan(const std::string& loanId, const std::string& adminId);
    CirculationResult returnBook(const std::string& loanId, const std::string& userId = "");
    CirculationResult renewLoan(const std::string& loanId, const std::string& userId);

    CirculationResult reserveBook(const std::string& userId, const std::string& bookId);
    CirculationResult cancelReservation(const std::string& reservationId, const std::string& userId);

    CirculationResult payFine(const std::string& loanId, double amount, const std::string& adminId = "SYSTEM");

    std::vector<Models::Loan> getUserLoans(const std::string& userId) const;
    std::vector<Models::Loan> getUserActiveLoans(const std::string& userId) const;
    std::vector<Models::Loan> getPendingLoans() const;
    std::vector<Models::Loan> getOverdueLoans() const;
    std::vector<Models::Loan> getAllLoans() const;
    std::vector<Models::Reservation> getUserReservations(const std::string& userId) const;

    void updateAllFines();
};

} // namespace LMS::Services
