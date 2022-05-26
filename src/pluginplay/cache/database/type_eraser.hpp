#pragma once
#include "database_api.hpp"
#include <pluginplay/any/any.hpp>

namespace pluginplay::cache::database {

/** @brief Specialize to change how an object of type T gets type-erased
 *
 *  @relates TypeEraser
 *
 *  The TypeEraser database uses the MakeAny class to type-erase objects of type
 *  @p T. Generally speaking the type-erasuer is done by calling
 *  `any::make_any_field`, but users can specialize this class for types when
 *  there is a better way to do the conversion.
 *
 *  N.B. Specializations are expected to have a function `convert` which takes
 *       an object of type U (U is const T&, or just T) and returns an object
 *       of type AnyField.
 *
 *  @tparam T The type being type-erased.
 */
template<typename T>
struct MakeAny {
    template<typename U>
    static auto convert(U&& v) {
        return any::make_any_field<T>(std::forward<U>(v));
    }
};

/** @brief Converts the keys to type-erased objects before storing.
 *
 *  This class wraps the process of type-erasing a key before inserting it into
 *  the wrapped database. This in turn allows the wrapped database to store keys
 *  of all sorts of types. N.B. the values are not type-erased and are instead
 *  forwarded verbatim to the wrapped db.
 *
 *  @tparam KeyType The type of keys the user is providing. It must be possible
 *                  to wrap objects of KeyType in an AnyField
 *  @tparam ValueType The type of the objects that the keys map to.
 */
template<typename KeyType, typename ValueType>
class TypeEraser : public DatabaseAPI<KeyType, ValueType> {
private:
    /// Type of the database API we implement
    using base_type = DatabaseAPI<KeyType, ValueType>;

public:
    /// Type of the keys, typedef of KeyType
    using typename base_type::key_type;

    /// Type of a read-only reference to a key, typedef of const KeyType&
    using typename base_type::const_key_reference;

    /// Type that the keys map to, typedef of ValueType
    using typename base_type::mapped_type;

    /// Type of an object holding a read-only reference
    using typename base_type::const_mapped_reference;

    /// Type used for type-erasure, typedef of any::AnyField
    using any_type = any::AnyField;

    /// Type of the API the wrapped DB must satisfy
    using wrapped_db = DatabaseAPI<any_type, mapped_type>;

    /// Type of a pointer to the wrapped DB
    using wrapped_mapper_pointer = std::shared_ptr<wrapped_db>;

    /** @brief Makes a new TypeEraser which wraps @p db.
     *
     *
     *
     *  @param[in] db A non-null pointer to the database this instance wraps.
     *                This database will type-erase keys before putting them
     *                into @p db.
     *
     *  @throw std::runtime_error if @p db is a null pointer. Strong throw
     *                            guarantee.
     */
    explicit TypeEraser(wrapped_mapper_pointer db);

public:
    /// type-erases key, then calls m_db_->count
    bool count_(const_key_reference key) const noexcept override;

    /// type-erases key, then calls m_db_->insert
    void insert_(key_type key, mapped_type value) override;

    /// type-erases key, then calls m_db_->free
    void free_(const_key_reference key) override;

    /// type-erases key, then calls m_db_->insert()
    const_mapped_reference at_(const_key_reference key) const override;

    /// Just calls m_db_->backup()
    void backup_() override { m_db_->backup(); }

    /// Just calls m_db_->dump
    void dump_() override { m_db_->dump(); }

private:
    /// Code factorization for type-erasing a read-only reference
    any_type wrap_(const_key_reference key) const;

    /// The DB storing the type-erased key to value relationships
    wrapped_mapper_pointer m_db_;
};

} // namespace pluginplay::cache::database

#include "type_eraser.ipp"
