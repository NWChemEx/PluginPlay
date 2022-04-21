#pragma once
#include <map>
#include <pluginplay/database/detail_/database_pimpl.hpp>

namespace testing {

/* Many of the DatabasePIMPL implementations require a database to be under
 * them. For unit testing purposes this is easiest if that database is just a
 * std::map. This class wraps a std::map in an API consistent with the API
 * of DatabasePIMPL. We unit test this class in
 * tests/pluginplay/database/detail_/map.cpp
 */
template<typename KeyType, typename ValueType>
class Map
  : public pluginplay::database::detail_::DatabasePIMPL<KeyType, ValueType> {
private:
    /// Type the class implements
    using base_type =
      pluginplay::database::detail_::DatabasePIMPL<KeyType, ValueType>;

public:
    using map_type               = std::map<KeyType, ValueType>;
    using key_type               = typename base_type::key_type;
    using const_key_reference    = typename base_type::const_key_reference;
    using mapped_type            = typename base_type::mapped_type;
    using const_mapped_reference = typename base_type::const_mapped_reference;

    Map(map_type map = {}) : m_map_(std::move(map)) {}

    auto& map() { return m_map_; }

    const auto& map() const { return m_map_; }

protected:
    bool count_(const_key_reference key) const noexcept override {
        return m_map_.count(key);
    }

    void insert_(key_type key, mapped_type value) override {
        m_map_[std::move(key)] = std::move(value);
    }

    void free_(const_key_reference key) override { m_map_.erase(key); }

    const_mapped_reference at_(const_key_reference key) const override {
        return const_mapped_reference(m_map_.at(key));
    }

    void backup_() override {}

    void dump_() override { m_map_.clear(); }

private:
    map_type m_map_;
};

} // namespace testing
