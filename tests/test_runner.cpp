/**
 * ============================================================
 *  Library Management System - Automated Unit Test Suite
 *  Author: Qamar Abbas
 *  C++20 Standard
 * ============================================================
 */

#include "utils/DateTime.h"
#include "utils/Crypto.h"
#include "utils/StringUtils.h"
#include "utils/Barcode.h"
#include "models/Book.h"
#include "models/User.h"
#include "models/Loan.h"
#include "models/Reservation.h"
#include "storage/CsvEngine.h"
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
#include "services/RecommendationService.h"
#include "services/ReceiptService.h"

#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <filesystem>

namespace fs = std::filesystem;

class TestRunner {
private:
    int m_totalTests{0};
    int m_passedTests{0};
    int m_failedTests{0};

public:
    void runTest(const std::string& testName, const std::function<bool()>& testFunc) {
        ++m_totalTests;
        std::cout << "  [TEST] " << testName << " ... ";
        try {
            if (testFunc()) {
                ++m_passedTests;
                std::cout << "\033[32m[PASSED]\033[0m\n";
            } else {
                ++m_failedTests;
                std::cout << "\033[31m[FAILED]\033[0m\n";
            }
        } catch (const std::exception& e) {
            ++m_failedTests;
            std::cout << "\033[31m[FAILED (Exception: " << e.what() << ")]\033[0m\n";
        } catch (...) {
            ++m_failedTests;
            std::cout << "\033[31m[FAILED (Unknown Exception)]\033[0m\n";
        }
    }

    void printSummary() const {
        std::cout << "\n==================================================\n";
        std::cout << "  TEST SUMMARY:\n";
        std::cout << "  Total Tests : " << m_totalTests << "\n";
        std::cout << "  Passed      : \033[32m" << m_passedTests << "\033[0m\n";
        std::cout << "  Failed      : " << (m_failedTests > 0 ? "\033[31m" : "\033[32m") << m_failedTests << "\033[0m\n";
        std::cout << "==================================================\n";
    }

    int getExitCode() const {
        return m_failedTests == 0 ? 0 : 1;
    }
};

int main() {
    std::cout << "\n==================================================\n";
    std::cout << "  RUNNING LMS AUTOMATED UNIT TESTS (C++20)\n";
    std::cout << "  Author: Qamar Abbas\n";
    std::cout << "==================================================\n\n";

    TestRunner runner;

    // ── 1. Date & DateTime Tests ──────────────────────────────────────────
    runner.runTest("Date::today and string conversion", []() {
        auto today = LMS::Utils::Date::today();
        std::string str = today.toString();
        auto parsed = LMS::Utils::Date::fromString(str);
        return today == parsed && today.isValid();
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
        std::string s = "  Clean Architecture  ";
        return LMS::Utils::StringUtils::trim(s) == "Clean Architecture" &&
               LMS::Utils::StringUtils::toLower("HeLLo") == "hello" &&
               LMS::Utils::StringUtils::toUpper("hello") == "HELLO";
    });

    runner.runTest("Levenshtein distance & fuzzy search", []() {
        size_t dist = LMS::Utils::StringUtils::levenshteinDistance("kitten", "sitting");
        bool match = LMS::Utils::StringUtils::fuzzyMatch("Atomic Habits", "Atmoic Habbits");
        return dist == 3 && match;
    });

    runner.runTest("ASCII Barcode & Card rendering", []() {
        std::string card = LMS::Utils::Barcode::renderLibraryCard("USR-001", "qamarabbas", "student", "2026-01-01");
        return card.find("LIBRARY MEMBERSHIP CARD") != std::string::npos &&
               card.find("USR-001") != std::string::npos;
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
        
        // Cannot borrow when 0 copies
        bool b3 = book.borrowCopy();
        if (b3) return false;

        // Return copy
        bool r1 = book.returnCopy();
        return r1 && book.getAvailableCopies() == 1 && book.isAvailable();
    });

    // ── 5. User Model & Policy Tests ──────────────────────────────────────
    runner.runTest("User role borrowing limits & policies", []() {
        LMS::Models::User student("USR-01", "student1", "hash", "s@test.com", "123",
                                  LMS::Models::UserRole::Member, LMS::Models::UserStatus::Active,
                                  LMS::Models::MembershipType::Student);
        LMS::Models::User faculty("USR-02", "prof1", "hash", "p@test.com", "123",
                                  LMS::Models::UserRole::Member, LMS::Models::UserStatus::Active,
                                  LMS::Models::MembershipType::Faculty);

        return student.getMaxBorrowLimit() == 4 &&
               student.getMaxLoanDays() == 14 &&
               faculty.getMaxBorrowLimit() == 8 &&
               faculty.getMaxLoanDays() == 30 &&
               student.canBorrow();
    });

    // ── 6. Loan State Machine & Overdue Fine Calculation ───────────────────
    runner.runTest("Loan due date and overdue fine calculation", []() {
        LMS::Utils::Date issueDate(2026, 1, 1);
        LMS::Utils::Date dueDate(2026, 1, 15);
        LMS::Models::Loan loan("LN-001", "USR-01", "BK-01", "Test Book", issueDate, issueDate, dueDate, LMS::Utils::Date(),
                               LMS::Models::LoanStatus::Approved, 0.0, false, 0);

        // On day 10 (not overdue)
        LMS::Utils::Date d10(2026, 1, 10);
        double f1 = loan.updateFine(d10, 0.50);
        if (f1 != 0.0 || loan.isOverdue(d10)) return false;

        // On day 20 (5 days overdue -> 5 * $0.50 = $2.50)
        LMS::Utils::Date d20(2026, 1, 20);
        double f2 = loan.updateFine(d20, 0.50);
        return f2 == 2.50 && loan.isOverdue(d20);
    });

    // ── 7. Full Integration Test (Mock in scratch directory) ──────────────
    runner.runTest("Full circulation flow (request -> approve -> return)", []() {
        std::string testDir = "build/test_data";
        fs::create_directories(testDir);

        auto bookRepo = std::make_shared<LMS::Storage::BookRepository>(testDir + "/books.csv");
        auto userRepo = std::make_shared<LMS::Storage::UserRepository>(testDir + "/users.csv");
        auto loanRepo = std::make_shared<LMS::Storage::LoanRepository>(testDir + "/loans.csv");
        auto resRepo  = std::make_shared<LMS::Storage::ReservationRepository>(testDir + "/reservations.csv");
        auto logger   = std::make_shared<LMS::Storage::AuditLogger>(testDir + "/audit.log");

        // Add a test book
        LMS::Models::Book testBook("BK-999", "Integration Test Book", "Author", "Testing", 2026, 1);
        bookRepo->save(testBook);

        // Add a test user
        LMS::Models::User testUser("USR-999", "testuser", "hash", "test@test.com", "000",
                                   LMS::Models::UserRole::Member, LMS::Models::UserStatus::Active,
                                   LMS::Models::MembershipType::Student);
        userRepo->save(testUser);

        LMS::Services::CirculationService circ(bookRepo, userRepo, loanRepo, resRepo, logger);

        // 1. Request loan
        auto reqRes = circ.requestLoan("USR-999", "BK-999");
        if (!reqRes.success || !reqRes.id.has_value()) return false;
        std::string loanId = reqRes.id.value();

        // 2. Approve loan
        auto appRes = circ.approveLoan(loanId, "ADMIN");
        if (!appRes.success) return false;

        // Verify book stock decreased
        auto bookAfter = bookRepo->findById("BK-999");
        if (!bookAfter.has_value() || bookAfter->getAvailableCopies() != 0) return false;

        // 3. Return book
        auto retRes = circ.returnBook(loanId, "USR-999");
        if (!retRes.success) return false;

        // Verify book stock restored
        auto bookRestored = bookRepo->findById("BK-999");
        if (!bookRestored.has_value() || bookRestored->getAvailableCopies() != 1) return false;

        // Clean up test files
        try { fs::remove_all(testDir); } catch (...) {}

        return true;
    });

    // ── 8. Book Renewal Policy Test ───────────────────────────────────────
    runner.runTest("Loan renewal limits and policy enforcement", []() {
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

        // Renewal 1: should succeed
        auto ren1 = circ.renewLoan(req.id.value(), "USR-888");
        if (!ren1.success) return false;

        // Renewal 2: should succeed
        auto ren2 = circ.renewLoan(req.id.value(), "USR-888");
        if (!ren2.success) return false;

        // Renewal 3: should fail (exceeds max 2 renewals)
        auto ren3 = circ.renewLoan(req.id.value(), "USR-888");
        if (ren3.success) return false;

        try { fs::remove_all(testDir); } catch (...) {}
        return true;
    });

    // ── 9. Book Reservation & Notification Queue Test ─────────────────────
    runner.runTest("Book reservation waitlist queue on stock depletion", []() {
        std::string testDir = "build/test_data_res";
        fs::create_directories(testDir);

        auto bookRepo = std::make_shared<LMS::Storage::BookRepository>(testDir + "/books.csv");
        auto userRepo = std::make_shared<LMS::Storage::UserRepository>(testDir + "/users.csv");
        auto loanRepo = std::make_shared<LMS::Storage::LoanRepository>(testDir + "/loans.csv");
        auto resRepo  = std::make_shared<LMS::Storage::ReservationRepository>(testDir + "/reservations.csv");
        auto logger   = std::make_shared<LMS::Storage::AuditLogger>(testDir + "/audit.log");

        // Single copy book
        LMS::Models::Book book("BK-777", "Popular Book", "Author", "Fiction", 2026, 1);
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

        // u1 borrows the only copy
        auto loanReq = circ.requestLoan("USR-701", "BK-777");
        circ.approveLoan(loanReq.id.value(), "ADMIN");

        // u2 places a reservation since stock is 0
        auto resResult = circ.reserveBook("USR-702", "BK-777");
        if (!resResult.success) return false;

        // u1 returns book -> system should automatically process waitlist
        auto ret = circ.returnBook(loanReq.id.value(), "USR-701");
        if (!ret.success) return false;

        auto userResList = circ.getUserReservations("USR-702");
        if (userResList.empty()) return false;

        try { fs::remove_all(testDir); } catch (...) {}
        return true;
    });

    // ── 10. Recommendation Service Scoring Test ───────────────────────────
    runner.runTest("RecommendationService affinity scoring", []() {
        std::string testDir = "build/test_data_rec";
        fs::create_directories(testDir);

        auto bookRepo = std::make_shared<LMS::Storage::BookRepository>(testDir + "/books.csv");
        auto loanRepo = std::make_shared<LMS::Storage::LoanRepository>(testDir + "/loans.csv");

        bookRepo->save(LMS::Models::Book("BK-1", "C++ Primer", "Lippman", "Programming", 2012, 5));
        bookRepo->save(LMS::Models::Book("BK-2", "Effective Modern C++", "Scott Meyers", "Programming", 2014, 5));
        bookRepo->save(LMS::Models::Book("BK-3", "The Great Gatsby", "Fitzgerald", "Classics", 1925, 5));

        LMS::Utils::Date now = LMS::Utils::Date::today();
        loanRepo->save(LMS::Models::Loan("LN-1", "USR-1", "BK-1", "C++ Primer", now, now, now.addDays(14), now,
                                         LMS::Models::LoanStatus::Returned, 0.0, true, 0));

        LMS::Services::RecommendationService recs(bookRepo, loanRepo);
        auto suggestions = recs.getRecommendationsForUser("USR-1", 5);

        // BK-2 should be top recommendation because of Programming category affinity
        bool ok = !suggestions.empty() && suggestions[0].getId() == "BK-2";

        try { fs::remove_all(testDir); } catch (...) {}
        return ok;
    });

    // ── 11. Receipt Generation & Export Test ───────────────────────────────
    runner.runTest("ReceiptService format and file export", []() {
        LMS::Utils::Date now = LMS::Utils::Date::today();
        LMS::Models::Loan loan("LN-TEST", "USR-1", "BK-1", "Clean Architecture", now, now, now.addDays(14), now,
                               LMS::Models::LoanStatus::Issued, 0.0, false, 0);
        LMS::Models::Book book("BK-1", "Clean Architecture", "Robert Martin", "CS", 2017, 3);
        LMS::Models::User user("USR-1", "qamarabbas", "hash", "q@test.com", "123",
                               LMS::Models::UserRole::Member, LMS::Models::UserStatus::Active,
                               LMS::Models::MembershipType::Student);

        std::string receipt = LMS::Services::ReceiptService::generateCheckoutReceipt(loan, book, user);
        if (receipt.find("LIBRARY BOOK CHECKOUT RECEIPT") == std::string::npos) return false;

        bool exported = LMS::Services::ReceiptService::exportReceiptToFile(receipt, "test_receipt.txt");
        if (!exported) return false;

        // Cleanup
        try { fs::remove("exports/test_receipt.txt"); } catch (...) {}
        return true;
    });

    runner.printSummary();
    return runner.getExitCode();
}
