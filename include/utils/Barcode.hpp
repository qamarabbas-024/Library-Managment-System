#pragma once

#include <string>
#include <vector>

namespace LMS::Utils {

class Barcode {
public:
    // Generates an ASCII representation of a Code-39 barcode
    static std::string renderBarcode(const std::string& code);
    
    // Generates an ASCII library membership card
    static std::string renderLibraryCard(const std::string& memberId,
                                         const std::string& username,
                                         const std::string& membershipType,
                                         const std::string& issueDate);

    // Generates an ASCII accession tag for books
    static std::string renderBookTag(const std::string& bookId,
                                     const std::string& title,
                                     const std::string& isbn,
                                     const std::string& shelf);
};

} // namespace LMS::Utils
