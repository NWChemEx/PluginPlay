#pragma once
#include "NWChemExRuntime/Promise.hpp"
#include <vector>

///Default namespace for the NWChemExRuntime library
namespace NWChemExRuntime {

/**
 * @brief An "abstract base class" for the various devices on which parallel
 * operations can be run.
 *
 * This class serves to insulate NWChemEx from the semantics of the
 * parallelization libraries for each piece of hardware.  Most parallel
 * libraries only target one or two pieces of hardware and of course they do it
 * with different syntaxes.  This class provides a consistent API so that the
 * backends may be changed (as will be necessary to cover all target hardware
 * platforms) without having to change the code.
 *
 * This class works off static polymorphism via the curiously recurring template
 * pattern (CRTP) where by this class is down cast to the more derived class and
 * a function having a predefined API is called to handle the difference in
 * details.  Thus to define a new device you must implement the following:
 *
 * @code
 * //R is the return type of an invocation of the task
 * template<typename R, typename FxnType,typename...Args>
 * void add_task_(Promise<R>&&,FxnType,Args...);
 *
 * template<typename R, typename ItrType, typename FxnType>
 * void foreach_(Promise<std::vector<R>>&&,ItrType,ItrType,FxnType);
 *
 * template<typename R, typename ItrType, typename FxnType,typename RedFxn>
 * void reduction_(Promise<R>&&,ItrType,ItrType,FxnType,RedFxn);
 *
 * @endcode
 * where aside from the first arguments (the promise instances) all functions
 * have the same arguments, with the same semantics as the corresponding
 * functions in this Device class.  The implementation functions are expected
 * to put the results in the promise instance they are given.
 *
 * @note A lot of backends separate the type of iteration (parallel for,
 * reduction, etc.)  from the details of where that operation is running.  As
 * we are not actually implementing the algorithms, but rather wrapping them, we
 * instead have opted to combine them.
 *
 * @tparam Derived The class implementing the parallel calls to some library.
 */
template<typename Derived>
class Device {
public:
    virtual ~Device()noexcept=default;

    /**
     * @brief Adds a task to the task queue.
     *
     * We do not expect user's to add single tasks often, rather we expect them
     * to largely use the other APIs of this class.  Nevertheless, sometimes
     * adding a series of single tasks is required (such as when each task has a
     * different signature or when computing the input to a function takes a
     * long time).
     *
     * @tparam FxnType The type of the callable object (lambda, function, or
     *         functor) that will run your task.
     * @tparam Args The types of the arguments to be passed to the task.  Each
     *         argument will be perfectly forwarded to the underlying task.
     * @param task A function that can be invoked with the specified
     *        arguments.  When called, the task should perform its function
     *        and return the result.
     * @return A future instance that can be used to retrieve the result
     *         at a later point in time.
     */
    template<typename FxnType,typename...Args>
    auto add_task(FxnType task, Args&&...args)
    {
        using ReturnType=decltype(task(std::forward<Args>(args)...));
        Promise<ReturnType> p;
        auto rv=p.get_future();
        down_cast()->add_task_(std::move(p),
                               task,std::forward<Args>(args)...);
        return std::move(rv);
    }

    /**
     * @brief Runs a function with each value of input in some range.
     *
     * Often we want to call the same function with a series of inputs.  This
     * function assumes that those inputs lie in some iterable range and that
     * the user wants the individual results back in a contiguous array.
     *
     * @note The resulting future is blocks until the entire range of tasks has
     * run.
     *
     * @tparam FxnType The type of the callable object (lambda, function, or
     *         functor) that will run your task.
     * @tparam ItrType Type of an iterator over the element range.Must satisfy
     *         the concept of forward iterator.  Better if it is a random
     *         access iterator too.
     * @param begin An iterator pointing to the first input element.
     * @param end An iterator pointing to just past the last input element.
     * @param task A callable object that takes an element in the input range
     *        and returns an object of type @p R.
     * @return A future to an std::vector where element i is semantically given
     *         by calling @p task with the ith input in the range [begin,end).
     */
    template<typename FxnType, typename ItrType>
    auto foreach(ItrType begin, ItrType end, FxnType task)
    {
        using ReturnType=decltype(task(*begin));
        Promise<std::vector<ReturnType>> p;
        auto rv=p.get_future();
        down_cast()->foreach_(std::move(p),begin,end,task);
        return rv;
    };

    /**
     * @brief Given a range of input, calls a function on each input, and
     *        combines the results.
     * @tparam R The type of the output of each task.  Must by copyable and
     *         moveable.
     * @tparam ItrType The type of an iterator to the range of input values.
     *         Must satisfy forward iterator, but better if it additionally
     *         satisfies random access iterator.
     * @tparam ReductionFxn The type of a callable object taking two @p R
     *         instances and then returning the result of combining the two
     *         instances.
     * @param begin An iterator pointing to the first element in the range of
     *        inputs.
     * @param end An iterator pointing to just past the last element in the
     *        range of inputs.
     * @param task A function that when given an input value in the range
     *        [begin,end) returns an instance of type @p R.
     * @return The result of calling @p ReductionFxn on the outputs of each
     *         task's results.
     */
    template<typename FxnType, typename ItrType, typename ReduceFxn>
    auto reduce(ItrType begin, ItrType end, FxnType task, ReduceFxn red)
    {
        using ReturnType=decltype(task(*begin));
        Promise<ReturnType> p;
        auto rv=p.get_future();
        down_cast()->template reduce_(std::move(p),begin,end,task,red);
        return rv;
    };

private:

    /**
     * @brief Function for down casting to the class implementing the queue.
     *
     * I decided that the actual down cast looks messy, so in an attempt to
     * make this class more readable I defined this function.
     *
     * @return A pointer to this instance, except down cast to be an instance
     * of the derived class
     * @throw None.  No throw guarantee.
     */
    Derived* down_cast()noexcept
    {
        return static_cast<Derived*>(this);
    }
};

}


