#include "storage/ReservationRepository.h"
#include "storage/CsvEngine.h"
#include <iomanip>
#include <sstream>

namespace LMS::Storage {

ReservationRepository::ReservationRepository(std::string filepath)
    : m_filepath(std::move(filepath)) {
    reload();
}

void ReservationRepository::rebuildIndex() {
    m_idIndex.clear();
    for (size_t i = 0; i < m_reservations.size(); ++i) {
        m_idIndex[m_reservations[i].getId()] = i;
    }
}

void ReservationRepository::reload() {
    m_reservations.clear();
    auto rows = CsvEngine::read(m_filepath);
    for (const auto& row : rows) {
        if (!row.empty()) {
            m_reservations.push_back(Models::Reservation::fromCSV(row));
        }
    }
    rebuildIndex();
}

bool ReservationRepository::flush() {
    std::vector<std::vector<std::string>> rows;
    rows.reserve(m_reservations.size());
    for (const auto& r : m_reservations) {
        rows.push_back(r.toCSV());
    }
    return CsvEngine::write(m_filepath, rows);
}

std::vector<Models::Reservation> ReservationRepository::findAll() const {
    return m_reservations;
}

std::optional<Models::Reservation> ReservationRepository::findById(const std::string& id) const {
    auto it = m_idIndex.find(id);
    if (it != m_idIndex.end()) {
        return m_reservations[it->second];
    }
    return std::nullopt;
}

bool ReservationRepository::save(const Models::Reservation& entity) {
    if (existsById(entity.getId())) {
        return update(entity);
    }
    m_reservations.push_back(entity);
    rebuildIndex();
    return flush();
}

bool ReservationRepository::update(const Models::Reservation& entity) {
    auto it = m_idIndex.find(entity.getId());
    if (it == m_idIndex.end()) return false;
    m_reservations[it->second] = entity;
    return flush();
}

bool ReservationRepository::remove(const std::string& id) {
    auto it = m_idIndex.find(id);
    if (it == m_idIndex.end()) return false;
    m_reservations.erase(m_reservations.begin() + it->second);
    rebuildIndex();
    return flush();
}

bool ReservationRepository::existsById(const std::string& id) const {
    return m_idIndex.find(id) != m_idIndex.end();
}

std::vector<Models::Reservation> ReservationRepository::findByUserId(const std::string& userId) const {
    std::vector<Models::Reservation> results;
    for (const auto& r : m_reservations) {
        if (r.getUserId() == userId) {
            results.push_back(r);
        }
    }
    return results;
}

std::vector<Models::Reservation> ReservationRepository::findPendingByBookId(const std::string& bookId) const {
    std::vector<Models::Reservation> results;
    for (const auto& r : m_reservations) {
        if (r.getBookId() == bookId && r.getStatus() == Models::ReservationStatus::Pending) {
            results.push_back(r);
        }
    }
    return results;
}

std::string ReservationRepository::generateNextId() const {
    int maxNum = 0;
    for (const auto& r : m_reservations) {
        try {
            std::string id = r.getId();
            if (id.rfind("RES-", 0) == 0) {
                id = id.substr(4);
            }
            int n = std::stoi(id);
            if (n > maxNum) maxNum = n;
        } catch (...) {}
    }
    std::ostringstream oss;
    oss << "RES-" << std::setfill('0') << std::setw(4) << (maxNum + 1);
    return oss.str();
}

} // namespace LMS::Storage
