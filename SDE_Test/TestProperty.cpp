#include <SDE/Property.hpp>
#include <catch2/catch.hpp>

using namespace SDE;
using const_pointer = typename Property::const_pointer;

template<typename T>
static void check_state(Property& prop, const_pointer ptr) {
    const bool is_managing = static_cast<bool>(prop);
    const bool should_be_managing = static_cast<bool>(ptr);
    REQUIRE(is_managing == should_be_managing);
    if constexpr (!std::is_same_v<T, nullptr_t>)
        REQUIRE(prop.value<T>() == detail_::SDEAnyCast<T>(*ptr));

}

TEST_CASE("Property Class") {
    Property prop;

    SECTION("Default Ctor") {
        check_state<nullptr_t>(prop, const_pointer{});
    }

    auto ptr = std::make_shared<const detail_::SDEAny>(detail_::make_SDEAny<int>
        (3));

    SECTION("Adding a value") {
        SECTION("Via shared_ptr") {
            prop.put(ptr);
            check_state<int>(prop, ptr);
            REQUIRE(prop.get() == ptr);
        }
        SECTION("Just the value") {
            prop.put(int{3});
            check_state<int>(prop, ptr);
        }
    }

    prop.put(ptr);

    SECTION("Copy Ctor") {
        Property copy(prop);
        check_state<int>(copy, ptr);
        SECTION("Is shallow") {
            REQUIRE(copy.get().get() == prop.get().get());
        }
    }

    SECTION("Copy Assignment") {
        Property copy;
        auto& pcopy = (copy = prop);
        check_state<int>(copy, ptr);
        SECTION("Supports chaining") {
            REQUIRE(&pcopy == &copy);
        }
        SECTION("Is shallow") {
            REQUIRE(copy.get().get() == prop.get().get());
        }
    }

    SECTION("Move Ctor") {
        Property move(std::move(prop));
        check_state<int>(move, ptr);
    }

    SECTION("Move Assignment") {
        Property move;
        auto& pmove = (move = std::move(prop));
        check_state<int>(move, ptr);
        SECTION("Supports chaining") {
            REQUIRE(&pmove == &move);
        }
    }
}
