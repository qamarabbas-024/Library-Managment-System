#include "utils/TableFormatter.h"
#include "utils/StringUtils.h"
#include "utils/Terminal.h"
#include <sstream>
#include <algorithm>

namespace LMS::Utils {

TableFormatter::TableFormatter(const std::vector<TableColumn>& columns)
    : m_columns(columns) {}

void TableFormatter::addColumn(const std::string& title, size_t minWidth, size_t maxWidth, Alignment align) {
    m_columns.push_back({title, minWidth, maxWidth, align});
}

void TableFormatter::addRow(const std::vector<std::string>& row) {
    m_rows.push_back(row);
}

void TableFormatter::clear() {
    m_rows.clear();
}

std::string TableFormatter::toString() const {
    std::ostringstream oss;
    render(oss);
    return oss.str();
}

void TableFormatter::render(std::ostream& os) const {
    if (m_columns.empty()) return;

    size_t colCount = m_columns.size();
    std::vector<size_t> colWidths(colCount);

    // Calculate column widths based on title and cell content
    for (size_t i = 0; i < colCount; ++i) {
        colWidths[i] = std::max(m_columns[i].title.length(), m_columns[i].minWidth);
        for (const auto& row : m_rows) {
            if (i < row.size()) {
                colWidths[i] = std::max(colWidths[i], row[i].length());
            }
        }
        colWidths[i] = std::min(colWidths[i], m_columns[i].maxWidth);
    }

    auto alignCell = [](const std::string& text, size_t width, Alignment align) -> std::string {
        std::string truncated = StringUtils::truncate(text, width);
        if (truncated.length() >= width) return truncated;
        size_t padTotal = width - truncated.length();

        switch (align) {
            case Alignment::Right:
                return std::string(padTotal, ' ') + truncated;
            case Alignment::Center: {
                size_t left = padTotal / 2;
                size_t right = padTotal - left;
                return std::string(left, ' ') + truncated + std::string(right, ' ');
            }
            case Alignment::Left:
            default:
                return truncated + std::string(padTotal, ' ');
        }
    };

    // Top border
    os << "  +";
    for (size_t i = 0; i < colCount; ++i) {
        os << std::string(colWidths[i] + 2, '-') << "+";
    }
    os << "\n";

    // Header row
    os << "  |";
    for (size_t i = 0; i < colCount; ++i) {
        os << " " << alignCell(m_columns[i].title, colWidths[i], Alignment::Center) << " |";
    }
    os << "\n";

    // Header separator
    os << "  +";
    for (size_t i = 0; i < colCount; ++i) {
        os << std::string(colWidths[i] + 2, '=') << "+";
    }
    os << "\n";

    // Data rows
    if (m_rows.empty()) {
        os << "  | " << alignCell("No records found.", [&]() {
            size_t total = 0;
            for (size_t w : colWidths) total += w + 3;
            return total > 3 ? total - 3 : 15;
        }(), Alignment::Center) << " |\n";
    } else {
        for (const auto& row : m_rows) {
            os << "  |";
            for (size_t i = 0; i < colCount; ++i) {
                std::string val = (i < row.size()) ? row[i] : "";
                os << " " << alignCell(val, colWidths[i], m_columns[i].align) << " |";
            }
            os << "\n";
        }
    }

    // Bottom border
    os << "  +";
    for (size_t i = 0; i < colCount; ++i) {
        os << std::string(colWidths[i] + 2, '-') << "+";
    }
    os << "\n";
}

} // namespace LMS::Utils
