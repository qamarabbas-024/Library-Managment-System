#pragma once

#include "storage/IRepository.h"
#include "models/Reservation.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace LMS::Storage {

class ReservationRepository : public IRepository<Models::Reservation, std::string> {
private:
    std::string m_filepath;
    std::vector<Models::Reservation> m_reservations;
    std::unordered_map<std::string, size_t> m_idIndex;

    void rebuildIndex();

public:
    explicit ReservationRepository(std::string filepath = "data/reservations.csv");

    std::vector<Models::Reservation> findAll() const override;
    std::optional<Models::Reservation> findById(const std::string& id) const override;
    bool save(const Models::Reservation& entity) override;
    bool update(const Models::Reservation& entity) override;
    bool remove(const std::string& id) override;
    bool existsById(const std::string& id) const override;
    size_t count() const override { return m_reservations.size(); }
    void reload() override;
    bool flush() override;

    std::vector<Models::Reservation> findByUserId(const std::string& userId) const;
    std::vector<Models::Reservation> findPendingByBookId(const std::string& bookId) const;
    std::string generateNextId() const;
};

} // namespace LMS::Storage
