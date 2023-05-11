#include "detail_/python_wrapper_pimpl.hpp"

namespace pluginplay::python {

PythonWrapper::PythonWrapper() noexcept = default;

PythonWrapper::PythonWrapper(std::any py_value) :
  m_pimpl_(std::make_unique<pimpl_type>(std::move(py_value))) {}

PythonWrapper::~PythonWrapper() noexcept = default;

bool PythonWrapper::has_value() const noexcept {
    return has_pimpl_() && m_pimpl_->has_value();
}

bool PythonWrapper::operator==(const PythonWrapper& rhs) const noexcept {
    if(has_value() != rhs.has_value()) return false;
    if(!has_value()) return true;
    return (*m_pimpl_) == (*rhs.m_pimpl_);
}

bool PythonWrapper::operator!=(const PythonWrapper& rhs) const noexcept {
    return !(*this == rhs);
}

PythonWrapper::reference PythonWrapper::value() {
    assert_pimpl_();
    return m_pimpl_->py_object;
}

PythonWrapper::const_reference PythonWrapper::value() const {
    assert_pimpl_();
    return m_pimpl_->py_object;
}

void PythonWrapper::assert_pimpl_() const {
    if(has_pimpl_()) return;
    throw std::runtime_error("Does not have a PIMPL");
}

bool PythonWrapper::has_pimpl_() const noexcept {
    return static_cast<bool>(m_pimpl_);
}

} // namespace pluginplay::python
