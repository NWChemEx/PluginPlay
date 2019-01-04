#pragma once
#include "SDE/detail_/ModuleInputBuilder.hpp"
#include "SDE/detail_/ModuleOutputBuilder.hpp"
#include "SDE/

namespace SDE {

class ModuleBase {
public:
    using input_type     = typename detail_::ModuleInputBuilder::input_type;
    using input_map      = Utilities::CaseInsensitiveMap<input_type>;
    using output_type    = typename detail_::ModuleOutputBuilder::output_type;
    using output_map     = Utilities::CaseInsensitiveMap<output_type>;
    using submod_request = SubmoduleRequest;
    using submod_map     = Utilities::CaseInsensitiveMap<submod_request>;

    output_map run(input_map inputs, submod_map submods) {
        // Hash inputs and submods
        // Check cache, return if present

        auto output = run_(std::move(inputs), std::move(submods));

        // Cache output, return cached output
        return output;
    }

    output_map outputs const { return outputs_; }

protected:
    void description(description_type desc) { desc_ = std::move(desc); }

    void reference(description_type new_ref) {
        references_.push_back(std::move(new_ref));
    }

    template<typename T>
    auto add_input(key_type key) {
        detail_::ModuleInputBuilder temp(inputs_[key]);
        return temp.type<T>();
    }

    template<typename T>
    auto add_output(key_type key) {
        detail_::ModuleOutputBuilder temp(outputs_[key]);
        return temp.type<T>();
    }

    template<typename property_type>
    void add_submodule(key_type key, key_type desc = "") {
        submod_request temp{std::move(desc),
                            std::type_index(typeid(property_type))};
        submods_.emplace(std::move(key), std::move(temp));
    }

    template<typename property_type>
    void satisfies_property_type() {
        property_type p;
        for(auto&& input : p.inputs()) inputs_.insert(std::move(input));
        for(auto&& output : p.outputs()) outputs_.insert(std::move(output));
    }

private:
    //@{
    input_map inputs_;
    output_map outputs_;
    submod_map submods_;
    //@}

    //@{
    /** @name Module metadata
     *
     */
    description_type desc_;
    std::vector<description_list> references_;
    //@}

    virtual output_map run_(input_map inputs, submod_map submods) = 0;
};

} // namespace SDE
