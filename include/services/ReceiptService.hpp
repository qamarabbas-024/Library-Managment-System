#pragma once

#include "models/Loan.hpp"
#include "models/Book.hpp"
#include "models/User.hpp"
#include <string>

namespace LMS::Services {

class ReceiptService {
public:
    // Formats a loan checkout receipt
    static std::string generateCheckoutReceipt(const Models::Loan& loan,
                                               const Models::Book& book,
                                               const Models::User& member);

    // Formats a book return slip (with fine details if late)
    static std::string generateReturnSlip(const Models::Loan& loan,
                                          const Models::Book& book,
                                          const Models::User& member);

    // Formats a fine payment receipt
    static std::string generateFineReceipt(const Models::Loan& loan,
                                          const Models::User& member,
                                          double amountPaid,
                                          const std::string& cashier);

    // Exports the receipt string to a .txt file in exports/ folder
    static bool exportReceiptToFile(const std::string& receiptText, const std::string& filename);
};

} // namespace LMS::Services
