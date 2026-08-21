#include "ui/LibraryApp.h"
#include "utils/Terminal.h"
#include "utils/TableFormatter.h"
#include "utils/StringUtils.h"
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace LMS::Utils;

namespace LMS::UI {

LibraryApp::LibraryApp(std::shared_ptr<Services::AuthService> auth,
                       std::shared_ptr<Services::BookService> books,
                       std::shared_ptr<Services::MemberService> members,
                       std::shared_ptr<Services::CirculationService> circ,
                       std::shared_ptr<Services::ReportService> reports)
    : m_auth(std::move(auth)),
      m_books(std::move(books)),
      m_members(std::move(members)),
      m_circ(std::move(circ)),
      m_reports(std::move(reports)) {}

// ═══════════════════════════════════════════════════════════════════════════
//  APPLICATION ENTRY POINT
// ═══════════════════════════════════════════════════════════════════════════

void LibraryApp::run() {
    Terminal::init();
    m_circ->updateAllFines(); // Refresh overdue fines on startup

    while (true) {
        Terminal::clear();
        showWelcomeScreen();

        int choice = Terminal::readInt("\n  Enter choice: ", 0, 3);
        switch (choice) {
            case 1: runLoginFlow();          break;
            case 2: runSignupFlow();         break;
            case 3: runForgotPasswordFlow(); break;
            case 0:
                Terminal::clear();
                std::cout << Color::Cyan << Color::Bold;
                std::cout << "\n  +===================================================+\n";
                std::cout <<   "  |        Thank you for using LMS v2.0               |\n";
                std::cout <<   "  |        Developed by Qamar Abbas                   |\n";
                std::cout <<   "  +===================================================+\n";
                std::cout << Color::Reset << "\n";
                return;
        }
    }
}

void LibraryApp::showWelcomeScreen() {
    std::cout << Color::Cyan << Color::Bold;
    std::cout << "\n";
    std::cout << "  ███████╗███████╗███╗   ███╗███████╗\n";
    std::cout << "  ██╔════╝██╔════╝████╗ ████║██╔════╝\n";
    std::cout << "  ███████╗█████╗  ██╔████╔██║███████╗\n";
    std::cout << "  ╚════██║██╔══╝  ██║╚██╔╝██║╚════██║\n";
    std::cout << "  ███████║███████╗██║ ╚═╝ ██║███████║\n";
    std::cout << "  ╚══════╝╚══════╝╚═╝     ╚═╝╚══════╝  " << Color::Gray << "v2.0\n" << Color::Reset;
    std::cout << Color::Cyan << Color::Bold;
    std::cout << "  Library Management System\n" << Color::Reset;
    std::cout << Color::Gray;
    std::cout << "  ─────────────────────────────────────────────────────────────────────────\n";
    std::cout << "  A modern C++20 library solution | Developed by Qamar Abbas\n" << Color::Reset;
    std::cout << "\n";
    std::cout << "  " << Color::White << Color::Bold << "[1]" << Color::Reset << "  Login\n";
    std::cout << "  " << Color::White << Color::Bold << "[2]" << Color::Reset << "  Register New Member\n";
    std::cout << "  " << Color::White << Color::Bold << "[3]" << Color::Reset << "  Forgot Password\n";
    std::cout << "  " << Color::White << Color::Bold << "[0]" << Color::Reset << "  Exit\n";
}

// ═══════════════════════════════════════════════════════════════════════════
//  AUTH FLOWS
// ═══════════════════════════════════════════════════════════════════════════

void LibraryApp::runLoginFlow() {
    Terminal::clear();
    Terminal::printHeader("LOGIN", "Please enter your credentials");
    std::cout << "\n";

    std::string username = Terminal::readString("  Username : ");
    std::string password = Terminal::readPassword("  Password : ");

    auto result = m_auth->login(username, password);
    if (!result.success) {
        Terminal::printError(result.message);
        Terminal::pause();
        return;
    }

    Terminal::printSuccess(result.message);
    Terminal::pause();

    if (m_auth->isAdmin()) {
        runAdminDashboard();
    } else {
        runMemberDashboard();
    }

    m_auth->logout();
}

void LibraryApp::runSignupFlow() {
    Terminal::clear();
    Terminal::printHeader("REGISTER NEW MEMBER", "Create your library account");
    std::cout << "\n";

    std::string username = Terminal::readString("  Username        : ");
    std::string email    = Terminal::readString("  Email address   : ");
    std::string phone    = Terminal::readString("  Phone number    : ", true);
    std::string password = Terminal::readPassword("  Password        : ");
    std::string confirm  = Terminal::readPassword("  Confirm Password: ");

    if (password != confirm) {
        Terminal::printError("Passwords do not match. Registration cancelled.");
        Terminal::pause();
        return;
    }

    std::cout << "\n  Membership type:\n";
    std::cout << "  " << Color::Bold << "[1]" << Color::Reset << " Student   (4 books, 14 days)\n";
    std::cout << "  " << Color::Bold << "[2]" << Color::Reset << " Faculty   (8 books, 30 days)\n";
    std::cout << "  " << Color::Bold << "[3]" << Color::Reset << " General   (2 books,  7 days)\n";
    int mType = Terminal::readInt("  Choice [1-3]: ", 1, 3, 1);

    Models::MembershipType memType = Models::MembershipType::Student;
    if (mType == 2) memType = Models::MembershipType::Faculty;
    else if (mType == 3) memType = Models::MembershipType::General;

    auto result = m_auth->signup(username, password, email, phone, memType);
    if (result.success) {
        Terminal::printSuccess(result.message);
    } else {
        Terminal::printError(result.message);
    }
    Terminal::pause();
}

void LibraryApp::runForgotPasswordFlow() {
    Terminal::clear();
    Terminal::printHeader("FORGOT PASSWORD", "2-Factor Identity Verification & Password Reset");
    std::cout << "\n";

    std::string email    = Terminal::readString("  Registered email       : ");
    std::string phone    = Terminal::readString("  Registered phone number: ");
    std::string newPass  = Terminal::readPassword("  New password           : ");
    std::string confirm  = Terminal::readPassword("  Confirm password       : ");

    if (newPass != confirm) {
        Terminal::printError("Passwords do not match.");
        Terminal::pause();
        return;
    }
    if (newPass.length() < 4) {
        Terminal::printError("Password must be at least 4 characters.");
        Terminal::pause();
        return;
    }

    if (m_auth->resetPassword(email, phone, newPass)) {
        Terminal::printSuccess("Identity verified! Password reset successfully. You may now log in.");
    } else {
        Terminal::printError("Verification failed. Email or phone number does not match registered records.");
    }
    Terminal::pause();
}

// ═══════════════════════════════════════════════════════════════════════════
//  ADMIN DASHBOARD
// ═══════════════════════════════════════════════════════════════════════════

void LibraryApp::runAdminDashboard() {
    const auto& user = m_auth->getCurrentUser();
    while (true) {
        Terminal::clear();
        Terminal::printHeader("ADMIN DASHBOARD", "Welcome, " + (user ? user->getUsername() : "Admin"));

        auto metrics = m_reports->getDashboardMetrics();
        std::cout << "\n";
        std::cout << Color::Cyan << "  ┌─ Library Snapshot ──────────────────────────────────────────────┐\n" << Color::Reset;
        std::cout << "  │  " << Color::Green  << "Books  : " << metrics.totalBooks   << " titles  │  " << metrics.availableCopies << " available\n" << Color::Reset;
        std::cout << "  │  " << Color::Yellow << "Loans  : " << metrics.activeLoans  << " active  │  " << metrics.pendingRequests << " pending approval\n" << Color::Reset;
        std::cout << "  │  " << Color::Red    << "Overdue: " << metrics.overdueLoans << "         │  Fines unpaid: $" << std::fixed << std::setprecision(2) << metrics.pendingFines << "\n" << Color::Reset;
        std::cout << "  │  " << Color::Blue   << "Members: " << metrics.totalMembers << " total   │  " << metrics.activeMembers << " active\n" << Color::Reset;
        std::cout << Color::Cyan << "  └────────────────────────────────────────────────────────────────┘\n" << Color::Reset;

        std::cout << "\n";
        std::cout << "  " << Color::Bold << "[1]" << Color::Reset << "  Book Management\n";
        std::cout << "  " << Color::Bold << "[2]" << Color::Reset << "  Member Management\n";
        std::cout << "  " << Color::Bold << "[3]" << Color::Reset << "  Circulation & Loans\n";
        std::cout << "  " << Color::Bold << "[4]" << Color::Reset << "  Reports & Analytics\n";
        std::cout << "  " << Color::Bold << "[0]" << Color::Reset << "  Logout\n";

        int ch = Terminal::readInt("\n  Choice: ", 0, 4);
        switch (ch) {
            case 1: adminBookManagement();       break;
            case 2: adminMemberManagement();     break;
            case 3: adminCirculationManagement(); break;
            case 4: adminReports();              break;
            case 0: return;
        }
    }
}

void LibraryApp::adminBookManagement() {
    while (true) {
        Terminal::clear();
        Terminal::printHeader("BOOK MANAGEMENT");
        std::cout << "\n";
        std::cout << "  " << Color::Bold << "[1]" << Color::Reset << "  List All Books\n";
        std::cout << "  " << Color::Bold << "[2]" << Color::Reset << "  Search Books\n";
        std::cout << "  " << Color::Bold << "[3]" << Color::Reset << "  Add New Book\n";
        std::cout << "  " << Color::Bold << "[4]" << Color::Reset << "  Edit Book\n";
        std::cout << "  " << Color::Bold << "[5]" << Color::Reset << "  Delete Book\n";
        std::cout << "  " << Color::Bold << "[6]" << Color::Reset << "  Restock Book Copies\n";
        std::cout << "  " << Color::Bold << "[0]" << Color::Reset << "  Back\n";

        int ch = Terminal::readInt("\n  Choice: ", 0, 6);
        switch (ch) {
            case 1: adminListBooks();     break;
            case 2: memberSearchBooks();  break;
            case 3: adminAddBook();       break;
            case 4: adminEditBook();      break;
            case 5: adminDeleteBook();    break;
            case 6: adminRestockBook();   break;
            case 0: return;
        }
    }
}

void LibraryApp::adminMemberManagement() {
    while (true) {
        Terminal::clear();
        Terminal::printHeader("MEMBER MANAGEMENT");
        std::cout << "\n";
        std::cout << "  " << Color::Bold << "[1]" << Color::Reset << "  List All Members\n";
        std::cout << "  " << Color::Bold << "[2]" << Color::Reset << "  Add Member\n";
        std::cout << "  " << Color::Bold << "[3]" << Color::Reset << "  Edit Member\n";
        std::cout << "  " << Color::Bold << "[4]" << Color::Reset << "  Delete Member\n";
        std::cout << "  " << Color::Bold << "[5]" << Color::Reset << "  Ban / Unban / Suspend Member\n";
        std::cout << "  " << Color::Bold << "[0]" << Color::Reset << "  Back\n";

        int ch = Terminal::readInt("\n  Choice: ", 0, 5);
        switch (ch) {
            case 1: adminListMembers();     break;
            case 2: adminAddMember();       break;
            case 3: adminEditMember();      break;
            case 4: adminDeleteMember();    break;
            case 5: adminBanUnbanMember();  break;
            case 0: return;
        }
    }
}

void LibraryApp::adminCirculationManagement() {
    while (true) {
        Terminal::clear();
        Terminal::printHeader("CIRCULATION MANAGEMENT");
        std::cout << "\n";
        std::cout << "  " << Color::Bold << "[1]" << Color::Reset << "  Approve / Reject Pending Loans\n";
        std::cout << "  " << Color::Bold << "[2]" << Color::Reset << "  View All Active Loans\n";
        std::cout << "  " << Color::Bold << "[3]" << Color::Reset << "  View Overdue Loans\n";
        std::cout << "  " << Color::Bold << "[4]" << Color::Reset << "  Process Fine Payment\n";
        std::cout << "  " << Color::Bold << "[0]" << Color::Reset << "  Back\n";

        int ch = Terminal::readInt("\n  Choice: ", 0, 4);
        switch (ch) {
            case 1: adminPendingLoans(); break;
            case 2: adminAllLoans();     break;
            case 3: adminOverdueLoans(); break;
            case 4: adminPayFine();      break;
            case 0: return;
        }
    }
}

void LibraryApp::adminReports() {
    while (true) {
        Terminal::clear();
        Terminal::printHeader("REPORTS & ANALYTICS");
        std::cout << "\n";
        std::cout << "  " << Color::Bold << "[1]" << Color::Reset << "  Library Dashboard Metrics\n";
        std::cout << "  " << Color::Bold << "[2]" << Color::Reset << "  Most Popular Books\n";
        std::cout << "  " << Color::Bold << "[3]" << Color::Reset << "  Recent Audit Log\n";
        std::cout << "  " << Color::Bold << "[0]" << Color::Reset << "  Back\n";

        int ch = Terminal::readInt("\n  Choice: ", 0, 3);
        switch (ch) {
            case 1: adminDashboardStats(); break;
            case 2: adminPopularBooks();   break;
            case 3: adminAuditLog();       break;
            case 0: return;
        }
    }
}

// ═══════════════════════════════════════════════════════════════════════════
//  ADMIN - BOOK ACTIONS
// ═══════════════════════════════════════════════════════════════════════════

void LibraryApp::adminListBooks() {
    Terminal::clear();
    Terminal::printHeader("ALL BOOKS IN CATALOG");
    auto books = m_books->getAllBooks("title");
    printBookTable(books);
    Terminal::pause();
}

void LibraryApp::adminAddBook() {
    Terminal::clear();
    Terminal::printHeader("ADD NEW BOOK");
    std::cout << "\n";

    std::string title    = Terminal::readString("  Title           : ");
    std::string author   = Terminal::readString("  Author          : ");
    std::string category = Terminal::readString("  Category        : ");
    std::string isbn     = Terminal::readString("  ISBN (optional) : ", true);
    std::string shelf    = Terminal::readString("  Shelf Location  : ", true, "General Stack");
    int year             = Terminal::readInt   ("  Publication Year: ", 1000, 2030);
    int copies           = Terminal::readInt   ("  Number of Copies: ", 1, 100);

    std::string adminId = m_auth->getCurrentUser() ? m_auth->getCurrentUser()->getId() : "ADMIN";
    auto result = m_books->addBook(title, author, category, year, copies, isbn, shelf, adminId);

    if (result.has_value()) {
        Terminal::printSuccess("Book added: " + result->getTitle() + " [" + result->getId() + "]");
    } else {
        Terminal::printError("Failed to add book. Please check your inputs.");
    }
    Terminal::pause();
}

void LibraryApp::adminEditBook() {
    Terminal::clear();
    Terminal::printHeader("EDIT BOOK");
    std::cout << "\n";

    std::string bookId = Terminal::readString("  Enter Book ID (e.g. BK-001): ");
    auto bookOpt = m_books->getBookById(bookId);
    if (!bookOpt.has_value()) {
        Terminal::printError("Book ID not found: " + bookId);
        Terminal::pause();
        return;
    }

    const auto& b = bookOpt.value();
    std::cout << "\n" << Color::Dim << "  Press Enter to keep current value.\n" << Color::Reset << "\n";

    std::string title    = Terminal::readString("  Title    [" + b.getTitle()   + "]: ", true, b.getTitle());
    std::string author   = Terminal::readString("  Author   [" + b.getAuthor()  + "]: ", true, b.getAuthor());
    std::string category = Terminal::readString("  Category [" + b.getCategory()+ "]: ", true, b.getCategory());
    std::string shelf    = Terminal::readString("  Shelf    [" + b.getShelfLocation()+ "]: ", true, b.getShelfLocation());

    int year   = Terminal::readInt("  Year     [" + std::to_string(b.getPublicationYear()) + "]: ", 1000, 2030, b.getPublicationYear());
    int copies = Terminal::readInt("  Copies   [" + std::to_string(b.getTotalCopies())     + "]: ", 1, 999, b.getTotalCopies());

    std::string adminId = m_auth->getCurrentUser() ? m_auth->getCurrentUser()->getId() : "ADMIN";
    if (m_books->updateBook(bookId, title, author, category, year, shelf, copies, adminId)) {
        Terminal::printSuccess("Book updated successfully.");
    } else {
        Terminal::printError("Update failed.");
    }
    Terminal::pause();
}

void LibraryApp::adminDeleteBook() {
    Terminal::clear();
    Terminal::printHeader("DELETE BOOK");
    std::cout << "\n";

    std::string bookId = Terminal::readString("  Enter Book ID to delete: ");
    auto bookOpt = m_books->getBookById(bookId);
    if (!bookOpt.has_value()) {
        Terminal::printError("Book ID not found.");
        Terminal::pause();
        return;
    }

    std::cout << "  Book: " << Color::Yellow << bookOpt->getTitle() << Color::Reset << "\n";
    if (!Terminal::readConfirmation("  Are you sure you want to permanently delete this book?", false)) {
        Terminal::printInfo("Deletion cancelled.");
        Terminal::pause();
        return;
    }

    std::string adminId = m_auth->getCurrentUser() ? m_auth->getCurrentUser()->getId() : "ADMIN";
    if (m_books->deleteBook(bookId, adminId)) {
        Terminal::printSuccess("Book deleted successfully.");
    } else {
        Terminal::printError("Cannot delete: book may have active loans, or ID not found.");
    }
    Terminal::pause();
}

void LibraryApp::adminRestockBook() {
    Terminal::clear();
    Terminal::printHeader("RESTOCK BOOK");
    std::cout << "\n";

    std::string bookId = Terminal::readString("  Enter Book ID to restock: ");
    auto bookOpt = m_books->getBookById(bookId);
    if (!bookOpt.has_value()) {
        Terminal::printError("Book not found.");
        Terminal::pause();
        return;
    }

    std::cout << "  Book: " << Color::Yellow << bookOpt->getTitle() << Color::Reset
              << " | Current stock: " << bookOpt->getAvailableCopies() << "/" << bookOpt->getTotalCopies() << "\n";
    int addCount = Terminal::readInt("  How many copies to add? ", 1, 100);

    std::string adminId = m_auth->getCurrentUser() ? m_auth->getCurrentUser()->getId() : "ADMIN";
    if (m_books->restockBook(bookId, addCount, adminId)) {
        Terminal::printSuccess("Restocked " + std::to_string(addCount) + " copies successfully.");
    } else {
        Terminal::printError("Restock failed.");
    }
    Terminal::pause();
}

// ═══════════════════════════════════════════════════════════════════════════
//  ADMIN - MEMBER ACTIONS
// ═══════════════════════════════════════════════════════════════════════════

void LibraryApp::adminListMembers() {
    Terminal::clear();
    Terminal::printHeader("ALL REGISTERED MEMBERS");
    auto members = m_members->getAllMembers();
    printMemberTable(members);
    Terminal::pause();
}

void LibraryApp::adminAddMember() {
    Terminal::clear();
    Terminal::printHeader("ADD NEW MEMBER");
    std::cout << "\n";

    std::string username = Terminal::readString("  Username  : ");
    std::string email    = Terminal::readString("  Email     : ");
    std::string phone    = Terminal::readString("  Phone     : ", true);
    std::string password = Terminal::readPassword("  Password  : ");

    std::cout << "\n  Membership Type:\n";
    std::cout << "  [1] Student  [2] Faculty  [3] General\n";
    int mt = Terminal::readInt("  Type [1-3]: ", 1, 3, 1);
    Models::MembershipType memType = (mt == 2) ? Models::MembershipType::Faculty :
                                     (mt == 3) ? Models::MembershipType::General :
                                                 Models::MembershipType::Student;

    std::string adminId = m_auth->getCurrentUser() ? m_auth->getCurrentUser()->getId() : "ADMIN";
    auto result = m_members->addMember(username, password, email, phone, memType, adminId);

    if (result.has_value()) {
        Terminal::printSuccess("Member added: " + result->getUsername() + " [" + result->getId() + "]");
    } else {
        Terminal::printError("Failed to add member. Username or email may already exist.");
    }
    Terminal::pause();
}

void LibraryApp::adminEditMember() {
    Terminal::clear();
    Terminal::printHeader("EDIT MEMBER PROFILE");
    std::cout << "\n";

    std::string uid = Terminal::readString("  Enter Member ID (e.g. USR-001): ");
    auto memOpt = m_members->getMemberById(uid);
    if (!memOpt.has_value()) {
        Terminal::printError("Member not found: " + uid);
        Terminal::pause();
        return;
    }

    const auto& u = memOpt.value();
    std::cout << "\n" << Color::Dim << "  Press Enter to keep current value.\n" << Color::Reset << "\n";
    std::string email = Terminal::readString("  Email [" + u.getEmail() + "]: ", true, u.getEmail());
    std::string phone = Terminal::readString("  Phone [" + u.getPhone() + "]: ", true, u.getPhone());
    std::string pass  = Terminal::readPassword("  New Password (leave blank to keep): ");

    if (m_members->updateProfile(uid, email, phone, pass)) {
        Terminal::printSuccess("Member updated.");
    } else {
        Terminal::printError("Update failed.");
    }
    Terminal::pause();
}

void LibraryApp::adminDeleteMember() {
    Terminal::clear();
    Terminal::printHeader("DELETE MEMBER");
    std::cout << "\n";

    std::string uid = Terminal::readString("  Enter Member ID to delete: ");
    auto memOpt = m_members->getMemberById(uid);
    if (!memOpt.has_value()) {
        Terminal::printError("Member not found.");
        Terminal::pause();
        return;
    }

    std::cout << "  Member: " << Color::Yellow << memOpt->getUsername() << Color::Reset << "\n";
    if (!Terminal::readConfirmation("  Delete this member permanently?", false)) {
        Terminal::printInfo("Cancelled.");
        Terminal::pause();
        return;
    }

    std::string adminId = m_auth->getCurrentUser() ? m_auth->getCurrentUser()->getId() : "ADMIN";
    if (m_members->deleteMember(uid, adminId)) {
        Terminal::printSuccess("Member deleted.");
    } else {
        Terminal::printError("Cannot delete: member may have active loans.");
    }
    Terminal::pause();
}

void LibraryApp::adminBanUnbanMember() {
    Terminal::clear();
    Terminal::printHeader("BAN / UNBAN / SUSPEND MEMBER");
    std::cout << "\n";

    std::string uid = Terminal::readString("  Enter Member ID: ");
    auto memOpt = m_members->getMemberById(uid);
    if (!memOpt.has_value()) {
        Terminal::printError("Member not found.");
        Terminal::pause();
        return;
    }

    std::cout << "  Member: " << Color::Yellow << memOpt->getUsername() << Color::Reset
              << " | Status: " << memOpt->getStatusString() << "\n\n";
    std::cout << "  [1] Ban Member      [2] Suspend Member\n";
    std::cout << "  [3] Activate Member [0] Cancel\n";

    int ch = Terminal::readInt("  Action: ", 0, 3);
    std::string adminId = m_auth->getCurrentUser() ? m_auth->getCurrentUser()->getId() : "ADMIN";
    bool ok = false;
    switch (ch) {
        case 1: ok = m_members->banMember(uid, adminId);     break;
        case 2: ok = m_members->suspendMember(uid, adminId); break;
        case 3: ok = m_members->activateMember(uid, adminId);break;
        case 0: return;
    }
    if (ok) Terminal::printSuccess("Member status updated.");
    else    Terminal::printError("Failed to update status.");
    Terminal::pause();
}

// ═══════════════════════════════════════════════════════════════════════════
//  ADMIN - CIRCULATION ACTIONS
// ═══════════════════════════════════════════════════════════════════════════

void LibraryApp::adminPendingLoans() {
    Terminal::clear();
    Terminal::printHeader("PENDING LOAN REQUESTS");

    auto pending = m_circ->getPendingLoans();
    if (pending.empty()) {
        Terminal::printInfo("No pending loan requests.");
        Terminal::pause();
        return;
    }

    printLoanTable(pending, true);

    std::string loanId = Terminal::readString("\n  Enter Loan ID to process (or 0 to go back): ", true);
    if (loanId == "0" || loanId.empty()) return;

    std::cout << "\n  [1] Approve   [2] Reject   [0] Cancel\n";
    int ch = Terminal::readInt("  Action: ", 0, 2);

    std::string adminId = m_auth->getCurrentUser() ? m_auth->getCurrentUser()->getId() : "ADMIN";
    Services::CirculationResult result;
    if (ch == 1) result = m_circ->approveLoan(loanId, adminId);
    else if (ch == 2) result = m_circ->rejectLoan(loanId, adminId);
    else return;

    if (result.success) Terminal::printSuccess(result.message);
    else                Terminal::printError(result.message);
    Terminal::pause();
}

void LibraryApp::adminAllLoans() {
    Terminal::clear();
    Terminal::printHeader("ALL LOANS");
    auto loans = m_circ->getAllLoans();
    printLoanTable(loans, true);
    Terminal::pause();
}

void LibraryApp::adminOverdueLoans() {
    Terminal::clear();
    Terminal::printHeader("OVERDUE LOANS");
    m_circ->updateAllFines();
    auto loans = m_circ->getOverdueLoans();
    if (loans.empty()) {
        Terminal::printSuccess("No overdue loans. All members are up to date!");
    } else {
        Terminal::printWarning("Found " + std::to_string(loans.size()) + " overdue loans.");
        printLoanTable(loans, true);
    }
    Terminal::pause();
}

void LibraryApp::adminPayFine() {
    Terminal::clear();
    Terminal::printHeader("PROCESS FINE PAYMENT");
    std::cout << "\n";

    std::string loanId = Terminal::readString("  Loan ID: ");
    double amount = Terminal::readDouble("  Amount paid ($): ", 0.01, 10000.0);

    std::string adminId = m_auth->getCurrentUser() ? m_auth->getCurrentUser()->getId() : "ADMIN";
    auto result = m_circ->payFine(loanId, amount, adminId);

    if (result.success) Terminal::printSuccess(result.message);
    else                Terminal::printError(result.message);
    Terminal::pause();
}

// ═══════════════════════════════════════════════════════════════════════════
//  ADMIN - REPORTS
// ═══════════════════════════════════════════════════════════════════════════

void LibraryApp::adminDashboardStats() {
    Terminal::clear();
    Terminal::printHeader("LIBRARY DASHBOARD METRICS");
    auto m = m_reports->getDashboardMetrics();

    std::cout << "\n";
    Terminal::printDivider(70, '=');
    std::cout << Color::Cyan << Color::Bold << "  CATALOG\n" << Color::Reset;
    std::cout << "    Unique Titles   : " << m.totalBooks << "\n";
    std::cout << "    Total Copies    : " << m.totalBookCopies << "\n";
    std::cout << "    Available       : " << Color::Green << m.availableCopies << Color::Reset << "\n";
    std::cout << "    Checked Out     : " << Color::Yellow << m.borrowedCopies << Color::Reset << "\n\n";

    Terminal::printDivider(70, '-');
    std::cout << Color::Cyan << Color::Bold << "  CIRCULATION\n" << Color::Reset;
    std::cout << "    Total Loans     : " << m.totalLoans << "\n";
    std::cout << "    Active Loans    : " << m.activeLoans << "\n";
    std::cout << "    Pending Requests: " << Color::Yellow << m.pendingRequests << Color::Reset << "\n";
    std::cout << "    Overdue Loans   : " << Color::Red << m.overdueLoans << Color::Reset << "\n\n";

    Terminal::printDivider(70, '-');
    std::cout << Color::Cyan << Color::Bold << "  FINES\n" << Color::Reset;
    std::cout << "    Total Incurred  : $" << std::fixed << std::setprecision(2) << m.totalFinesIncurred << "\n";
    std::cout << "    Collected       : $" << Color::Green << std::fixed << std::setprecision(2) << m.totalFinesCollected << Color::Reset << "\n";
    std::cout << "    Outstanding     : $" << Color::Red << std::fixed << std::setprecision(2) << m.pendingFines << Color::Reset << "\n\n";

    Terminal::printDivider(70, '-');
    std::cout << Color::Cyan << Color::Bold << "  MEMBERS\n" << Color::Reset;
    std::cout << "    Registered      : " << m.totalMembers << "\n";
    std::cout << "    Active          : " << Color::Green << m.activeMembers << Color::Reset << "\n";
    Terminal::printDivider(70, '=');

    Terminal::pause();
}

void LibraryApp::adminPopularBooks() {
    Terminal::clear();
    Terminal::printHeader("MOST POPULAR BOOKS");
    auto books = m_reports->getMostPopularBooks(10);
    printBookTable(books);
    Terminal::pause();
}

void LibraryApp::adminAuditLog() {
    Terminal::clear();
    Terminal::printHeader("RECENT AUDIT LOG", "Last 30 system events");
    std::cout << "\n";
    auto logs = m_reports->getRecentAuditLogs(30);
    if (logs.empty()) {
        Terminal::printInfo("No audit log entries yet.");
    } else {
        for (const auto& entry : logs) {
            std::cout << Color::Gray << "  " << entry << Color::Reset << "\n";
        }
    }
    Terminal::pause();
}

// ═══════════════════════════════════════════════════════════════════════════
//  MEMBER DASHBOARD
// ═══════════════════════════════════════════════════════════════════════════

void LibraryApp::runMemberDashboard() {
    while (true) {
        m_auth->refreshCurrentUser();
        const auto& user = m_auth->getCurrentUser();
        if (!user.has_value()) return;

        Terminal::clear();
        Terminal::printHeader("MEMBER PORTAL", "Welcome, " + user->getUsername());

        auto activeLoans = m_circ->getUserActiveLoans(user->getId());
        std::cout << "\n";
        std::cout << Color::Cyan << "  ┌─ Your Account ─────────────────────────────────────────────────┐\n" << Color::Reset;
        std::cout << "  │  Membership : " << Color::Yellow << user->getMembershipTypeString() << Color::Reset << "\n";
        std::cout << "  │  Active Loans: " << activeLoans.size() << " / " << user->getMaxBorrowLimit() << "  (Loan period: " << user->getMaxLoanDays() << " days)\n";
        std::cout << Color::Cyan << "  └────────────────────────────────────────────────────────────────┘\n" << Color::Reset;

        std::cout << "\n";
        std::cout << "  " << Color::Bold << "[1]" << Color::Reset << "  Browse & Search Books\n";
        std::cout << "  " << Color::Bold << "[2]" << Color::Reset << "  Request a Book Loan\n";
        std::cout << "  " << Color::Bold << "[3]" << Color::Reset << "  My Active Loans\n";
        std::cout << "  " << Color::Bold << "[4]" << Color::Reset << "  Return a Book\n";
        std::cout << "  " << Color::Bold << "[5]" << Color::Reset << "  Renew a Loan\n";
        std::cout << "  " << Color::Bold << "[6]" << Color::Reset << "  Reserve a Book (Hold Queue)\n";
        std::cout << "  " << Color::Bold << "[7]" << Color::Reset << "  My Reservations\n";
        std::cout << "  " << Color::Bold << "[8]" << Color::Reset << "  Update Profile\n";
        std::cout << "  " << Color::Bold << "[9]" << Color::Reset << "  Change Password\n";
        std::cout << "  " << Color::Bold << "[0]" << Color::Reset << "  Logout\n";

        int ch = Terminal::readInt("\n  Choice: ", 0, 9);
        switch (ch) {
            case 1:  memberSearchBooks();       break;
            case 2:  memberRequestLoan();       break;
            case 3:  memberMyLoans();           break;
            case 4:  memberReturnBook();        break;
            case 5:  memberRenewLoan();         break;
            case 6:  memberReserveBook();       break;
            case 7:  memberMyReservations();    break;
            case 8:  memberUpdateProfile();     break;
            case 9:  memberChangePassword();    break;
            case 0: return;
        }
    }
}

void LibraryApp::memberSearchBooks() {
    Terminal::clear();
    Terminal::printHeader("BOOK CATALOG", "Search or browse all books");
    std::cout << "\n  [1] View all books  [2] Search by keyword  [3] Browse by category\n";
    int ch = Terminal::readInt("  Choice: ", 1, 3, 1);

    std::vector<Models::Book> books;
    if (ch == 1) {
        books = m_books->getAllBooks("title");
    } else if (ch == 2) {
        std::string query = Terminal::readString("  Search (title/author/ISBN): ");
        books = m_books->searchBooks(query);
        if (books.empty()) {
            Terminal::printWarning("No books found matching '" + query + "'");
            Terminal::pause();
            return;
        }
    } else {
        auto cats = m_books->getCategories();
        std::cout << "\n  Available categories:\n";
        for (size_t i = 0; i < cats.size(); ++i) {
            std::cout << "  " << Color::Bold << "[" << (i + 1) << "]" << Color::Reset << " " << cats[i] << "\n";
        }
        int catChoice = Terminal::readInt("  Category [1-" + std::to_string(cats.size()) + "]: ", 1, (int)cats.size());
        books = m_books->getBooksByCategory(cats[catChoice - 1]);
    }

    printBookTable(books);
    Terminal::pause();
}

void LibraryApp::memberRequestLoan() {
    Terminal::clear();
    Terminal::printHeader("REQUEST BOOK LOAN");
    std::cout << "\n";

    std::string bookId = Terminal::readString("  Enter Book ID (e.g. BK-001): ");
    std::string userId = m_auth->getCurrentUser()->getId();
    auto result = m_circ->requestLoan(userId, bookId);

    if (result.success) Terminal::printSuccess(result.message);
    else                Terminal::printError(result.message);
    Terminal::pause();
}

void LibraryApp::memberMyLoans() {
    Terminal::clear();
    Terminal::printHeader("MY ACTIVE LOANS");
    std::string userId = m_auth->getCurrentUser()->getId();
    m_circ->updateAllFines();
    auto loans = m_circ->getUserActiveLoans(userId);
    printLoanTable(loans);
    Terminal::pause();
}

void LibraryApp::memberReturnBook() {
    Terminal::clear();
    Terminal::printHeader("RETURN A BOOK");
    std::cout << "\n";

    std::string userId = m_auth->getCurrentUser()->getId();
    auto activeLoans = m_circ->getUserActiveLoans(userId);
    if (activeLoans.empty()) {
        Terminal::printInfo("You have no active loans to return.");
        Terminal::pause();
        return;
    }

    printLoanTable(activeLoans);
    std::string loanId = Terminal::readString("\n  Enter Loan ID to return: ");
    auto result = m_circ->returnBook(loanId, userId);

    if (result.success) Terminal::printSuccess(result.message);
    else                Terminal::printError(result.message);
    Terminal::pause();
}

void LibraryApp::memberRenewLoan() {
    Terminal::clear();
    Terminal::printHeader("RENEW A LOAN");
    std::cout << "\n";

    std::string userId = m_auth->getCurrentUser()->getId();
    auto loans = m_circ->getUserActiveLoans(userId);
    if (loans.empty()) {
        Terminal::printInfo("No active loans to renew.");
        Terminal::pause();
        return;
    }

    printLoanTable(loans);
    std::string loanId = Terminal::readString("\n  Enter Loan ID to renew: ");
    auto result = m_circ->renewLoan(loanId, userId);

    if (result.success) Terminal::printSuccess(result.message);
    else                Terminal::printError(result.message);
    Terminal::pause();
}

void LibraryApp::memberReserveBook() {
    Terminal::clear();
    Terminal::printHeader("RESERVE A BOOK");
    std::cout << "\n";
    Terminal::printInfo("Reserve a book that is currently not available.");
    std::cout << "\n";

    std::string bookId = Terminal::readString("  Enter Book ID: ");
    std::string userId = m_auth->getCurrentUser()->getId();
    auto result = m_circ->reserveBook(userId, bookId);

    if (result.success) Terminal::printSuccess(result.message);
    else                Terminal::printError(result.message);
    Terminal::pause();
}

void LibraryApp::memberMyReservations() {
    Terminal::clear();
    Terminal::printHeader("MY RESERVATIONS");
    std::string userId = m_auth->getCurrentUser()->getId();
    auto reservations = m_circ->getUserReservations(userId);

    if (reservations.empty()) {
        Terminal::printInfo("You have no active reservations.");
        Terminal::pause();
        return;
    }

    TableFormatter table({
        {"Res ID",   7, 12, Alignment::Left},
        {"Book ID",  7, 12, Alignment::Left},
        {"Date",    10, 12, Alignment::Center},
        {"Status",   8, 12, Alignment::Center}
    });

    for (const auto& r : reservations) {
        table.addRow({r.getId(), r.getBookId(), r.getReserveDate().toString(), r.getStatusString()});
    }
    std::cout << "\n";
    table.render();

    std::cout << "\n  Enter Reservation ID to cancel (or 0 to go back): ";
    std::string resId = Terminal::readString("  ", true);
    if (resId == "0" || resId.empty()) return;

    auto result = m_circ->cancelReservation(resId, userId);
    if (result.success) Terminal::printSuccess(result.message);
    else                Terminal::printError(result.message);
    Terminal::pause();
}

void LibraryApp::memberUpdateProfile() {
    Terminal::clear();
    Terminal::printHeader("UPDATE PROFILE");
    const auto& user = m_auth->getCurrentUser();
    std::cout << "\n" << Color::Dim << "  Press Enter to keep current value.\n" << Color::Reset << "\n";

    std::string email = Terminal::readString("  Email [" + user->getEmail() + "]: ", true, user->getEmail());
    std::string phone = Terminal::readString("  Phone [" + user->getPhone() + "]: ", true, user->getPhone());

    if (m_members->updateProfile(user->getId(), email, phone)) {
        Terminal::printSuccess("Profile updated successfully.");
        m_auth->refreshCurrentUser();
    } else {
        Terminal::printError("Update failed. Email may already be in use.");
    }
    Terminal::pause();
}

void LibraryApp::memberChangePassword() {
    Terminal::clear();
    Terminal::printHeader("CHANGE PASSWORD");
    std::cout << "\n";

    std::string oldPass = Terminal::readPassword("  Current Password: ");
    std::string newPass = Terminal::readPassword("  New Password    : ");
    std::string confirm = Terminal::readPassword("  Confirm New Pass: ");

    if (newPass != confirm) {
        Terminal::printError("Passwords do not match.");
        Terminal::pause();
        return;
    }
    if (newPass.length() < 4) {
        Terminal::printError("New password must be at least 4 characters.");
        Terminal::pause();
        return;
    }

    std::string userId = m_auth->getCurrentUser()->getId();
    if (m_auth->changePassword(userId, oldPass, newPass)) {
        Terminal::printSuccess("Password changed successfully.");
    } else {
        Terminal::printError("Current password is incorrect.");
    }
    Terminal::pause();
}

// ═══════════════════════════════════════════════════════════════════════════
//  SHARED TABLE RENDERERS
// ═══════════════════════════════════════════════════════════════════════════

void LibraryApp::printBookTable(const std::vector<Models::Book>& books) const {
    TableFormatter table({
        {"ID",        7, 12,  Alignment::Left},
        {"Title",    20, 38,  Alignment::Left},
        {"Author",   15, 25,  Alignment::Left},
        {"Category", 12, 18,  Alignment::Left},
        {"Year",      4,  6,  Alignment::Center},
        {"Avail",     5,  7,  Alignment::Center},
        {"Total",     5,  7,  Alignment::Center},
        {"Shelf",    10, 16,  Alignment::Left}
    });

    for (const auto& b : books) {
        std::string avail = b.isAvailable()
            ? (Color::Green + std::to_string(b.getAvailableCopies()) + Color::Reset)
            : (Color::Red + "0" + Color::Reset);

        table.addRow({
            b.getId(), b.getTitle(), b.getAuthor(), b.getCategory(),
            std::to_string(b.getPublicationYear()),
            std::to_string(b.getAvailableCopies()),
            std::to_string(b.getTotalCopies()),
            b.getShelfLocation()
        });
    }

    std::cout << "\n  Total: " << books.size() << " book(s)\n\n";
    table.render();
}

void LibraryApp::printLoanTable(const std::vector<Models::Loan>& loans, bool showUser) const {
    std::vector<TableColumn> cols = {
        {"Loan ID",   7, 12, Alignment::Left},
        {"Book",     15, 30, Alignment::Left},
        {"Due Date", 10, 12, Alignment::Center},
        {"Status",    8, 12, Alignment::Center},
        {"Fine ($)",  7, 10, Alignment::Right},
        {"Renewals",  4,  8, Alignment::Center}
    };
    if (showUser) {
        cols.insert(cols.begin() + 1, {"User ID", 7, 12, Alignment::Left});
    }

    TableFormatter table(cols);
    std::cout << "\n  Total: " << loans.size() << " loan(s)\n\n";

    if (loans.empty()) {
        table.render();
        return;
    }

    for (const auto& l : loans) {
        std::string fineStr = (l.getFineAmount() > 0) ?
            (std::ostringstream() << std::fixed << std::setprecision(2) << l.getFineAmount()).str() :
            "-";

        std::vector<std::string> row = {
            l.getLoanId(),
            l.getBookTitle().empty() ? l.getBookId() : l.getBookTitle(),
            l.getDueDate().toString(),
            l.getStatusString(),
            fineStr,
            std::to_string(l.getRenewalCount())
        };

        if (showUser) {
            row.insert(row.begin() + 1, l.getUserId());
        }

        table.addRow(row);
    }
    table.render();
}

void LibraryApp::printMemberTable(const std::vector<Models::User>& members) const {
    TableFormatter table({
        {"ID",         7, 12,  Alignment::Left},
        {"Username",  10, 20,  Alignment::Left},
        {"Email",     18, 35,  Alignment::Left},
        {"Phone",     12, 16,  Alignment::Left},
        {"Type",       8, 10,  Alignment::Center},
        {"Status",     8, 12,  Alignment::Center},
        {"Loans",      5,  7,  Alignment::Center}
    });

    std::cout << "\n  Total: " << members.size() << " member(s)\n\n";
    for (const auto& u : members) {
        table.addRow({
            u.getId(), u.getUsername(), u.getEmail(), u.getPhone(),
            u.getMembershipTypeString(), u.getStatusString(),
            std::to_string(u.getActiveLoans())
        });
    }
    table.render();
}

} // namespace LMS::UI
