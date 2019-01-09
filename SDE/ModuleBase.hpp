#pragma once
#include "SDE/ModuleInput.hpp"
#include "SDE/ModuleOutput.hpp"
#include "SDE/SubmoduleRequest.hpp"
#include <Utilities/Containers/CaseInsensitiveMap.hpp>

namespace SDE {

class ModuleBase {
public:
    using input_type     = ModuleInput;
    using input_map      = Utilities::CaseInsensitiveMap<input_type>;
    using output_type    = ModuleOutput;
    using output_map     = Utilities::CaseInsensitiveMap<output_type>;
    using submod_request = SubmoduleRequest;
    using submod_map     = Utilities::CaseInsensitiveMap<submod_request>;
    using hash_type      = std::string;

    //@{
    /** @name Ctors and Assignment Operators
     *
     *  ModuleBase is meant to be inherited from. In order to avoid the user
     *  having to perform polymorphic copies/moves correctly, we simply disable
     *  copying/moving ModuleBase objects. While this may sound extreme, note
     *  that ModuleBase objects are simply PIMPLs in Module objects. The Module
     *  class can be safely copied and moved thanks to it only holding a pointer
     *  to the ModuleBase object.
     */
    ModuleBase()                      = default;
    ModuleBase(const ModuleBase& rhs) = delete;
    ModuleBase& operator=(const ModuleBase& rhs) = delete;
    ModuleBase(ModuleBase&& rhs)                 = delete;
    ModuleBase& operator=(ModuleBase&& rhs) = delete;
    //@}
    virtual ~ModuleBase() = default;

    output_map run(input_map inputs, submod_map submods) const {
        return run_(std::move(inputs), std::move(submods));
    }

    void memoize(bphash::Hasher& h, const input_map& inputs,
                 const submod_map& submods) const {
        for(const auto & [k, v] : inputs) v.hash(h);
        for(const auto & [k, v] : submods) v.hash(h);
    }

    const output_map& outputs() const { return outputs_; }
    output_map& outputs() { return outputs_; }
    const input_map& inputs() const { return inputs_; }
    input_map& inputs() { return inputs_; }
    const submod_map& submods() const { return submods_; }
    submod_map& submods() { return submods_; }

protected:
    using description_type = typename ModuleInput::description_type;
    using key_type         = typename input_map::key_type;

    void description(description_type desc) { desc_ = std::move(desc); }

    void reference(description_type new_ref) {
        references_.push_back(std::move(new_ref));
    }

    template<typename T>
    auto add_input(key_type key) {
        return inputs_[key].set_type<T>();
    }

    template<typename T>
    auto add_output(key_type key) {
        return outputs_[key].set_type<T>();
    }

    template<typename T>
    void add_submodule(key_type key) {
        return submods_[key].set_type<T>();
    }

    template<typename property_type>
    void satisfies_property_type() {
        property_type p;
        auto inputs = p.inputs();
        inputs_.insert(inputs.begin(), inputs.end());
        auto outputs = p.outputs();
        outputs_.insert(outputs.begin(), outputs.end());
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
    std::vector<description_type> references_;
    //@}

    virtual output_map run_(input_map inputs, submod_map submods) const = 0;
};

} // namespace SDE
