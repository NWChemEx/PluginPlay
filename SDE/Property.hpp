#pragma once
#include "SDE/SDEAny.hpp"

namespace SDE {

/** @brief Class designed to wrap a returned computed result.
 *
 *  @relates ComputedProperties
 *
 *  This class wraps each of the objects stored in the ComputedProperties class
 *  and is not really intended for use apart from the ComputedProperties class.
 *  At the moment this is a simple class that just wraps an `SDEAny` instance.
 *  Down the road one can imagine making it more like a continuation, where
 *  instead of holding the value it holds a callback that computes the value.
 *
 *  Generally speaking the objects returned by modules are heavy. For this
 *  reason this class stores the object as a shared_ptr. A handle to this shared
 *  pointer can be obtained using the `get` function (modeled after the
 *  `shared_ptr` class itself). Typically, however, a user will want to go
 *  through the `value` function which returns a const reference to the actual
 *  result (const reference because the object is a result of a scientific
 *  computation and therefore should be treated as immutable).
 *
 */
class Property{
public:
    /// Type of the managed pointer within the class
    using const_pointer = std::shared_ptr<const detail_::SDEAny>;

    //@{
    /** @name Ctors and Assignment Operators
     *
     *  @warning Copy construction/assignment are shallow copies and not deep
     *           copies. The reasoning is that a deep copy would be pointless
     *           because the resulting objects would be identical, but still
     *           immutable.
     *
     *  1) Default ctor
     *  2 and 3) Copy ctor/assignment operator
     *  4 and 5) Move ctor/assignment operator
     *
     *  @param rhs For 2 and 3 the object to copy. For 4 and 5 the object to
     *         take ownership of. For 4 and 5, @p rhs is in a valid, but
     *         otherwise undefined state after the call.
     *
     *  @throw None All ctors/assignment operators are no throw guarantee.
     */
    Property() = default;
    Property(const Property& rhs) = default;
    Property& operator=(const Property& rhs) = default;
    Property(Property&& rhs) = default;
    Property& operator=(Property&& rhs) = default;
    //@}

    /** @brief Deletes the current Property instance.
     *
     *  Since state is managed by a `shared_ptr`, the deletion of the actual
     *  object may not occur until when the `Property` instance is deleted.
     *
     *  @throw None No throw gurantee.
     */
    ~Property() = default;

    /** @brief Allows logic dispatching based on whether or not an object is
     *         being managed.
     *
     * @return True if the current instance is managing an object and false
     *         otherwise.
     *
     * @throw None No throw guarantee.
     */
    explicit operator bool() const noexcept { return static_cast<bool>(prop_); }

    /** @brief Returns the managed state of the instance.
     *
     * As a user you typically want to use the value function which returns the
     * actual value of the property (in a read-only manner). This function
     * returns the shared_ptr that is managing the type-erased value. The
     * primary usage of this function is to provide the Cache a handle to the
     * data. The function is public as there are conceivably scenarios where the
     * user may want a handle to the data in case the Cache and the original
     * ComputedProperties instance go out of scope.
     *
     * @return The shared_ptr managing the state of this object.
     *
     * @throw None No throw guarantee.
     */
    const_pointer get() const noexcept { return prop_; }

    /** @brief Returns a read-only variant of the actual object.
     *
     * This is the function users typically want to use. It requires that you
     * know what the type of the wrapped object is. Typically that will be the
     * case, however, if you do not know the type for certain you are best
     * using `get` and accessing the object through detail_::SDEAny's API.
     *
     * @tparam T The type of the object.
     * @return A read-only variant of the actual object.
     * @throw std::bad_cast if the wrapped object is not of type @p T. Strong
     *        throw guarantee.
     */
    template<typename T>
    const T& value() const {return detail_::SDEAnyCast<const T&>(*prop_); }

    /** @brief Used to add a value to the current instance.
     *
     * There are two scenarios, we are adding an already managed object (*i.e.*,
     * the user gave us a `shared_ptr`) or we have been given the actual object
     * and we now need to manage it. This function handles both scenarios by
     * dispatching on the type of the input.
     *
     * @tparam T The type of the input.
     * @param value The object we are trying to add to the current instance.
     * @throw std::bad_alloc if there is insufficient memory to create the
     *        managed pointer.
     */
    template<typename T>
    void put(T&& value) {
        //If it's a shared_ptr just assign it, otherwise make a pointer
        if constexpr (std::is_convertible_v<const_pointer, std::decay_t<T>>) {
            prop_ = value;
        }
        else {
            auto new_any = detail_::make_SDEAny<T>(std::forward<T>(value));
            prop_ = std::make_shared<detail_::SDEAny>(std::move(new_any));
        }
    }
private:
    /// The actual managed state
    std::shared_ptr<const detail_::SDEAny> prop_;
};

} //End namespace
