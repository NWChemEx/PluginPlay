#pragma once
#include <cereal/archives/json.hpp>
#include <cereal/cereal.hpp>
#include <cereal/types/array.hpp>
#include <cereal/types/chrono.hpp>
#include <cereal/types/complex.hpp>
#include <cereal/types/deque.hpp>
#include <cereal/types/functional.hpp>
#include <cereal/types/list.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/queue.hpp>
#include <cereal/types/set.hpp>
#include <cereal/types/stack.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/tuple.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/unordered_set.hpp>
#include <cereal/types/valarray.hpp>
#include <cereal/types/vector.hpp>
#include <variant>

namespace SDE {
namespace detail_ {

class Serializer {
public:
    Serializer()                      = default;
    Serializer(const Serializer& rhs) = default;
    Serializer& operator=(const Serializer& rhs) = default;
    Serializer(Serializer&& rhs)                 = default;
    Serializer& operator=(Serializer&& rhs) = default;
    Serializer(std::ostream& os) : output_(os) {}
    Serializer(std::istream& is) : input_(is) {}
    ~Serializer() = default;

    template<typename T>
    Serializer& operator<<(T&& value) {
        serialize_(std::forward<T>(value), input_);
        return *this;
    }

    template<typename T>
    Serializer& operator>>(T&& value) {
        serialize_(std::forward<T>(value), output_);
        return *this;
    }

private:
    template<typename T, typename Archive>
    void serialize_(T&& value, Archive& ar) {
        std::visit(
          [&](auto&& arg) {
              using clean_t = std::decay_t<decltype(arg)>;
              if constexpr(std::is_same_v<clean_t, std::monostate>) {
                  throw std::runtime_error("Set archive first!!!");
              } else
                  arg(std::forward<T>(value));
          },
          ar);
    }
    std::variant<std::monostate, cereal::JSONInputArchive> input_;
    std::variant<std::monostate, cereal::JSONOutputArchive> output_;
};

} // namespace detail_
} // namespace SDE
