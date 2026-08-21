#include "services/CirculationService.h"
#include "utils/DateTime.h"
#include <sstream>
#include <iomanip>

namespace LMS::Services {

CirculationService::CirculationService(std::shared_ptr<Storage::BookRepository> bookRepo,
                                       std::shared_ptr<Storage::UserRepository> userRepo,
                                       std::shared_ptr<Storage::LoanRepository> loanRepo,
                                       std::shared_ptr<Storage::ReservationRepository> reservationRepo,
                                       std::shared_ptr<Storage::AuditLogger> logger)
    : m_bookRepo(std::move(bookRepo)),
      m_userRepo(std::move(userRepo)),
      m_loanRepo(std::move(loanRepo)),
      m_reservationRepo(std::move(reservationRepo)),
      m_logger(std::move(logger)) {}

CirculationResult CirculationService::requestLoan(const std::string& userId, const std::string& bookId) {
    auto userOpt = m_userRepo->findById(userId);
    if (!userOpt.has_value()) {
        return {false, "User account not found.", std::nullopt};
    }

    const auto& user = userOpt.value();
    if (!user.canBorrow()) {
        if (user.getStatus() != Models::UserStatus::Active) {
            return {false, "Account is not active (" + user.getStatusString() + "). Borrowing disabled.", std::nullopt};
        }
        return {false, "Borrowing limit reached (" + std::to_string(user.getActiveLoans()) + "/" + std::to_string(user.getMaxBorrowLimit()) + " books).", std::nullopt};
    }

    auto bookOpt = m_bookRepo->findById(bookId);
    if (!bookOpt.has_value()) {
        return {false, "Book not found in library catalog.", std::nullopt};
    }

    const auto& book = bookOpt.value();
    if (!book.isAvailable()) {
        return {false, "All copies of '" + book.getTitle() + "' are currently borrowed. You may place a reservation.", std::nullopt};
    }

    // Check if user already has an active or pending loan for this book
    auto userLoans = m_loanRepo->findByUserId(userId);
    for (const auto& l : userLoans) {
        if (l.getBookId() == bookId && 
            (l.getStatus() == Models::LoanStatus::Requested || 
             l.getStatus() == Models::LoanStatus::Approved || 
             l.getStatus() == Models::LoanStatus::Issued ||
             l.getStatus() == Models::LoanStatus::Overdue)) {
            return {false, "You already have an active or pending request for this book.", std::nullopt};
        }
    }

    std::string loanId = m_loanRepo->generateNextId();
    Utils::Date today = Utils::Date::today();
    Utils::Date dueDate = today.addDays(user.getMaxLoanDays());

    Models::Loan loan(loanId, userId, bookId, book.getTitle(), today, today, dueDate, Utils::Date(),
                      Models::LoanStatus::Requested, 0.0, false, 0);

    if (m_loanRepo->save(loan)) {
        if (m_logger) m_logger->audit("LOAN_REQUEST", userId, "Requested book '" + book.getTitle() + "' (" + bookId + ")");
        return {true, "Loan request submitted successfully. Awaiting librarian approval.", loanId};
    }

    return {false, "Failed to record loan request.", std::nullopt};
}

CirculationResult CirculationService::approveLoan(const std::string& loanId, const std::string& adminId) {
    auto loanOpt = m_loanRepo->findById(loanId);
    if (!loanOpt.has_value()) {
        return {false, "Loan record not found.", std::nullopt};
    }

    Models::Loan loan = loanOpt.value();
    if (loan.getStatus() != Models::LoanStatus::Requested) {
        return {false, "Loan is not in pending status.", std::nullopt};
    }

    auto bookOpt = m_bookRepo->findById(loan.getBookId());
    if (!bookOpt.has_value()) {
        return {false, "Associated book not found in catalog.", std::nullopt};
    }

    Models::Book book = bookOpt.value();
    if (!book.isAvailable()) {
        return {false, "Cannot approve: no copies available in inventory.", std::nullopt};
    }

    auto userOpt = m_userRepo->findById(loan.getUserId());
    if (!userOpt.has_value()) {
        return {false, "Member account not found.", std::nullopt};
    }

    Models::User user = userOpt.value();
    int loanDays = user.getMaxLoanDays();

    // Deduct book inventory & increment active loans
    book.borrowCopy();
    user.incrementActiveLoans();
    loan.approve(loanDays);
    loan.setBookTitle(book.getTitle());

    m_bookRepo->update(book);
    m_userRepo->update(user);
    m_loanRepo->update(loan);

    if (m_logger) m_logger->audit("LOAN_APPROVED", adminId, "Approved loan " + loanId + " for member " + user.getUsername());
    return {true, "Loan approved. Due date is " + loan.getDueDate().toString() + ".", loanId};
}

CirculationResult CirculationService::rejectLoan(const std::string& loanId, const std::string& adminId) {
    auto loanOpt = m_loanRepo->findById(loanId);
    if (!loanOpt.has_value()) return {false, "Loan not found.", std::nullopt};

    Models::Loan loan = loanOpt.value();
    loan.reject();
    m_loanRepo->update(loan);

    if (m_logger) m_logger->audit("LOAN_REJECTED", adminId, "Rejected loan " + loanId);
    return {true, "Loan request rejected.", loanId};
}

CirculationResult CirculationService::returnBook(const std::string& loanId, const std::string& userId) {
    auto loanOpt = m_loanRepo->findById(loanId);
    if (!loanOpt.has_value()) {
        return {false, "Loan ID not found.", std::nullopt};
    }

    Models::Loan loan = loanOpt.value();
    if (!userId.empty() && loan.getUserId() != userId) {
        return {false, "Unauthorized: this loan does not belong to you.", std::nullopt};
    }

    if (loan.getStatus() != Models::LoanStatus::Approved && 
        loan.getStatus() != Models::LoanStatus::Issued &&
        loan.getStatus() != Models::LoanStatus::Overdue) {
        return {false, "This loan is already closed or not active.", std::nullopt};
    }

    Utils::Date today = Utils::Date::today();
    loan.markReturned(today);

    // Return copy to book inventory
    auto bookOpt = m_bookRepo->findById(loan.getBookId());
    if (bookOpt.has_value()) {
        Models::Book book = bookOpt.value();
        book.returnCopy();
        m_bookRepo->update(book);
    }

    // Decrement user active loans
    auto userOpt = m_userRepo->findById(loan.getUserId());
    if (userOpt.has_value()) {
        Models::User user = userOpt.value();
        user.decrementActiveLoans();
        m_userRepo->update(user);
    }

    m_loanRepo->update(loan);

    // Check if there are pending reservations for this book
    auto pendingRes = m_reservationRepo->findPendingByBookId(loan.getBookId());
    if (!pendingRes.empty()) {
        auto topRes = pendingRes.front();
        topRes.setStatus(Models::ReservationStatus::Notified);
        m_reservationRepo->update(topRes);
        if (m_logger) m_logger->info("RESERVATION_NOTIFIED", topRes.getUserId(), "Book " + loan.getBookId() + " is now available for reserved member");
    }

    std::ostringstream msg;
    msg << "Book returned successfully.";
    if (loan.getFineAmount() > 0) {
        msg << " Overdue fine incurred: $" << std::fixed << std::setprecision(2) << loan.getFineAmount() << ".";
    }

    if (m_logger) m_logger->audit("BOOK_RETURNED", loan.getUserId(), "Returned book for loan " + loanId);
    return {true, msg.str(), loanId};
}

CirculationResult CirculationService::renewLoan(const std::string& loanId, const std::string& userId) {
    auto loanOpt = m_loanRepo->findById(loanId);
    if (!loanOpt.has_value()) return {false, "Loan not found.", std::nullopt};

    Models::Loan loan = loanOpt.value();
    if (loan.getUserId() != userId) return {false, "Unauthorized.", std::nullopt};

    // Check if book has pending reservations
    auto pendingRes = m_reservationRepo->findPendingByBookId(loan.getBookId());
    if (!pendingRes.empty()) {
        return {false, "Renewal denied: other members are currently waiting on the reservation waitlist.", std::nullopt};
    }

    if (!loan.canRenew(2)) {
        if (loan.isOverdue()) return {false, "Cannot renew overdue loan. Please return book and pay outstanding fines.", std::nullopt};
        return {false, "Maximum renewals (2) reached for this loan.", std::nullopt};
    }

    loan.renew(14, 2);
    m_loanRepo->update(loan);

    if (m_logger) m_logger->audit("LOAN_RENEWED", userId, "Renewed loan " + loanId + " until " + loan.getDueDate().toString());
    return {true, "Loan renewed successfully. New due date is " + loan.getDueDate().toString() + ".", loanId};
}

CirculationResult CirculationService::reserveBook(const std::string& userId, const std::string& bookId) {
    auto bookOpt = m_bookRepo->findById(bookId);
    if (!bookOpt.has_value()) return {false, "Book not found.", std::nullopt};

    std::string resId = m_reservationRepo->generateNextId();
    Models::Reservation res(resId, userId, bookId, Utils::Date::today(), Models::ReservationStatus::Pending);

    if (m_reservationRepo->save(res)) {
        if (m_logger) m_logger->audit("BOOK_RESERVED", userId, "Reserved book: " + bookId);
        return {true, "Reservation placed. You will be notified when a copy is returned.", resId};
    }
    return {false, "Failed to place reservation.", std::nullopt};
}

CirculationResult CirculationService::cancelReservation(const std::string& reservationId, const std::string& userId) {
    auto resOpt = m_reservationRepo->findById(reservationId);
    if (!resOpt.has_value()) return {false, "Reservation not found.", std::nullopt};

    Models::Reservation res = resOpt.value();
    if (res.getUserId() != userId) return {false, "Unauthorized.", std::nullopt};

    res.setStatus(Models::ReservationStatus::Cancelled);
    m_reservationRepo->update(res);
    return {true, "Reservation cancelled.", reservationId};
}

CirculationResult CirculationService::payFine(const std::string& loanId, double amount, const std::string& adminId) {
    auto loanOpt = m_loanRepo->findById(loanId);
    if (!loanOpt.has_value()) return {false, "Loan not found.", std::nullopt};

    Models::Loan loan = loanOpt.value();
    if (loan.getFineAmount() <= 0 || loan.isFinePaid()) {
        return {false, "No outstanding fines on this loan record.", std::nullopt};
    }

    if (amount >= loan.getFineAmount()) {
        loan.setFinePaid(true);
        m_loanRepo->update(loan);
        if (m_logger) m_logger->audit("FINE_PAID", adminId, "Fine paid in full ($" + std::to_string(loan.getFineAmount()) + ") for loan " + loanId);
        return {true, "Fine paid in full.", loanId};
    } else {
        loan.setFineAmount(loan.getFineAmount() - amount);
        m_loanRepo->update(loan);
        if (m_logger) m_logger->audit("FINE_PARTIAL_PAYMENT", adminId, "Partial fine paid for loan " + loanId);
        return {true, "Partial payment recorded. Remaining balance: $" + std::to_string(loan.getFineAmount()), loanId};
    }
}

std::vector<Models::Loan> CirculationService::getUserLoans(const std::string& userId) const {
    return m_loanRepo->findByUserId(userId);
}

std::vector<Models::Loan> CirculationService::getUserActiveLoans(const std::string& userId) const {
    return m_loanRepo->findActiveLoansByUserId(userId);
}

std::vector<Models::Loan> CirculationService::getPendingLoans() const {
    return m_loanRepo->findPendingLoans();
}

std::vector<Models::Loan> CirculationService::getOverdueLoans() const {
    return m_loanRepo->findOverdueLoans();
}

std::vector<Models::Loan> CirculationService::getAllLoans() const {
    return m_loanRepo->findAll();
}

std::vector<Models::Reservation> CirculationService::getUserReservations(const std::string& userId) const {
    return m_reservationRepo->findByUserId(userId);
}

void CirculationService::updateAllFines() {
    auto loans = m_loanRepo->findAll();
    Utils::Date today = Utils::Date::today();
    for (auto& l : loans) {
        l.updateFine(today);
        m_loanRepo->update(l);
    }
}

} // namespace LMS::Services
