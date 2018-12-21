#pragma once

namespace SDE {
namespace detail_ {
class ModulePIMPL;
}

class Module {
public:
    using result_type = OutputResults;
    using input_type = InputParameters;

    Module();
    Module(const Module& rhs);
    Module& operator=(const Module& rhs);
    Module(Module&& rhs) noexcept;
    Module& operator=(Module&& rhs) noexcept;
    ~Module()noexcept;

    Module(std::unique_ptr<ModulePIMPL> pimpl) : pimpl_(std::move(pimpl)) {}

    template<typename property_type>
    bool can_be_run_as() const {
        property_type p;
        return can_be_run_as_(p.type(), p.inputs(), p.outputs());
    }

    template<typename property_type, typename...Args>
    auto run_as(Args&&...args) {
        property_type p;
        auto inputs = p.inputs();
        auto outputs = p.outputs();

        return p.run();
    }

    auto run(const input_type& ps);
private:
    using
    bool can_be_run_as_(inputs, outputs);

    std::unique_ptr<ModulePIMPL> pimpl_;
};

} //End namespace
