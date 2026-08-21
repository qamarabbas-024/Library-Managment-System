/**
 * ============================================================
 *  Library Management System - Automated Unit Tests (C++20)
 *  Author: Qamar Abbas
 * ============================================================
 */

#include "utils/DateTime.h"
#include "utils/Crypto.h"
#include "utils/StringUtils.h"
#include "models/Book.h"
#include "models/User.h"
#include "models/Loan.h"
#include "models/Reservation.h"
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

#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <filesystem>

namespace fs = std::filesystem;

class TestRunner {
private:
    int m_passed{0};
    int m_failed{0};
    std::vector<std::string> m_failedNames;

public:
    void runTest(const std::string& testName, const std::function<bool()>& testFunc) {
        std::cout << "  [TEST] " << testName << " ... ";
        try {
            if (testFunc()) {
                std::cout << "\033[32m[PASSED]\033[0m\n";
                ++m_passed;
            } else {
                std::cout << "\033[31m[FAILED]\033[0m\n";
                ++m_failed;
                m_failedNames.push_back(testName);
            }
        } catch (const std::exception& e) {
            std::cout << "\033[31m[FAILED - EXCEPTION: " << e.what() << "]\033[0m\n";
            ++m_failed;
            m_failedNames.push_back(testName + " (" + e.what() + ")");
        } catch (...) {
            std::cout << "\033[31m[FAILED - UNKNOWN EXCEPTION]\033[0m\n";
            ++m_failed;
            m_failedNames.push_back(testName + " (unknown exception)");
        }
    }

    void printSummary() const {
        std::cout << "\n==================================================\n";
        std::cout << "  TEST SUMMARY:\n";
        std::cout << "  Total Tests : " << (m_passed + m_failed) << "\n";
        std::cout << "  Passed      : " << m_passed << "\n";
        std::cout << "  Failed      : " << m_failed << "\n";
        if (!m_failedNames.empty()) {
            std::cout << "  Failed list:\n";
            for (const auto& name : m_failedNames) {
                std::cout << "    - " << name << "\n";
            }
        }
        std::cout << "==================================================\n";
    }

    int getExitCode() const { return m_failed > 0 ? 1 : 0; }
};

int main() {
    TestRunner runner;

    std::cout << "\n==================================================\n";
    std::cout << "  RUNNING LMS AUTOMATED UNIT TESTS (C++20)\n";
    std::cout << "  Author: Qamar Abbas\n";
    std::cout << "==================================================\n\n";

    // ── 1. Date and Time Tests ────────────────────────────────────────────
    runner.runTest("Date::today and string conversion", []() {
        LMS::Utils::Date today = LMS::Utils::Date::today();
        std::string s = today.toString();
        LMS::Utils::Date parsed = LMS::Utils::Date::fromString(s);
        return parsed.toString() == s;
    });

    runner.runTest("Date arithmetic (addDays & daysUntil)", []() {
        LMS::Utils::Date d1(2026, 1, 1);
        LMS::Utils::Date d2 = d1.addDays(14);
        return d2.toString() == "2026-01-15" && d1.daysUntil(d2) == 14;
    });

    runner.runTest("Date comparison operators", []() {
        LMS::Utils::Date past(2025, 12, 31);
        LMS::Utils::Date future(2026, 1, 1);
        return past < future && future > past && past != future;
    });

    // ── 2. Cryptographic Tests ────────────────────────────────────────────
    runner.runTest("SHA-256 deterministic hash", []() {
        std::string hash = LMS::Utils::Crypto::sha256("hello");
        return hash == "2cf24dba5fb0a30e26e83b2ac5b9e29e1b161e5c1fa7425e73043362938b9824";
    });

    runner.runTest("Salted password hashing & verification", []() {
        std::string salt = LMS::Utils::Crypto::generateSalt(16);
        std::string pass = "SecurePass123";
        std::string hash = LMS::Utils::Crypto::hashPassword(pass, salt);
        return LMS::Utils::Crypto::verifyPassword(pass, hash, salt) &&
               !LMS::Utils::Crypto::verifyPassword("WrongPass", hash, salt);
    });

    // ── 3. String Utilities Tests ─────────────────────────────────────────
    runner.runTest("StringUtils trim & case conversion", []() {
        std::string s = "  Library Project  ";
        return LMS::Utils::StringUtils::trim(s) == "Library Project" &&
               LMS::Utils::StringUtils::toLower("HeLLo") == "hello" &&
               LMS::Utils::StringUtils::toUpper("hello") == "HELLO";
    });

    runner.runTest("StringUtils substring search (case-insensitive)", []() {
        return LMS::Utils::StringUtils::containsIgnoreCase("Clean Architecture", "clean") &&
               LMS::Utils::StringUtils::containsIgnoreCase("C++ Programming", "PROGRAMMING") &&
               !LMS::Utils::StringUtils::containsIgnoreCase("Python", "Java");
    });

    runner.runTest("CSV line parser with quoted commas", []() {
        std::string line = "BK-001,\"Design Patterns: Elements, Reusable\",Gamma,CS,1994,3";
        auto fields = LMS::Utils::StringUtils::parseCSVLine(line);
        return fields.size() == 6 &&
               fields[0] == "BK-001" &&
               fields[1] == "Design Patterns: Elements, Reusable" &&
               fields[2] == "Gamma";
    });

    // ── 4. Book Model Invariants ──────────────────────────────────────────
    runner.runTest("Book stock & borrowing invariants", []() {
        LMS::Models::Book book("BK-100", "Effective Modern C++", "Scott Meyers", "Programming", 2014, 2);
        if (!book.isAvailable() || book.getAvailableCopies() != 2) return false;
        
        bool b1 = book.borrowCopy();
        if (!b1 || book.getAvailableCopies() != 1 || book.getBorrowCount() != 1) return false;

        bool b2 = book.borrowCopy();
        if (!b2 || book.getAvailableCopies() != 0 || book.isAvailable()) return false;

        bool b3 = book.borrowCopy(); // Should fail (0 copies left)
        if (b3) return false;

        book.returnCopy();
        return book.getAvailableCopies() == 1 && book.isAvailable();
    });

    // ── 5. User Roles and Borrowing Limits ─────────────────────────────────
    runner.runTest("User role borrowing limits & policies", []() {
        LMS::Models::User student("USR-1", "student_user", "hash", "s@test.com", "123",
                                  LMS::Models::UserRole::Member, LMS::Models::UserStatus::Active,
                                  LMS::Models::MembershipType::Student);
        LMS::Models::User faculty("USR-2", "faculty_user", "hash", "f@test.com", "456",
                                  LMS::Models::UserRole::Member, LMS::Models::UserStatus::Active,
                                  LMS::Models::MembershipType::Faculty);

        return student.getMaxBorrowLimit() == 4 &&
               student.getMaxLoanDays() == 14 &&
               faculty.getMaxBorrowLimit() == 8 &&
               faculty.getMaxLoanDays() == 30;
    });

    // ── 6. Loan Lifecycle & Fines ─────────────────────────────────────────
    runner.runTest("Loan due date and overdue fine calculation", []() {
        LMS::Utils::Date reqDate(2026, 1, 1);
        LMS::Utils::Date issueDate(2026, 1, 1);
        LMS::Utils::Date dueDate(2026, 1, 15);
        LMS::Utils::Date returnDate(2026, 1, 20); // 5 days late

        LMS::Models::Loan loan("LN-001", "USR-1", "BK-001", "Clean Code",
                               reqDate, issueDate, dueDate, returnDate,
                               LMS::Models::LoanStatus::Issued);

        double fine = loan.updateFine(returnDate, 0.50); // 5 days * $0.50 = $2.50
        return fine >= 2.49 && fine <= 2.51;
    });

    // ── 7. Full Circulation Flow ──────────────────────────────────────────
    runner.runTest("Full circulation flow (request -> approve -> return)", []() {
        std::string testDir = "build/test_data";
        fs::create_directories(testDir);

        auto bookRepo = std::make_shared<LMS::Storage::BookRepository>(testDir + "/books.csv");
        auto userRepo = std::make_shared<LMS::Storage::UserRepository>(testDir + "/users.csv");
        auto loanRepo = std::make_shared<LMS::Storage::LoanRepository>(testDir + "/loans.csv");
        auto resRepo  = std::make_shared<LMS::Storage::ReservationRepository>(testDir + "/reservations.csv");
        auto logger   = std::make_shared<LMS::Storage::AuditLogger>(testDir + "/audit.log");

        LMS::Models::Book testBook("BK-999", "Integration Test Book", "Author", "Testing", 2026, 1);
        bookRepo->save(testBook);

        LMS::Models::User testUser("USR-999", "testuser", "hash", "test@test.com", "000",
                                   LMS::Models::UserRole::Member, LMS::Models::UserStatus::Active,
                                   LMS::Models::MembershipType::Student);
        userRepo->save(testUser);

        LMS::Services::CirculationService circ(bookRepo, userRepo, loanRepo, resRepo, logger);

        auto reqRes = circ.requestLoan("USR-999", "BK-999");
        if (!reqRes.success || !reqRes.id.has_value()) return false;
        std::string loanId = reqRes.id.value();

        auto appRes = circ.approveLoan(loanId, "ADMIN");
        if (!appRes.success) return false;

        auto bookAfter = bookRepo->findById("BK-999");
        if (!bookAfter.has_value() || bookAfter->getAvailableCopies() != 0) return false;

        auto retRes = circ.returnBook(loanId, "USR-999");
        if (!retRes.success) return false;

        auto bookRestored = bookRepo->findById("BK-999");
        if (!bookRestored.has_value() || bookRestored->getAvailableCopies() != 1) return false;

        try { fs::remove_all(testDir); } catch (...) {}
        return true;
    });

    // ── 8. Book Renewal Limits ────────────────────────────────────────────
    runner.runTest("Loan renewal limits (max 2 renewals)", []() {
        std::string testDir = "build/test_data_renewal";
        fs::create_directories(testDir);

        auto bookRepo = std::make_shared<LMS::Storage::BookRepository>(testDir + "/books.csv");
        auto userRepo = std::make_shared<LMS::Storage::UserRepository>(testDir + "/users.csv");
        auto loanRepo = std::make_shared<LMS::Storage::LoanRepository>(testDir + "/loans.csv");
        auto resRepo  = std::make_shared<LMS::Storage::ReservationRepository>(testDir + "/reservations.csv");
        auto logger   = std::make_shared<LMS::Storage::AuditLogger>(testDir + "/audit.log");

        LMS::Models::Book book("BK-888", "Renewal Book", "Author", "CS", 2026, 2);
        bookRepo->save(book);

        LMS::Models::User user("USR-888", "renewer", "hash", "r@test.com", "111",
                               LMS::Models::UserRole::Member, LMS::Models::UserStatus::Active,
                               LMS::Models::MembershipType::Student);
        userRepo->save(user);

        LMS::Services::CirculationService circ(bookRepo, userRepo, loanRepo, resRepo, logger);

        auto req = circ.requestLoan("USR-888", "BK-888");
        circ.approveLoan(req.id.value(), "ADMIN");

        auto ren1 = circ.renewLoan(req.id.value(), "USR-888");
        if (!ren1.success) return false;

        auto ren2 = circ.renewLoan(req.id.value(), "USR-888");
        if (!ren2.success) return false;

        auto ren3 = circ.renewLoan(req.id.value(), "USR-888"); // Should fail
        if (ren3.success) return false;

        try { fs::remove_all(testDir); } catch (...) {}
        return true;
    });

    // ── 9. Book Reservation Queue ─────────────────────────────────────────
    runner.runTest("Book reservation waitlist queue on stock depletion", []() {
        std::string testDir = "build/test_data_res";
        fs::create_directories(testDir);

        auto bookRepo = std::make_shared<LMS::Storage::BookRepository>(testDir + "/books.csv");
        auto userRepo = std::make_shared<LMS::Storage::UserRepository>(testDir + "/users.csv");
        auto loanRepo = std::make_shared<LMS::Storage::LoanRepository>(testDir + "/loans.csv");
        auto resRepo  = std::make_shared<LMS::Storage::ReservationRepository>(testDir + "/reservations.csv");
        auto logger   = std::make_shared<LMS::Storage::AuditLogger>(testDir + "/audit.log");

        LMS::Models::Book book("BK-777", "Single Copy Book", "Author", "Fiction", 2026, 1);
        bookRepo->save(book);

        LMS::Models::User u1("USR-701", "user1", "hash", "u1@test.com", "111",
                             LMS::Models::UserRole::Member, LMS::Models::UserStatus::Active,
                             LMS::Models::MembershipType::Student);
        LMS::Models::User u2("USR-702", "user2", "hash", "u2@test.com", "222",
                             LMS::Models::UserRole::Member, LMS::Models::UserStatus::Active,
                             LMS::Models::MembershipType::Student);
        userRepo->save(u1);
        userRepo->save(u2);

        LMS::Services::CirculationService circ(bookRepo, userRepo, loanRepo, resRepo, logger);

        auto loanReq = circ.requestLoan("USR-701", "BK-777");
        circ.approveLoan(loanReq.id.value(), "ADMIN");

        auto resResult = circ.reserveBook("USR-702", "BK-777");
        if (!resResult.success) return false;

        auto ret = circ.returnBook(loanReq.id.value(), "USR-701");
        if (!ret.success) return false;

        auto userResList = circ.getUserReservations("USR-702");
        if (userResList.empty()) return false;

        try { fs::remove_all(testDir); } catch (...) {}
        return true;
    });

    // ── 10. 2FA Password Reset Verification ───────────────────────────────
    runner.runTest("2-Factor password reset verification (Email + Phone)", []() {
        std::string testDir = "build/test_data_auth";
        fs::create_directories(testDir);

        auto userRepo = std::make_shared<LMS::Storage::UserRepository>(testDir + "/users.csv");
        auto logger   = std::make_shared<LMS::Storage::AuditLogger>(testDir + "/audit.log");
        LMS::Services::AuthService auth(userRepo, logger);

        auth.signup("student_reset", "OldPass123", "student_reset@test.com", "03001234567", LMS::Models::MembershipType::Student);

        // Reset with wrong phone -> should fail
        bool fail = auth.resetPassword("student_reset@test.com", "03009999999", "NewPass123");
        if (fail) return false;

        // Reset with correct email + phone -> should succeed
        bool ok = auth.resetPassword("student_reset@test.com", "03001234567", "NewPass123");
        if (!ok) return false;

        // Login with new password
        auto loginRes = auth.login("student_reset", "NewPass123");
        bool loggedIn = loginRes.success;

        try { fs::remove_all(testDir); } catch (...) {}
        return loggedIn;
    });

    runner.printSummary();
    return runner.getExitCode();
}
