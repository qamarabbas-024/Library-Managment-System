#include "utils/Barcode.h"
#include "utils/StringUtils.h"
#include "utils/Terminal.h"
#include <sstream>
#include <iomanip>

namespace LMS::Utils {

std::string Barcode::renderBarcode(const std::string& code) {
    std::ostringstream oss;
    std::string clean = StringUtils::toUpper(StringUtils::trim(code));
    if (clean.empty()) clean = "LMS-CODE";

    oss << "  ||";
    for (char c : clean) {
        int val = static_cast<int>(c);
        if (val % 3 == 0)      oss << "|| | ";
        else if (val % 3 == 1) oss << "| || ";
        else                   oss << "|| ||";
    }
    oss << "||\n";

    oss << "  ||";
    for (char c : clean) {
        int val = static_cast<int>(c);
        if (val % 3 == 0)      oss << "|| | ";
        else if (val % 3 == 1) oss << "| || ";
        else                   oss << "|| ||";
    }
    oss << "||\n";

    oss << "  *" << clean << "*";
    return oss.str();
}

std::string Barcode::renderLibraryCard(const std::string& memberId,
                                      const std::string& username,
                                      const std::string& membershipType,
                                      const std::string& issueDate) {
    std::ostringstream oss;
    oss << "  +-------------------------------------------------------------+\n";
    oss << "  |                   LIBRARY MEMBERSHIP CARD                   |\n";
    oss << "  |                      UNIVERSITY OF LMS                      |\n";
    oss << "  +-------------------------------------------------------------+\n";
    oss << "  | Member ID   : " << StringUtils::padRight(memberId, 44) << "|\n";
    oss << "  | Name        : " << StringUtils::padRight(username, 44) << "|\n";
    oss << "  | Type        : " << StringUtils::padRight(StringUtils::toUpper(membershipType), 44) << "|\n";
    oss << "  | Issued Date : " << StringUtils::padRight(issueDate, 44) << "|\n";
    oss << "  +-------------------------------------------------------------+\n";
    oss << "  |  BARCODE:                                                   |\n";
    
    // Add visual barcode
    std::string barLine = "  |  ||| | |||| | || | |||| || | || |||| | || |||              |\n";
    oss << barLine;
    oss << barLine;
    oss << "  |  *" << memberId << "*" << StringUtils::padRight("", 56 - memberId.length()) << "|\n";
    oss << "  +-------------------------------------------------------------+\n";
    return oss.str();
}

std::string Barcode::renderBookTag(const std::string& bookId,
                                   const std::string& title,
                                   const std::string& isbn,
                                   const std::string& shelf) {
    std::ostringstream oss;
    oss << "  +-------------------------------------------------------------+\n";
    oss << "  |                     BOOK ACCESSION TAG                      |\n";
    oss << "  +-------------------------------------------------------------+\n";
    oss << "  | Accession ID: " << StringUtils::padRight(bookId, 44) << "|\n";
    oss << "  | Title       : " << StringUtils::padRight(StringUtils::truncate(title, 42), 44) << "|\n";
    oss << "  | ISBN        : " << StringUtils::padRight(isbn, 44) << "|\n";
    oss << "  | Location    : " << StringUtils::padRight(shelf, 44) << "|\n";
    oss << "  +-------------------------------------------------------------+\n";
    return oss.str();
}

} // namespace LMS::Utils
