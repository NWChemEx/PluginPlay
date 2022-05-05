// #pragma once

// namespace pluginplay::database::detail_ {

// template<typename KeyType>
// class FieldMapMapper {
// private:
//     using key_key_type = typename KeyType::key_type;

//     using key_value_type = AnyField

//       using value_key_type = typename ValueType::key_type;

//     using value_value_type = UUIDMapper<AnyField>;

// public:
//     using key_type = KeyType;

//     using mapped_type = ValueType;

//     using sub_db_type = DatabasePIMPL<key_value_type, value_value_type>;

//     using sub_db_pointer = std::shared_ptr<sub_db_type>;

//     FieldMapMapper(sub_db_pointer db) : m_db_(std::move(db)) {
//         if(!m_db_) throw std::runtime_error("Expected a non-null database");
//     }

//     bool count(const key_type& key) const noexcept override {
//         for(const auto& [k, v] : key)
//             if(!m_db_->count(v)) return false;
//         return true;
//     }

//     void insert(const key_type& key) {
//         for(const auto& [k, v] : key) m_db_->insert(v);
//     }

//     std::optional<mapped_type> at_(const key_type& key) {
//         mapped_type rv;
//         for(const auto& [k, v] : key) {
//             if(!m_db_->cont(v)) return std::optional<mapped_type>{};
//             rv.emplace(k, m_db_->at(v).get());
//         }
//         return const_mapped_reference{std::move(rv)};
//     }

//     void backup() { m_db_->backup(); }

//     void dump() { m_db_->dump(); }

// private:
//     sub_db_type m_db_;
// };

// } // namespace pluginplay::database::detail_
