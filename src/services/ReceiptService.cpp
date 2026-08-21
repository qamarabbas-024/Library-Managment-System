#include "services/ReceiptService.h"
#include "storage/CsvEngine.h"
#include "utils/DateTime.h"
#include "utils/StringUtils.h"
#include <sstream>
#include <iomanip>
#include <fstream>

namespace LMS::Services {

std::string ReceiptService::generateCheckoutReceipt(const Models::Loan& loan,
                                                    const Models::Book& book,
                                                    const Models::User& member) {
    std::ostringstream oss;
    oss << "===============================================================\n";
    oss << "               LIBRARY BOOK CHECKOUT RECEIPT                   \n";
    oss << "                     UNIVERSITY OF LMS                         \n";
    oss << "===============================================================\n";
    oss << "  Loan ID       : " << loan.getLoanId() << "\n";
    oss << "  Issue Date    : " << loan.getIssueDate().toString() << "\n";
    oss << "  DUE DATE      : " << loan.getDueDate().toString() << " [IMPORTANT]\n";
    oss << "---------------------------------------------------------------\n";
    oss << "  MEMBER DETAILS:\n";
    oss << "    Member ID   : " << member.getId() << "\n";
    oss << "    Name        : " << member.getUsername() << "\n";
    oss << "    Type        : " << member.getMembershipTypeString() << "\n";
    oss << "---------------------------------------------------------------\n";
    oss << "  BOOK DETAILS:\n";
    oss << "    Accession ID: " << book.getId() << "\n";
    oss << "    Title       : " << book.getTitle() << "\n";
    oss << "    Author      : " << book.getAuthor() << "\n";
    oss << "    Shelf       : " << book.getShelfLocation() << "\n";
    oss << "===============================================================\n";
    oss << "  Notice: Overdue items accrue a fine of $0.50 per day.\n";
    oss << "  Thank you for utilizing your University Library.\n";
    oss << "===============================================================\n";
    return oss.str();
}

std::string ReceiptService::generateReturnSlip(const Models::Loan& loan,
                                               const Models::Book& book,
                                               const Models::User& member) {
    std::ostringstream oss;
    oss << "===============================================================\n";
    oss << "                 LIBRARY BOOK RETURN SLIP                      \n";
    oss << "===============================================================\n";
    oss << "  Loan ID       : " << loan.getLoanId() << "\n";
    oss << "  Member        : " << member.getUsername() << " (" << member.getId() << ")\n";
    oss << "  Book          : " << book.getTitle() << " (" << book.getId() << ")\n";
    oss << "  Due Date      : " << loan.getDueDate().toString() << "\n";
    oss << "  Return Date   : " << loan.getReturnDate().toString() << "\n";
    oss << "---------------------------------------------------------------\n";
    if (loan.getFineAmount() > 0) {
        oss << "  Status        : RETURNED WITH OVERDUE FINE\n";
        oss << "  Fine Incurred : $" << std::fixed << std::setprecision(2) << loan.getFineAmount() << "\n";
        oss << "  Fine Paid     : " << (loan.isFinePaid() ? "YES [PAID IN FULL]" : "PENDING") << "\n";
    } else {
        oss << "  Status        : RETURNED ON TIME [NO FINES ACCRUED]\n";
    }
    oss << "===============================================================\n";
    return oss.str();
}

std::string ReceiptService::generateFineReceipt(const Models::Loan& loan,
                                                const Models::User& member,
                                                double amountPaid,
                                                const std::string& cashier) {
    std::ostringstream oss;
    oss << "===============================================================\n";
    oss << "                OFFICIAL FINE PAYMENT RECEIPT                  \n";
    oss << "===============================================================\n";
    oss << "  Receipt No.   : RCP-" << loan.getLoanId() << "\n";
    oss << "  Date & Time   : " << Utils::DateTime::now().toString() << "\n";
    oss << "  Cashier       : " << cashier << "\n";
    oss << "---------------------------------------------------------------\n";
    oss << "  Member ID     : " << member.getId() << " (" << member.getUsername() << ")\n";
    oss << "  Loan ID       : " << loan.getLoanId() << "\n";
    oss << "  Book Title    : " << loan.getBookTitle() << "\n";
    oss << "---------------------------------------------------------------\n";
    oss << "  Amount Paid   : $" << std::fixed << std::setprecision(2) << amountPaid << "\n";
    oss << "  Payment Status: " << (loan.isFinePaid() ? "COMPLETED" : "PARTIAL PAYMENT") << "\n";
    oss << "===============================================================\n";
    return oss.str();
}

bool ReceiptService::exportReceiptToFile(const std::string& receiptText, const std::string& filename) {
    std::string path = "exports/" + filename;
    Storage::CsvEngine::ensureDirectoryExists(path);

    std::ofstream fout(path);
    if (!fout.is_open()) return false;
    fout << receiptText;
    fout.close();
    return true;
}

} // namespace LMS::Services
