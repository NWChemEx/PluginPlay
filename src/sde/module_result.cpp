#include "detail_/module_result_pimpl.hpp"
#include "sde/module_result.hpp"
#include <typeindex>

namespace sde {

using shared_any = typename ModuleResult::shared_any;
using pimpl_t    = detail_::ModuleResultPIMPL;

ModuleResult::ModuleResult() : pimpl_(std::make_unique<pimpl_t>()) {}

ModuleResult::ModuleResult(const ModuleResult& rhs) :
  pimpl_(rhs.pimpl_->clone()) {}

ModuleResult::ModuleResult(ModuleResult&& rhs) noexcept = default;

ModuleResult& ModuleResult::operator=(ModuleResult&& rhs) noexcept = default;

ModuleResult::~ModuleResult() noexcept = default;

bool ModuleResult::has_type() const noexcept { return pimpl_->has_type(); }

bool ModuleResult::has_value() const noexcept { return pimpl_->has_value(); }

bool ModuleResult::has_description() const noexcept {
    return pimpl_->has_description();
}

ModuleResult& ModuleResult::set_type_(const std::type_info& rtti) {
    pimpl_->set_type(rtti);
    return *this;
}

void ModuleResult::change_(type::any new_value) {
    pimpl_->set_value(std::make_shared<type::any>(std::move(new_value)));
}

void ModuleResult::change_(shared_any new_value) {
    pimpl_->set_value(new_value);
}

ModuleResult& ModuleResult::set_description(type::description desc) noexcept {
    pimpl_->set_description(std::move(desc));
    return *this;
}

type::rtti ModuleResult::type() const { return pimpl_->type(); }

const shared_any& ModuleResult::at_() const { return pimpl_->value(); }

const type::description& ModuleResult::description() const {
    return pimpl_->description();
}

bool ModuleResult::operator==(const ModuleResult& rhs) const {
    return *pimpl_ == *rhs.pimpl_;
}

template<class Archive>
inline void ModuleResult::save(Archive& ar) const {
    pimpl_->save<Archive>(ar);
}

template<class Archive, class Anytype>
inline void ModuleResult::load(Archive& ar) {
    pimpl_->load<Archive, Anytype>(ar);
}

template void ModuleResult::save<cereal::JSONOutputArchive>(
  cereal::JSONOutputArchive&) const;
template void ModuleResult::load<cereal::JSONInputArchive, int>(
  cereal::JSONInputArchive&);
template void ModuleResult::load<cereal::JSONInputArchive, double>(
  cereal::JSONInputArchive&);
template void ModuleResult::save<cereal::BinaryOutputArchive>(
  cereal::BinaryOutputArchive&) const;
template void ModuleResult::load<cereal::BinaryInputArchive, int>(
  cereal::BinaryInputArchive&);
template void ModuleResult::load<cereal::BinaryInputArchive, double>(
  cereal::BinaryInputArchive&);

} // namespace sde
