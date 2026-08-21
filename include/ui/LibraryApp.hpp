#pragma once

#include "services/AuthService.hpp"
#include "services/BookService.hpp"
#include "services/MemberService.hpp"
#include "services/CirculationService.hpp"
#include "services/ReportService.hpp"
#include <memory>

namespace LMS::UI {

class LibraryApp {
private:
    std::shared_ptr<Services::AuthService>        m_auth;
    std::shared_ptr<Services::BookService>        m_books;
    std::shared_ptr<Services::MemberService>      m_members;
    std::shared_ptr<Services::CirculationService> m_circ;
    std::shared_ptr<Services::ReportService>      m_reports;

    // ── Main menu screens ─────────────────────────────────────────────────
    void showWelcomeScreen();
    void runLoginFlow();
    void runSignupFlow();
    void runForgotPasswordFlow();

    // ── Admin flows ───────────────────────────────────────────────────────
    void runAdminDashboard();
    void adminBookManagement();
    void adminMemberManagement();
    void adminCirculationManagement();
    void adminReports();

    void adminAddBook();
    void adminEditBook();
    void adminDeleteBook();
    void adminListBooks();
    void adminRestockBook();

    void adminListMembers();
    void adminAddMember();
    void adminEditMember();
    void adminDeleteMember();
    void adminBanUnbanMember();

    void adminPendingLoans();
    void adminAllLoans();
    void adminOverdueLoans();
    void adminPayFine();

    void adminDashboardStats();
    void adminAuditLog();
    void adminPopularBooks();

    // ── Member flows ──────────────────────────────────────────────────────
    void runMemberDashboard();
    void memberSearchBooks();
    void memberRequestLoan();
    void memberMyLoans();
    void memberReturnBook();
    void memberRenewLoan();
    void memberReserveBook();
    void memberMyReservations();
    void memberUpdateProfile();
    void memberChangePassword();

    // ── Shared helpers ────────────────────────────────────────────────────
    void printBookTable(const std::vector<Models::Book>& books) const;
    void printLoanTable(const std::vector<Models::Loan>& loans, bool showUser = false) const;
    void printMemberTable(const std::vector<Models::User>& members) const;

public:
    LibraryApp(std::shared_ptr<Services::AuthService> auth,
               std::shared_ptr<Services::BookService> books,
               std::shared_ptr<Services::MemberService> members,
               std::shared_ptr<Services::CirculationService> circ,
               std::shared_ptr<Services::ReportService> reports);

    void run();
};

} // namespace LMS::UI
