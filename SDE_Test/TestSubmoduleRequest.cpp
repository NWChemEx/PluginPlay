#include "examples/TestModuleBase.hpp"
#include <SDE/SubmoduleRequest.hpp>
#include <SDE/detail_/ModulePIMPL.hpp>
#include <catch2/catch.hpp>

using namespace SDE;
using namespace SDE::detail_;

struct PropType1 {
    static auto inputs() { return SDE::type::input_map{}; }
};

TEST_CASE("SubmoduleRequest : default ctor") {
    SubmoduleRequest request;
    SECTION("State") {
        REQUIRE(request.description() == "");
        REQUIRE(!request.ready());
    }
}

TEST_CASE("SubmoduleRequest : comparisons") {
    SubmoduleRequest request1, request2;
    SECTION("Default state") {
        REQUIRE(request1 == request2);
        REQUIRE(!(request1 != request2));
    }
    SECTION("Different descriptions") {
        request1.set_description("hello world");
        REQUIRE(request1 != request2);
        REQUIRE(!(request1 == request2));
    }
    SECTION("Different property types") {
        request1.set_type<PropType1>();
        REQUIRE(request1 != request2);
        REQUIRE(!(request1 == request2));
    }
    SECTION("Different modules") {
        request1.set_type<Area>();
        request2.set_type<Area>();
        auto pimpl =
          std::make_unique<ModulePIMPL>(std::make_shared<Rectangle>());
        request1.change(std::make_shared<Module>(std::move(pimpl)));
        REQUIRE(request1 != request2);
        REQUIRE(!(request1 == request2));
    }
}

TEST_CASE("SubmoduleRequest : set_description") {
    SubmoduleRequest request1;
    auto* ptr = &(request1.set_description("hello world"));
    REQUIRE(request1.description() == "hello world");
    REQUIRE(ptr == &request1);
}

TEST_CASE("SubmoduleRequest : set_type") {
    SubmoduleRequest r;
    r.set_type<Area>();
    SECTION("Type already set") {
        REQUIRE_THROWS_AS(r.set_type<Area>(), std::runtime_error);
    }
}

TEST_CASE("SubmoduleRequest : change") {
    SubmoduleRequest r;
    r.set_type<Area>();
    auto pimpl  = std::make_unique<ModulePIMPL>(std::make_shared<Rectangle>());
    auto ptr    = std::make_shared<Module>(std::move(pimpl));
    auto pimpl2 = std::make_unique<ModulePIMPL>(std::make_shared<Rectangle>());
    auto ptr2   = std::make_shared<Module>(std::move(pimpl2));
    r.change(ptr);
    REQUIRE(r.value() == *ptr2);
}

TEST_CASE("SubmoduleRequest : ready") {
    SubmoduleRequest r;
    SECTION("No type") { REQUIRE(!r.ready()); }
    SECTION("No module") {
        r.set_type<PrismVolume>();
        REQUIRE(!r.ready());
    }
    SECTION("Not ready module") {
        r.set_type<PrismVolume>();
        auto pimpl = std::make_unique<ModulePIMPL>(std::make_shared<Prism>());
        r.change(std::make_shared<Module>(std::move(pimpl)));
        REQUIRE(!r.ready());
    }
    SECTION("Ready") {
        r.set_type<Area>();
        auto pimpl =
          std::make_unique<ModulePIMPL>(std::make_shared<Rectangle>());
        r.change(std::make_shared<Module>(std::move(pimpl)));
        REQUIRE(r.ready());
    }
}

TEST_CASE("SubmoduleRequest : lock") {
    SubmoduleRequest r;
    SECTION("Can't lock if not ready") {
        REQUIRE_THROWS_AS(r.lock(), std::runtime_error);
    }
    SECTION("Can lock if ready") {
        r.set_type<Area>();
        auto pimpl =
          std::make_unique<ModulePIMPL>(std::make_shared<Rectangle>());
        r.change(std::make_shared<Module>(std::move(pimpl)));
        r.lock();
        REQUIRE(r.value().locked());
    }
}

TEST_CASE("SubmoduleRequest : hash") {
    SubmoduleRequest r;
    type::hasher h(bphash::HashType::Hash128);
    SECTION("No module") {
        r.hash(h);
        auto hv = bphash::hash_to_string(h.finalize());
        REQUIRE(hv == "00000000000000000000000000000000");
    }
    SECTION("Module") {
        r.set_type<Area>();
        auto pimpl =
          std::make_unique<ModulePIMPL>(std::make_shared<Rectangle>());
        r.change(std::make_shared<Module>(std::move(pimpl)));
        r.hash(h);
        auto hv = bphash::hash_to_string(h.finalize());
        REQUIRE(hv == "1b740793eacb5cdf38dc426f626ab5ab");
    }
}
