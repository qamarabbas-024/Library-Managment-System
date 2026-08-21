#pragma once

#include <string>
#include <chrono>
#include <iostream>

namespace LMS::Utils {

class Date {
private:
    int m_year{1970};
    int m_month{1};
    int m_day{1};

public:
    Date() = default;
    Date(int year, int month, int day);

    static Date today();
    static Date fromString(const std::string& str); // YYYY-MM-DD
    std::string toString() const;

    Date addDays(int days) const;
    int daysUntil(const Date& other) const; // other - this
    bool isPast() const;
    bool isOverdue(const Date& dueDate) const;

    bool operator==(const Date& other) const;
    bool operator!=(const Date& other) const;
    bool operator<(const Date& other) const;
    bool operator<=(const Date& other) const;
    bool operator>(const Date& other) const;
    bool operator>=(const Date& other) const;

    int getYear() const { return m_year; }
    int getMonth() const { return m_month; }
    int getDay() const { return m_day; }

    bool isValid() const;
};

class DateTime {
private:
    std::chrono::system_clock::time_point m_timePoint;

public:
    DateTime();
    explicit DateTime(std::chrono::system_clock::time_point tp);

    static DateTime now();
    static DateTime fromString(const std::string& str);
    std::string toString(const std::string& format = "%Y-%m-%d %H:%M:%S") const;
    std::string toISODate() const;

    Date toDate() const;
};

} // namespace LMS::Utils
