#pragma once

#include "utils/DateTime.hpp"
#include <string>
#include <vector>

namespace LMS::Models {

enum class ReservationStatus {
    Pending,
    Notified,
    Fulfilled,
    Cancelled,
    Expired
};

class Reservation {
private:
    std::string m_reservationId;
    std::string m_userId;
    std::string m_bookId;
    Utils::Date m_reserveDate;
    ReservationStatus m_status{ReservationStatus::Pending};
    int m_priority{1};

public:
    Reservation() = default;
    Reservation(std::string id, std::string uid, std::string bookId, Utils::Date date, 
                ReservationStatus status = ReservationStatus::Pending, int priority = 1);

    const std::string& getId() const { return m_reservationId; }
    const std::string& getUserId() const { return m_userId; }
    const std::string& getBookId() const { return m_bookId; }
    const Utils::Date& getReserveDate() const { return m_reserveDate; }
    ReservationStatus getStatus() const { return m_status; }
    int getPriority() const { return m_priority; }

    std::string getStatusString() const;
    void setStatus(ReservationStatus s) { m_status = s; }

    std::vector<std::string> toCSV() const;
    static Reservation fromCSV(const std::vector<std::string>& row);
    static ReservationStatus parseStatus(const std::string& str);
};

} // namespace LMS::Models
