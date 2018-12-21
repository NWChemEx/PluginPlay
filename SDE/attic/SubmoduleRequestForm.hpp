#pragma once
#include <string>
#include <memory>
#include <typeindex>
#include <iostream>

namespace SDE {
namespace detail_{
///Class that actually implements the SubmoduleRequestForm
class SubmoduleRequestFormPIMPL;
}

/** @brief Struct for collecting the details of a particular request.
 *
 *  Right now we allow for a module to specify the property type a module must
 *  satisfy, and optionally which of the modules registered in the module
 *  manager should be used as the default.
 */
struct SubmoduleRequest {
    ///The type used to store the RTTI type for the property type
    using rtti_type = std::type_index;

    ///The type for the keys.
    using key_type = std::string;

    ///The property type the submodule must satisfy.
    rtti_type property_type;

    ///The key the module will use when it wants to call the submodule.
    key_type callback_key;

    ///The key of the module to use as the default submodule
    key_type default_module_key;

};

/** @brief Class for detailing a module's submodule needs.
 *
 *  Before a module can be loaded the ModuleManager needs to complete the call
 *  graph for that module. In order for the ModuleManager to fill out the call
 *  graph, it needs to know what types of submodules your module will need. That
 *  is where this class comes in. The SubmoduleRequestForm class implements the
 *  mechanism by which your module requests submodules from the ModuleManager.
 *  At the moment we support two requests:
 *
 *  1. Just the property type the submodule should fulfill
 *
 *     - This request will result in the ModuleManger providing your module
 *       with whatever module has been registered as the default for that
 *       property type
 *     - This is the preferred option as it maximizes interoperability
 *
 *  2. The property type and the key for the default implementation.
 *
 *     - This request will result in the ModuleManager providing your module
 *       with the requested submodule unless the user overrides.
 *     - This option is primarily for situations where your module works best
 *       with a particular submodule, but will still work if another module
 *       (satisfying the same property type) is provided
 *
 *  While not part of the class at present, the SubmoduleRequestForm is also a
 *  logical place for a module to provide details pertaining to how it will use
 *  a submodule. Possible details include "iteratively" and in "parallel".
 *  Eventually such details could be used by the ModuleManager to fine-tune the
 *  module used to satisfy the request.
 *
 *  @note If your module will only work with one particular submodule you should
 *  not list that submodule in the SubmoduleRequestForm. Rather you should
 *  simply call the required submodule. This situation arises when your module
 *  is actually coupled to the internals of the particular submodule and is thus
 *  a form of technical debt. Repaying this technical debt requires moving the
 *  explicit coupling to the submodule's API, possibly creating a new property
 *  type in the process.
 */
class SubmoduleRequestForm {
public:
    /// The type of the elements contained within this container
    using value_type = SubmoduleRequest;

    /// Type of a read-write reference to an element
    using reference = value_type&;

    /// Type of a read-only reference to an element
    using const_reference = const value_type &;

    /// The type used for the various keys
    using key_type = typename SubmoduleRequest::key_type;

    /// The type of a natural number
    using size_type = std::size_t;

    /// The type of a pointer to a pimpl
    using pimpl_pointer = std::unique_ptr<detail_::SubmoduleRequestFormPIMPL>;

    //@{
    /** @name Ctors and assignment operators.
     *
     *  1) Default ctor.
     *
     *  2 and 3) Copy ctor/assignment operator.
     *
     *  4 and 5) Move ctor/assignment operator.
     *
     *  @param rhs The other SubmoduleRequestForm instance to copy/move from.
     *         For move operations, after the operation @p rhs is in a valid,
     *         but otherwise unspecified state.
     *
     * @throw std::bad_alloc Default ctor and copy ctor/assignment operator
     *         throw if there is insufficient memory to complete the operation.
     *         ??? guarantee.
     * @throw None Move ctor/assignment operator are no throw guarantee.
     */
    SubmoduleRequestForm();
    SubmoduleRequestForm(const SubmoduleRequestForm& rhs);
    SubmoduleRequestForm& operator=(const SubmoduleRequestForm& rhs) {
        return *this = std::move(SubmoduleRequestForm(rhs));
    }
    SubmoduleRequestForm(SubmoduleRequestForm&& rhs) noexcept;
    SubmoduleRequestForm& operator=(SubmoduleRequestForm&& rhs) noexcept;
    //@}


    /** @brief Default dtor.
     *
     * @throw None No throw guarantee.
     */
    ~SubmoduleRequestForm() noexcept;

    /** @brief Exchanges the contents of the current instance with the state of
     *         another instance.
     *
     * @param rhs The instance to exchange state with.
     *
     * @throw None No throw guarantee.
     */
    void swap(SubmoduleRequestForm& rhs) noexcept;


    //@{
    /** @name Adding a request.
     *
     *  1. Requests a submodule of type `property_type` be assigned to @p key.
     *
     *  2. Same as 1., but also provides a default, @p default_key, for the
     *     submodule.
     *
     *  3. Takes a fully specified SubmoduleRequest.
     *
     *  @param key is the key the submodule will be assigned to
     *  @tparam property_type is the C++ class defining the property type the
     *  submodule must satisfy.
     *
     *  @throw std::bad_alloc if there is insufficient memory to add the
     *         element. Strong throw guarantee.
     */
    template<typename property_type>
    void push(key_type key) { push<property_type>(std::move(key),""); }
    template<typename property_type>
    void push(key_type key, key_type default_key){
        value_type request{std::move(std::type_index(typeid(property_type))),
                           std::move(key),
                           std::move(default_key)};
        push(std::move(request));
    }
    void push(value_type request);
    //@}

    /** @brief Used to determine if all requests have been addressed or not.
     *
     * @return True if the current instance contains no requests and false
     *         otherwise.
     *
     * @throw None No throw guarantee.
     */
    bool empty()const noexcept;

    /** @brief Computes the number of unaddressed requests in the container.
     *
     *  @return The number of requests in the form.
     *
     *  @throw None No throw guarantee.
     */
    size_type size()const noexcept;

    //@{
    /** @name Request Retrieval.
     *
     *  @brief Functions for retrieving requests from the form.
     *
     * 1 and 2) Retrieve the oldest request still on the form.
     *
     * 3 and 4) Retrieve the newest request still on the form.
     *
     * 2 and 4 are respectively read-only versions of 1 and 2.
     *
     * @warning Calling front/back on an empty container is undefined behavior.
     *
     * @throw None No throw guarantee.
     */
    reference front() noexcept;
    const_reference front() const noexcept {
        return const_cast<SubmoduleRequestForm&>(*this).front();
    }
    reference back() noexcept;
    const_reference back() const noexcept {
        return const_cast<SubmoduleRequestForm&>(*this).back();
    }
    //@}

    /** @brief Removes the oldest request from the form.
     *
     *  After calling this function the oldest element will be deleted.
     *
     *  @warning calling pop on an empty container is undefined behavior.
     *
     *  @throw None No throw guarantee.
     */
    void pop() noexcept;

private:
    /// The instance that actually implements the class
    pimpl_pointer pimpl_;
}; //End class SubmoduleRequestForm

/** @brief Compares two SubmoduleRequest instances for equality.
 *
 * @relates SubmoduleRequest
 *
 * Two SubmoduleRequests are equal if they request a submodule of the same
 * property type, intend to call the submodule the same thing, and default to
 * the same module key.
 *
 *
 * @param lhs The request on the left of the equality operator.
 * @param rhs The request on the right of the equality operator.
 * @return True if the two instances are equal and false otherwise.
 *
 * @throw None No throw guarantee.
 */
bool operator==(const SubmoduleRequest& lhs,
                const SubmoduleRequest& rhs) noexcept{
    return lhs.property_type      == rhs.property_type &&
           lhs.callback_key       == rhs.callback_key  &&
           lhs.default_module_key == rhs.default_module_key;
}

/** @brief Compares two SubmoduleRequest instances for inequality.
 *
 * @relates SubmoduleRequest
 *
 * Two SubmoduleRequests are equal if they request a submodule of the same
 * property type, intend to call the submodule the same thing, and default to
 * the same module key.
 *
 *
 * @param lhs The request on the left of the inequality operator.
 * @param rhs The request on the right of the inequality operator.
 * @return False if the two instances are equal and true otherwise.
 *
 * @throw None No throw guarantee.
 */
bool operator!=(const SubmoduleRequest& lhs,
                const SubmoduleRequest& rhs) noexcept { return !(lhs == rhs); }

/** @brief Allows SubmoduleRequest instances to be printed.
 *
 * @relates SubmoduleRequest
 *
 * @param os The outstream to print to.
 * @param request The SubmoduleRequest to print.
 * @return The outstream containing, now with @p request appended to the stream.
 *
 * @throws ??? Weak throw guarantee.
 */
std::ostream& operator<<(std::ostream& os, const SubmoduleRequest& request) {
    os << "Property type: " << request.property_type.name() <<
          " Callback key: " << request.callback_key;
    if(request.default_module_key != "")
        os << " Default module key: " << request.default_module_key;
    return os;
}

/** @brief Prints a SubmoduleRequestForm to a stream.
 *
 * @relates SubmoduleRequestForm
 *
 * @param os The output stream to print to.
 * @param request The request to print to the stream. Will be copied so that
 *        draining the queue does not interfere with the state of the original
 *        request.
 * @return The output stream, now with @p request appended.
 * @throw ??? Weak throw guarantee.
 */
std::ostream& operator<<(std::ostream& os, SubmoduleRequestForm request) {
    while(!request.empty()){
        os << request.front() << std::endl;
        request.pop();
    }
    return os;
}

} // namespace SDE
