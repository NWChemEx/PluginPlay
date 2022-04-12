#pragma once
#include "../value.hpp"

namespace pluginplay::database::detail_ {

template<typename KeyType, typename ValueType>
class DatabasePIMPL {
public:
    using const_key_reference = const KeyType&;

    using mapped_type = KeyType;

    using mapped_reference = Value<mapped_type>;

    using const_mapped_reference = ConstValue<mapped_type>;

    bool count(const_key_reference key) const noexcept { return count_(key); }

    void insert(KeyType key, ValueType value);

    void free(const_key_reference key) { free_(key); }

    const_mapped_reference operator[](const_key_reference key) const;

    void backup() { backup_(); }

    void dump() { dump_(); }

protected:
    virtual bool count_(const_key_reference key) const noexcept = 0;

    virtual void insert_(KeyType key, ValueType value) = 0;

    virtual void free_(const_key_reference key) = 0;

    virtual const_mapped_reference at_(const_key_reference key) const = 0;

    virtual void backup_() = 0;

    virtual void dump_() = 0;
};

#define TPARAMS template<typename KeyType, typename ValueType>
#define DB_PIMPL DatabasePIMPL<KeyType, ValueType>

TPARAMS
void DB_PIMPL::insert(KeyType key, ValueType value) {
    insert_(std::move(key), std::move(value));
}

TPARAMS
typename DB_PIMPL::const_mapped_reference DB_PIMPL::operator[](
  const_key_reference key) const {
    return at_(key);
}

#undef TPARAMS
#undef DB_PIMPL

} // namespace pluginplay::database::detail_
