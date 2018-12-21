#include "SubmoduleRequestForm.hpp"
#include <queue>

namespace SDE {

//Grab some types from the class
using SRF = SubmoduleRequestForm;
using value_type = typename SRF::value_type;
using reference = typename SRF::reference;
using key_type = typename SRF::key_type;
using size_type = typename SRF::size_type;

namespace detail_ {
/* Not sure if there's a need for other PIMPLs, if there are then this class
 * needs turned into an abstract base class and PIMPLs should derive from the
 * abstract base class.
 */
class SubmoduleRequestFormPIMPL {
public:
    std::queue<value_type> requests;
};

} // namespace detail_

SRF::SubmoduleRequestForm() :
    pimpl_(std::make_unique<detail_::SubmoduleRequestFormPIMPL>()) {}

SRF::SubmoduleRequestForm(const SRF& rhs) :
    pimpl_(std::make_unique<detail_::SubmoduleRequestFormPIMPL>(*rhs.pimpl_)) {}

SRF::SubmoduleRequestForm(SRF&& rhs) noexcept = default;
SRF& SRF::operator=(SRF&& rhs) noexcept = default;

SRF::~SubmoduleRequestForm() = default;

void SRF::swap(SRF& rhs) noexcept { pimpl_.swap(rhs.pimpl_); }

void SRF::push(value_type request) {
    pimpl_->requests.emplace(std::move(request));
}

bool SRF::empty() const noexcept { return pimpl_->requests.empty(); }

size_type SRF::size() const noexcept { return pimpl_->requests.size(); }

reference SRF::front() noexcept { return pimpl_->requests.front(); }
reference SRF::back() noexcept {return pimpl_->requests.back(); }

void SRF::pop() noexcept { pimpl_->requests.pop(); }

} //namespace SDE
