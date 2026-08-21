#include "utils/DateTime.h"
#include <sstream>
#include <iomanip>
#include <ctime>

namespace LMS::Utils {

Date::Date(int year, int month, int day)
    : m_year(year), m_month(month), m_day(day) {}

Date Date::today() {
    auto now = std::chrono::system_clock::now();
    std::time_t tt = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &tt);
#else
    localtime_r(&tt, &tm);
#endif
    return Date(tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
}

Date Date::fromString(const std::string& str) {
    if (str.length() < 10) return Date::today();
    try {
        int y = std::stoi(str.substr(0, 4));
        int m = std::stoi(str.substr(5, 2));
        int d = std::stoi(str.substr(8, 2));
        return Date(y, m, d);
    } catch (...) {
        return Date::today();
    }
}

std::string Date::toString() const {
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(4) << m_year << "-"
        << std::setfill('0') << std::setw(2) << m_month << "-"
        << std::setfill('0') << std::setw(2) << m_day;
    return oss.str();
}

Date Date::addDays(int days) const {
    std::tm tm{};
    tm.tm_year = m_year - 1900;
    tm.tm_mon = m_month - 1;
    tm.tm_mday = m_day + days;
    tm.tm_isdst = -1;

    std::time_t next = std::mktime(&tm);
    std::tm resTm{};
#ifdef _WIN32
    localtime_s(&resTm, &next);
#else
    localtime_r(&next, &resTm);
#endif
    return Date(resTm.tm_year + 1900, resTm.tm_mon + 1, resTm.tm_mday);
}

int Date::daysUntil(const Date& other) const {
    std::tm tm1{};
    tm1.tm_year = m_year - 1900;
    tm1.tm_mon = m_month - 1;
    tm1.tm_mday = m_day;
    tm1.tm_isdst = -1;
    std::time_t t1 = std::mktime(&tm1);

    std::tm tm2{};
    tm2.tm_year = other.m_year - 1900;
    tm2.tm_mon = other.m_month - 1;
    tm2.tm_mday = other.m_day;
    tm2.tm_isdst = -1;
    std::time_t t2 = std::mktime(&tm2);

    double diffSecs = std::difftime(t2, t1);
    return static_cast<int>(diffSecs / (60.0 * 60.0 * 24.0));
}

bool Date::isPast() const {
    return *this < Date::today();
}

bool Date::isOverdue(const Date& dueDate) const {
    return *this > dueDate;
}

bool Date::operator==(const Date& other) const {
    return m_year == other.m_year && m_month == other.m_month && m_day == other.m_day;
}

bool Date::operator!=(const Date& other) const {
    return !(*this == other);
}

bool Date::operator<(const Date& other) const {
    if (m_year != other.m_year) return m_year < other.m_year;
    if (m_month != other.m_month) return m_month < other.m_month;
    return m_day < other.m_day;
}

bool Date::operator<=(const Date& other) const {
    return *this < other || *this == other;
}

bool Date::operator>(const Date& other) const {
    return !(*this <= other);
}

bool Date::operator>=(const Date& other) const {
    return !(*this < other);
}

bool Date::isValid() const {
    return m_year >= 1900 && m_month >= 1 && m_month <= 12 && m_day >= 1 && m_day <= 31;
}

// DateTime
DateTime::DateTime() : m_timePoint(std::chrono::system_clock::now()) {}
DateTime::DateTime(std::chrono::system_clock::time_point tp) : m_timePoint(tp) {}

DateTime DateTime::now() {
    return DateTime(std::chrono::system_clock::now());
}

DateTime DateTime::fromString(const std::string& str) {
    if (str.empty()) return DateTime::now();
    std::tm tm{};
    std::istringstream ss(str);
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
    if (ss.fail()) {
        ss.clear();
        ss.str(str);
        ss >> std::get_time(&tm, "%Y-%m-%d");
        if (ss.fail()) return DateTime::now();
    }
    std::time_t tt = std::mktime(&tm);
    return DateTime(std::chrono::system_clock::from_time_t(tt));
}

std::string DateTime::toString(const std::string& format) const {
    std::time_t tt = std::chrono::system_clock::to_time_t(m_timePoint);
    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &tt);
#else
    localtime_r(&tt, &tm);
#endif
    char buf[128];
    std::strftime(buf, sizeof(buf), format.c_str(), &tm);
    return std::string(buf);
}

std::string DateTime::toISODate() const {
    return toString("%Y-%m-%d");
}

Date DateTime::toDate() const {
    return Date::fromString(toISODate());
}

} // namespace LMS::Utils
