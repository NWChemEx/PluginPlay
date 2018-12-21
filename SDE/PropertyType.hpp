#pragma once

namespace SDE {

class PropertyTypeBase {
public:

};


/**
 *
 * @note Since the order of the inputs/outputs maps to the order of the run
 *       function we can't use the actual user-facing classes,
 */
template<typename derived_type>
class PropertyType {
private:
    ///Typedef used internally to shorten this class's type
    using my_type = PropertyType<derived_type>;

public:
    ///
    using key_type = std::string;

    using desc_type = std::string;

    using input_type = Option;

    using result_type = Property;

    virtual ~PropertyType() = default;

    ///The type returned by
    using input_return = std::vector<std::pair<std::string, input_type>>;
    using result_return = std::vector<std::pair<std::string, result_type>>;


    input_return inputs() { return inputs_; }
    result_return results() { return results_; }

    auto type_() const { return std::type_index(typeid(derived_type)); }

protected:
    //@{
    /** @name Ctors and Assignment Operators
     *
     *  The PropertyType class is designed to be inherited from and to be passed
     *  around purely in a non-polymorphic manner. To avoid polymorphic use all
     *  constructors are protected, thereby preventing users from calling them
     *  from outside the class. The derived class should feel free to expose
     *  its ctors (and is required to minimally expose the default ctor) to the
     *  outside world.
     *
     */
    PropertyType() = default;
    PropertyType(const my_type&) = default;
    my_type& operator=(const my_type&) = default;
    PropertyType(my_type&&) = default;
    my_type& operator=(my_type&&) = default;
    //@}

    template<typename T>
    void add_input(key_type key, desc_type desc) {
        input_type input(T{}, std::move(desc));
        auto elem = std::make_pair(std::move(key), std::move(input));
        inputs_.push_back(std::move(elem));
    }

    template<typename T>
    void add_result(key_type key, desc_type desc) {
        result_type result(T{}, std::move(desc));
        auto elem = std::make_pair(std::move(key), std::move(result));
        results_.push_back(std::move(elem));
    }

private:
    auto& downcast_() const {return static_cast<const derived_type&>(*this);}

    input_return inputs_;
    result_return results_;


}; //End class PropertyType

} //End namespace
