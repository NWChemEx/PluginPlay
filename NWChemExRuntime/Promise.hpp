#pragma once
#include "NWChemExRuntime/Future.hpp"
#include <stdexcept> //For std::logic_error
#include <memory> //For shared_ptr

namespace NWChemExRuntime {

/**
 * @brief The set part of the Promise/Future abstraction for asynchronous
 * parallelization.
 *
 * Semantically this class behaves exactly like the STL promise class.  In
 * particular this includes:
 *
 * -Being a single use class (calling set_value more than once is not allowed)
 *
 * For threads the STL provides promise/future classes; however, they have an
 * annoying "feature" that they assume they are always running in a threaded
 * environment.  In other words, it's not possible to use a promise in the
 * same thread that the future lives in.  This class does not provide any such
 * restrictions.
 *
 * This class also serves as a buffer between the user accessing the data and
 * the backend computing it.  Hence, if need be, its possible to interject
 * additional abstractions into this class without affecting the user.
 *
 * Implementation note.  I didn't feel like seeing how the C++ library does it,
 * but my current solution is a nested smart pointer.  At the inner most level
 * we have a unique pointer because the Future/Promise refer to the same object.
 * Outside that we have a shared pointer to make sure the unique object persists
 * as long as the Future/Promise pair.
 *
 * @tparam T The type of the data we are storing.  Must satisfy the concept of
 *           moveable and/or copyable.
 *
 * @todo All operations need to be atomic
 */
template<typename T>
class Promise {
public:
    /**
     * @brief Creates a promise capable of storing an instance of type T.
     *
     * This constructor makes the address that the Future/Promise pair agree to
     * use for "message passing", i.e. the outer smart pointer.
     *
     * @throw std::bad_alloc if there is insufficient memory to allocate memory
     *        for a unique_ptr<T> instance.
     */
    Promise():
        value_(std::make_shared<std::unique_ptr<T>>())
    {}

    ///Promises can't be satisfied by multiple people
    Promise(const Promise<T>&)=delete;

    ///Promises can't be assigned to
    Promise<T>& operator=(const Promise<T>&)=delete;

    /**
     * @brief Takes ownership of another Promise's shared state.
     *
     * @param rhs The other Promise instance from which we are taking the state.
     *        After taking @p rhs's state it remains in a valid, but undefined
     *        state.
     * @throw None. No throw guarantee.
     */
    Promise(Promise<T>&& /*rhs*/)noexcept=default;

    /**
     * @brief Takes ownership of another Promise's shared state.
     *
     * @param rhs The Promise we are now responsible for fulfilling.  After this
     *        call @p rhs is in a valid, but undefined state.
     * @return The current instance with @p rhs's shared state.
     * @throw None. No throw guarantee.
     *
     */
    Promise<T>& operator=(Promise<T>&& /*rhs*/)noexcept=default;
    /**
     * @brief Creates a future which shares this promise's state.
     *
     * @note Each Promise can only be affiliated with one Future instance.
     *
     * @return A future which has access to this promise's shared state.
     * @throw std::logic_error if a Future is already affiliated with this
     *        Promise.
     */
    Future<T> get_future()
    {
        if(value_.use_count()>1)
            throw std::logic_error("A future already exists");
        return Future<T>(value_);
    }

    /**
     * @brief Fulfills the promise.
     *
     * @tparam T2 The type of the value we are setting, must be implicitly
     *         convertible to an instance of type T.
     * @param value The actual instance that is fulfilling the promise.
     * @throw std::logic_error if the Promise has already been satisfied. Strong
     *        throw guarantee.
     */
    template<typename T2>
    void set_value(T2&& value)
    {
        if(*value_)
            throw std::logic_error("Promise already set");
        auto unq_temp=std::make_unique<T>(std::forward<T2>(value));
        value_->swap(unq_temp);
    }
private:
    ///The value being shared by the Future/Promise pair
    std::shared_ptr<std::unique_ptr<T>> value_;
};

/**
 * @brief Specialization of Promise to Promise<void>
 *
 * See Promise<T> for details.
 *
 * @todo All operations need to be atomic
 */
template<>
class Promise<void> {
public:
    ///@copydoc Promise<T>::Promise()
    Promise():
            value_(std::make_shared<bool>(false))
    {}

    ///Promises can't be satisfied by multiple people
    Promise(const Promise<void>&)=delete;

    ///Promises can't be assigned to
    Promise<void>& operator=(const Promise<void>&)=delete;

    /**
     * @brief Takes ownership of another Promise's shared state.
     *
     * @param rhs The other Promise instance from which we are taking the state.
     *        After taking @p rhs's state it remains in a valid, but undefined
     *        state.
     * @throw None. No throw guarantee.
     */
    Promise(Promise<void>&& /*rhs*/)noexcept=default;

    /**
     * @brief Takes ownership of another Promise's shared state.
     *
     * @param rhs The Promise we are now responsible for fulfilling.  After this
     *        call @p rhs is in a valid, but undefined state.
     * @return The current instance with @p rhs's shared state.
     * @throw None. No throw guarantee.
     *
     */
    Promise<void>& operator=(Promise<void>&& /*rhs*/)noexcept=default;
    /**
     * @brief Creates a future which shares this promise's state.
     *
     * @note Each Promise can only be affiliated with one Future instance.
     *
     * @return A future which has access to this promise's shared state.
     * @throw std::logic_error if a Future is already affiliated with this
     *        Promise.
     */
    Future<void> get_future()
    {
        if(value_.use_count()>1)
            throw std::logic_error("A future already exists");
        return Future<void>(value_);
    }

    /**
     * @brief Fulfills the promise.
     *
     * @throw std::logic_error if the Promise has already been satisfied. Strong
     *        throw guarantee.
     */
    void set_value()
    {
        if(*value_)
            throw std::logic_error("Promise already set");
        *value_=true;
    }
private:
    ///The value being shared by the Future/Promise pair
    std::shared_ptr<bool> value_;
};


}


