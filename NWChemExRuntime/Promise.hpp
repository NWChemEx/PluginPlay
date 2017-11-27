#pragma once

namespace NWChemExRuntime {

/**
 * @brief The mechanism for a task to provide a result obtained in parallel.
 *
 * Together the Promise/Future classes abstract the mechanism for asynchronous
 * task-based parallelism.  The Promise is the setting part of that mechanism.
 *
 * When a task is added to a task queue it is given a Promise instance which is
 * essentially a buffer for the result.  When the task is done computing the
 * result it places the result in the buffer.  The value then sits in the buffer
 * until the user asks for it via the associated Future instance.
 *
 * @note Once a task sets a Promise the Promise will likely go out of
 * scope. In order to avoid having to explicitly keep the Promise alive we
 * instead require that all Promise instances are managed by shared_ptr
 * instances.  Thus when a Promise makes a Future, the Future gets a shared_ptr
 * to the Promise allowing the Promise to live on in the Future.  If you do not
 * create a Promise instance that lives in a shared_ptr you will likely get a
 * weird error.
 *
 * @tparam T The type of the result.  Must be either copyable and/or movable.
 */

template<typename T>
class Promise: public std::enable_shared_from_this<Promise<T>> {
public:
    /**
     * @brief Makes a Promise capable of holding a single T instance.
     *
     * After default creation a Promise holds only a null pointer to a T
     * instance.  Memory is allocated (if necessary) when set_value is called.
     *
     * @throw None No throw guarantee.
     */
    Promise()noexcept=default;

    /**
     * @brief Takes ownership of another promise.
     */

    ///Promises are non-copyable
    Promise(const Promise<T>& /*rhs*/)=delete;

    ///Promises are non-assignable
    Promise<T>& operator=(const Promise<T>& /*rhs*/)=delete;

    Future<T> get_future() const {
        return Future<T>(shared_from_this());
    }

    /**
     * @brief Member function that fulfills the promise.
     *
     * After a function finishes computing a value, its last task is to set the
     * value of the Promise it had to fulfill.
     *
     * @tparam T2
     * @param value
     */

    template<typename T2>
    void set_value(T2&& value)
    {
        std::make_unique<T>(std::forward<T2>(value));
    }

private:
    
    ///The value the user wants
    std::unique_ptr<T> value_;
};//End Promise
}


