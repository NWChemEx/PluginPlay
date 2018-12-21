#pragma once

namespace SDE {

///Wrapper for a description to be used if the value is also of type std::string
struct Description {
    using description_type = std::string;
    description_type value;
};



struct Trans


class ModuleBase {
public:
    using key_type = std::string;
    using description_type = typename Description::description_type;

    using result_type = OutputResults;
    using input_type = InputParameters;
    using submodule_list = std::map<std::string, Module>;

    auto run(const input_type& ps, const submodule_list& submods) {
        return run_(ps, submods_);
    }

protected:
    template<typename property_type>
    void satisfies_property_type(){
        property_type p;
        for(auto [k,v] : p.inputs()) add_input_(k, v);
        for(auto [k,v] : p.results()) add_result_(k,v);
    }

    template<typename T>
    ModuleIOOptions add_input(key_type key) {
        return ModuleIOOptions(iparams_[key]).type<T>();
    }

    template<typename property_type>
    void add_submodule(key_type key) {

    }


private:
    virtual result_type run_(const input_type& ps,
                             const submodule_list& submods) = 0;

    void add_input_()

    input_type iparams_;
    result_type oparams_;
};

} //End namespace
