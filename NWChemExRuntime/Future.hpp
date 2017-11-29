#pragma once
#include <memory>

namespace NWChemExRuntime {

template<typename T> class Promise;

/**
 * @brief The "getting" end of the Future/Promise channel.
 *
 * Semantically this class behaves exactly like the STL future class.  In
 * particular this means you should only call get() once.
 *
 * The Future class acts as a claim check for the promised data.  By this we
 * mean it allows the user to retrieve their requested data at a later point in
 * time.
 *
 * @tparam T The type of the data within the Future.  Must meet the concepts of
 * copyable and/or moveable.
 */
template<typename T>
class Future {
public:

    /**
     * @brief Allows the user to create a Future placeholder instance.
     *
     * Default constructed Futures have no shared state and always have
     * valid()==false.  They can be made valid by moving a valid future into
     * them.
     *
     * @throw None. No throw guarantee.
     */
    Future()noexcept=default;

    ///Default destructor is fine
    ~Future()noexcept=default;

    ///Futures are not copyable
    Future(const Future<T>&)=delete;

    ///Futures are not assignable
    Future<T>& operator=(const Future<T>&)=delete;

    /**
     * @brief Takes ownership of another Future's shared state.
     *
     * @param rhs The Future instance we are taking the shared state of.
     * @throw None. No throw guarantee.
     */
    Future(Future<T>&& /*rhs*/)noexcept=default;

    /**
     * @brief Takes the contents of another Future by move semantics.
     *
     * @param rhs The Future instance we are taking the shared state of.
     * @return The current Future instance after taking ownership of the
     *         shared state of @p rhs
     * @throw None. No throw guarantee.
     */
    Future<T>& operator=(Future<T>&& /*rhs*/)noexcept=default;

    /**
     * @brief Returns the promised value to the user.
     *
     * This call is non-blocking if the value has already been computed and
     * blocking otherwise.  At the moment the blocking is a busy wait, so
     * you'll be down a thread.
     *
     * @warning Calling get twice is undefined behavior and will likely cause
     * your program to hand indefinitely.
     *
     * @return The requested value via std::move(value).
     */
    T get()
    {
        wait();
        return std::move(*(*value_));
    }

    /**
     * @brief Busy waits until the Future is ready
     *
     * @throw None. No throw guarantee.
     */
    void wait()noexcept
    {
        while(!(*value_)){}
    }

    /**
     * @brief  Returns true if this Future shares a state with a promise
     *
     * @note Valid is not the same as ready, i.e. a valid Future may still have
     *       to wait for its value to be computed.
     *
     * @return True if this Future contains a valid shared state.
     */
    bool valid()const noexcept
    {
        //use_count==0 if value_ is empty
        return value_.use_count();
    }
protected:
    friend class Promise<T>;

    /**
     * @brief Makes a Future containing a unique shared state.
     *
     * This constructor is only accessible from a Promise to ensure that all
     * valid Futures are assigned a single Promise.
     *
     * @param value The memory location for the shared state.
     * @throw None. Memory was allocated in Promise.  No throw guarantee.
     */
    Future(std::shared_ptr<std::unique_ptr<T>> value)noexcept:
            value_(value)
    {}

private:
    ///The value shared by the Future/Promise pair
    std::shared_ptr<std::unique_ptr<T>> value_;
};

/**
 * @brief Specialization of Future to Future<void>.
 *
 * See primary Future template for details.
 */
template<>
class Future<void> {
public:

    /**
     * @brief Allows the user to create a Future placeholder instance.
     *
     * Default constructed Futures have no shared state and always have
     * valid()==false.  They can be made valid by moving a valid future into
     * them.
     *
     * @throw None. No throw guarantee.
     */
    Future()noexcept=default;

    ///Default destructor is fine
    ~Future()noexcept=default;

    ///Futures are not copyable
    Future(const Future<void>&)=delete;

    ///Futures are not assignable
    Future<void>& operator=(const Future<void>&)=delete;

    ///@copydoc Future<T>::(Future<T>&&)noexcept
    Future(Future<void>&& /*rhs*/)noexcept=default;

    /**
     * @brief Takes the contents of another Future by move semantics.
     *
     * @param rhs The Future instance we are taking the shared state of.
     * @return The current Future instance after taking ownership of the
     *         shared state of @p rhs
     * @throw None. No throw guarantee.
     */
    Future<void>& operator=(Future<void>&& /*rhs*/)noexcept=default;

    /**
     * @brief Returns when .
     *
     * This call is non-blocking if the value has already been computed and
     * blocking otherwise.  At the moment the blocking is a busy wait, so
     * you'll be down a thread.
     *
     * @warning Calling get twice is undefined behavior and will likely cause
     * your program to hand indefinitely.
     *
     */
    void get()
    {
        wait();
        return;
    }

    /**
     * @brief Busy waits until the Future is ready
     *
     * @throw None. No throw guarantee.
     */
    void wait()noexcept
    {
        while(!(*value_)){}
    }

    /**
     * @brief  Returns true if this Future shares a state with a promise
     *
     * @note Valid is not the same as ready, i.e. a valid Future may still have
     *       to wait for its value to be computed.
     *
     * @return True if this Future contains a valid shared state.
     */
    bool valid()const noexcept
    {
        //use_count==0 if value_ is empty
        return value_.use_count();
    }
protected:
    friend class Promise<void>;

    /**
     * @brief Makes a Future containing a unique shared state.
     *
     * This constructor is only accessible from a Promise to ensure that all
     * valid Futures are assigned a single Promise.
     *
     * @param value The memory location for the shared state.
     * @throw None. Memory was allocated in Promise.  No throw guarantee.
     */
    Future(std::shared_ptr<bool> value)noexcept:
            value_(value)
    {}

private:
    ///The value shared by the Future/Promise pair
    std::shared_ptr<bool> value_;
};


}


