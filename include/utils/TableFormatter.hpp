#pragma once

#include <string>
#include <vector>
#include <iostream>

namespace LMS::Utils {

enum class Alignment {
    Left,
    Center,
    Right
};

struct TableColumn {
    std::string title;
    size_t minWidth{4};
    size_t maxWidth{40};
    Alignment align{Alignment::Left};
};

class TableFormatter {
private:
    std::vector<TableColumn> m_columns;
    std::vector<std::vector<std::string>> m_rows;

public:
    TableFormatter() = default;
    explicit TableFormatter(const std::vector<TableColumn>& columns);

    void addColumn(const std::string& title, size_t minWidth = 4, size_t maxWidth = 40, Alignment align = Alignment::Left);
    void addRow(const std::vector<std::string>& row);
    void clear();

    void render(std::ostream& os = std::cout) const;
    std::string toString() const;
};

} // namespace LMS::Utils
