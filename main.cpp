/**
 * ============================================================
 *  Library Management System  v2.0
 *  Author  : Qamar Abbas
 *  Language: C++20
 *  Storage : Flat-file CSV (no external database required)
 * ============================================================
 */

#include "storage/BookRepository.h"
#include "storage/UserRepository.h"
#include "storage/LoanRepository.h"
#include "storage/ReservationRepository.h"
#include "storage/AuditLogger.h"
#include "services/AuthService.h"
#include "services/BookService.h"
#include "services/MemberService.h"
#include "services/CirculationService.h"
#include "services/ReportService.h"
#include "ui/LibraryApp.h"
#include <iostream>
#include <memory>

int main() {
    // ── Persistence layer (flat-file CSV) ─────────────────────────────────
    auto bookRepo    = std::make_shared<LMS::Storage::BookRepository>       ("data/books.csv");
    auto userRepo    = std::make_shared<LMS::Storage::UserRepository>       ("data/users.csv");
    auto loanRepo    = std::make_shared<LMS::Storage::LoanRepository>       ("data/loans.csv");
    auto resRepo     = std::make_shared<LMS::Storage::ReservationRepository>("data/reservations.csv");
    auto auditLogger = std::make_shared<LMS::Storage::AuditLogger>          ("logs/audit.log");

    // ── Service layer (business logic) ────────────────────────────────────
    auto authService = std::make_shared<LMS::Services::AuthService>          (userRepo, auditLogger);
    auto bookService = std::make_shared<LMS::Services::BookService>          (bookRepo, auditLogger);
    auto memService  = std::make_shared<LMS::Services::MemberService>        (userRepo, loanRepo, auditLogger);
    auto circService = std::make_shared<LMS::Services::CirculationService>   (bookRepo, userRepo, loanRepo, resRepo, auditLogger);
    auto repService  = std::make_shared<LMS::Services::ReportService>        (bookRepo, userRepo, loanRepo, auditLogger);
    auto recService  = std::make_shared<LMS::Services::RecommendationService>(bookRepo, loanRepo);

    // ── UI layer ──────────────────────────────────────────────────────────
    LMS::UI::LibraryApp app(authService, bookService, memService, circService, repService, recService);
    app.run();

    return 0;
}
