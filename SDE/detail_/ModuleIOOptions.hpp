#include "SDE/ModuleIO.hpp"

namespace SDE {
namespace detail_ {

class ModuleIOOptions {
public:
    using description_type = typename ModuleIO::description_type;
    template<typename T>
    using value_checker = typename ModuleIO::value_checker<T>;

    //@{
    /**
     * 1) Default ctor
     * 2 and 3) Copy ctor/assignment operator
     * 4 and 5) Move ctor/assignment operator
     * 6) 
     * 
     * With the exception of 2 and 6 all ctor/assignment operators are deleted
     * to avoid unintentional usage.
     */
    ModuleIOOptions() = delete;

    ModuleIOOptions(const ModuleIOOptions &) = default;

    ModuleIOOptions &operator=(const ModuleIOOptions &) = delete;

    ModuleIOOptions(ModuleIOOptions &&) = delete;

    ModuleIOOptions &operator=(ModuleIOOptions &&) = delete;

    ModuleIOOptions(ModuleIO &parent) : parent_(&parent) {}
    //@}

    virtual ~ModuleIOOptions() = default;

    template<typename T>
    ModuleIOOptions& type() {
        add_type_check_ < std::decay_t < T >> ();
        return *this;
    }

    ModuleIOOptions& description(description_type desc) {
        parent_->desc = std::move(desc);
        return *this;
    }

    template<typename T>
    ModuleIOOptions& check(value_checker<T> new_check,
                           description_type desc = ""){
        parent_->add_check<T>(new_check);
        return *this;
    }

    template<typename T>
    ModuleIOOptions& default_value(T&& value) {
        using clean_T = std::decay_t<T>;
        add_type_check_<clean_T>();
        parent_->change(std::forward<T>(value));
        return *this;
    }

private:
    template<typename T>
    void add_type_check_() {
        //Cast in wrapper lambda will fail if the object isn't of type T
        value_checker<T> check = [](const T& ) { return true; };
        parent_->add_check(check, "Type Check");
    }
    ModuleIO* parent_;
};

class ModuleOutputOptions : private ModuleIOOptions {
public:
    ModuleOutputOptions(ModuleOutput& parent) :
        ModuleIOOptions(parent), my_parent_(&parent) {}
    ModuleOutputOptions(const ModuleOutputOptions& rhs) = default;

    using ModuleIOOptions::type;
    using ModuleIOOptions::description;
    using ModuleIOOptions::check;
    using ModuleIOOptions::default_value;

private:
    ModuleOutput* my_parent_;
};

class ModuleInputOptions : private ModuleIOOptions {
public:
    ModuleInputOptions(ModuleInput& parent) :
        ModuleIOOptions(parent), my_parent_(&parent) {}

    ModuleInputOptions(const ModuleInputOptions& rhs) = default;
        
    using ModuleIOOptions::type;
    using ModuleIOOptions::description;
    using ModuleIOOptions::check;
    using ModuleIOOptions::default_value;        

    ModuleInputOptions& transparent(){
        my_parent_->is_transparent = true;
        return *this;
    }

    ModuleInputOptions& opaque(){
        my_parent_->is_transparent = false;
        return *this;
    }

    ModuleInputOptions& optional(){
        my_parent_->is_optional = true;
        return *this;
    }

    ModuleInputOptions& required(){
        my_parent_->is_optional = false;
        return *this;
    }
private:
    ModuleInput* my_parent_;
};
} // namespace detail_
} // namespace SDE
