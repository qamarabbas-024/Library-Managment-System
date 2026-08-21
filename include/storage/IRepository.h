#pragma once

#include <vector>
#include <optional>
#include <functional>

namespace LMS::Storage {

template <typename T, typename ID>
class IRepository {
public:
    virtual ~IRepository() = default;

    virtual std::vector<T> findAll() const = 0;
    virtual std::optional<T> findById(const ID& id) const = 0;
    virtual bool save(const T& entity) = 0;
    virtual bool update(const T& entity) = 0;
    virtual bool remove(const ID& id) = 0;
    virtual bool existsById(const ID& id) const = 0;
    virtual size_t count() const = 0;
    virtual void reload() = 0;
    virtual bool flush() = 0;
};

} // namespace LMS::Storage
