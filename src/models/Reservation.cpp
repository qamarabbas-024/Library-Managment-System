#include "models/Reservation.hpp"
#include "utils/StringUtils.hpp"

namespace LMS::Models {

Reservation::Reservation(std::string id, std::string uid, std::string bookId, Utils::Date date, 
                         ReservationStatus status, int priority)
    : m_reservationId(std::move(id)),
      m_userId(std::move(uid)),
      m_bookId(std::move(bookId)),
      m_reserveDate(date),
      m_status(status),
      m_priority(priority) {}

std::string Reservation::getStatusString() const {
    switch (m_status) {
        case ReservationStatus::Pending: return "pending";
        case ReservationStatus::Notified: return "notified";
        case ReservationStatus::Fulfilled: return "fulfilled";
        case ReservationStatus::Cancelled: return "cancelled";
        case ReservationStatus::Expired: return "expired";
    }
    return "pending";
}

ReservationStatus Reservation::parseStatus(const std::string& str) {
    std::string s = Utils::StringUtils::toLower(str);
    if (s == "notified") return ReservationStatus::Notified;
    if (s == "fulfilled") return ReservationStatus::Fulfilled;
    if (s == "cancelled") return ReservationStatus::Cancelled;
    if (s == "expired") return ReservationStatus::Expired;
    return ReservationStatus::Pending;
}

std::vector<std::string> Reservation::toCSV() const {
    return {
        m_reservationId,
        m_userId,
        m_bookId,
        m_reserveDate.toString(),
        getStatusString(),
        std::to_string(m_priority)
    };
}

Reservation Reservation::fromCSV(const std::vector<std::string>& row) {
    if (row.size() < 3) return Reservation();
    std::string id = row[0];
    std::string uid = row[1];
    std::string bookId = row[2];
    Utils::Date date = (row.size() > 3) ? Utils::Date::fromString(row[3]) : Utils::Date::today();
    ReservationStatus status = (row.size() > 4) ? parseStatus(row[4]) : ReservationStatus::Pending;
    int priority = 1;
    try { if (row.size() > 5) priority = std::stoi(row[5]); } catch (...) {}

    return Reservation(id, uid, bookId, date, status, priority);
}

} // namespace LMS::Models
