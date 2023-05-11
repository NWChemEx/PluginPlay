#pragma once
#include <any>
#include <memory>

namespace pluginplay::python {
namespace detail_ {
class PythonWrapperPIMPL;
}

class PythonWrapper {
public:
    using pimpl_type    = detail_::PythonWrapperPIMPL;
    using pimpl_pointer = std::unique_ptr<pimpl_type>;

    using value_type      = std::any;
    using reference       = value_type&;
    using const_reference = const value_type&;

    PythonWrapper() noexcept;
    explicit PythonWrapper(value_type py_value);
    ~PythonWrapper() noexcept;

    bool has_value() const noexcept;

    bool operator==(const PythonWrapper& rhs) const noexcept;
    bool operator!=(const PythonWrapper& rhs) const noexcept;

    reference value();
    const_reference value() const;

private:
    void assert_pimpl_() const;
    bool has_pimpl_() const noexcept;

    pimpl_pointer m_pimpl_;
};

} // namespace pluginplay::python
