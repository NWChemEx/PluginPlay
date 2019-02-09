#pragma once
#include "SDE/Types.hpp"
#include "SDE/Utility.hpp"
#include "SDE/detail_/SDEAny.hpp"
#include <memory>
#include <string>

namespace SDE {
namespace detail_ {
class ModuleResultPIMPL;
}

/** @brief Holds the results of calling a module's run function.
 *
 *  The results computed by a module are handled similar to the inputs to a
 *  module. The big difference here is that we store the results as `shared_ptr`
 *  instances. The reason for this is that most results are being used not only
 *  at the return site, but also by the cache (here "used" means someone has a
 *  handle to them, all results are read-only to protect their integrity). The
 *  class itself will hide this detail from the user, if the user would like, by
 *  automatically dereferencing the `shared_ptr` depending on the requested
 *  type.
 *
 */
class ModuleResult {
public:
    /// The type of a `shared_ptr` to a type-erased value
    using shared_any = std::shared_ptr<const type::any>;

    ///@{
    /** @name Ctors and Assignment Operators
     *
     *  The functions in this section create a ModuleResult instance with a
     *  default state, or state that is copied/taken from another instance.
     *  Customization of the state is accomplished using the setters, which
     *  support a factory-pattern syntax. Copies are deep copies with regards
     *  to metadata and shallow copies with respect to the value. This design
     *  accounts for the fact that the wrapped value is immutable and thus there
     *  is no point in copying it.
     *
     *  @param rhs The instance to copy/move state from. For move construction
     *         and assignment, @p rhs will be in a valid, but otherwise
     *         undefined state after the operation.
     *  @throw std::bad_alloc if there is insufficient memory to allocate a new
     *         pimpl. 2 and 3 will additionally throw if there is insufficient
     *         memory to copy the state
     */
    ModuleResult();
    ModuleResult(const ModuleResult& rhs);
    ModuleResult& operator=(const ModuleResult& rhs);
    ModuleResult(ModuleResult&& rhs) noexcept;
    ModuleResult& operator=(ModuleResult&& rhs) noexcept;
    ///@}

    /** @brief Standard destructor
     *
     *  Since values are ultimately stored within `shared_ptr` instances
     *  references to the wrapped value will continue to remain valid until the
     *  reference count goes to zero. References to metadata, however, will be
     *  invalidated once the dtor is called.
     *
     *  @throw none No throw guarantee.
     */
    ~ModuleResult() noexcept;

    ///@{
    /** @name Getters
     *
     *  The functions in this section can be used to view the state of the
     *  instance. Of note, the `value` member supports retreiving either the
     *  value itself, or the `shared_ptr` wrapping the value. The returned type
     *  is determined by the value of the template type parameter.
     *
     *  Respectively the members retrieve:
     *
     *  - the wrapped value, and
     *  - the description.
     *
     * @tparam T The type to return the wrapped value as.
     * @return The requested piece of state.
     * @throw std::bad_cast if the wrapped object can not be converted to @p T.
     *        Strong throw guarantee.
     * @throw none retrieval of the description is no throw guarantee.
     */
    template<typename T>
    T value() const {
        using clean_T = std::decay_t<T>;
        if constexpr(std::is_same_v<shared_any, clean_T>)
            return at_();
        else if constexpr(detail_::IsSharedPtr<clean_T>::value) {
            using type = typename clean_T::element_type;
            return T(at_(), &value<type&>());
        } else
            return detail_::SDEAnyCast<T>(*at_());
    }

    const type::description& description() const noexcept;
    ///@}

    ///@{
    /** @name Setters
     *
     * Members in this section can be used to change the state of the instance.
     *
     * Respectively these members change:
     *
     * - the value wrapped in the instance,
     * - the expected type of the wrapped value, and
     * - the description of the value.
     *
     * @tparam T The type of the wrapped value.
     * @param new_value The value to hold. If it is a `shared_ptr` the held
     *        value will be an alias to the input. Otherwise a new `shared_ptr`
     *        will be created to hold the value.
     *
     * @throw std::runtime_error is thrown by 1 if the type has not been set
     *        yet. Strong throw guarantee.
     * @throw std::invalid_argument is thrown by 1 if @p T is not convertible
     *        to the type this instance is supposed to wrap. Strong throw
     *        guarantee.
     * @throw std::bad_alloc is thrown by 1 if there is insufficient memory
     *        to create a `shared_ptr` for the value. Strong throw guarantee.
     *
     */
    template<typename T>
    void change(T&& new_value) {
        using clean_T = std::decay_t<T>;
        constexpr bool is_shared_any =
          std::is_same_v<clean_T, shared_any> || // is shared_ptr<const any>
          std::is_same_v<clean_T, std::shared_ptr<type::any>>; // no const
        if constexpr(is_shared_any)
            change_(std::forward<T>(new_value));
        else
            change_(std::move(wrap_value_(std::forward<T>(new_value))));
    }

    template<typename T>
    auto& set_type() {
        constexpr bool is_clean = std::is_same_v<std::decay_t<T>, T>;
        static_assert(is_clean, "Results must be unqualified types.");
        return set_type_(typeid(T));
    }

    ModuleResult& set_description(type::description desc) noexcept;
    ///@}

    ///@{
    /** @name Comparison operators
     *
     *  Functions in this section can be used to compare ModuleResult instances.
     *  Two instances are equivalent if their values compare equivalent and the
     *  associated metadata compares equal.
     *
     *  @param rhs The instance to compare to.
     *  @return True if the comparison is true and false otherwise.
     *  @throw ??? throws if the wrapped type's comparison operator throws.
     */
    bool operator==(const ModuleResult& rhs) const;
    bool operator!=(const ModuleResult& rhs) const { return !((*this) == rhs); }
    ///@}
private:
    /// Helper function for wrapping a value in an SDEAny
    template<typename T>
    static type::any wrap_value_(T&& new_value) {
        using clean_T = std::decay_t<T>;
        return detail_::make_SDEAny<clean_T>(std::forward<T>(new_value));
    }

    ///@{
    /** @name Bridge functions.
     *
     *  These functions bridge the gap between the templated public members of
     *  the class and the pimpled backend. These functions work by
     *  taking/returning objects of common types, typically SDEAny.
     *
     * @param new_value the value we will be wrapping.
     * @param type The type that wrapped values must be.
     *
     * @return 1 returns the SDEAny that is holding the value.
     * @return 4 returns the current instance after setting the type.
     */
    const shared_any& at_() const noexcept;

    void change_(type::any new_value);
    void change_(shared_any new_value) noexcept;

    ModuleResult& set_type_(const std::type_info& type) noexcept;
    ///@}

    /// The object that holds the actual state of the instance.
    std::unique_ptr<detail_::ModuleResultPIMPL> pimpl_;
};

} // namespace SDE
