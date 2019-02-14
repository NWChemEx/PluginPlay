#include "examples/TestModuleBase.hpp"
#include <SDE/Module.hpp>
#include <SDE/SubmoduleRequest.hpp>
#include <SDE/detail_/ModulePIMPL.hpp>
#include <catch2/catch.hpp>

using namespace SDE;
using namespace SDE::detail_;

inline std::shared_ptr<Module> make_module() {
    auto pimpl = std::make_unique<ModulePIMPL>(std::make_shared<Rectangle>());
    return std::make_shared<Module>(std::move(pimpl));
}

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
        request1.set_type<double>();
        REQUIRE(request1 != request2);
        REQUIRE(!(request1 == request2));
    }
    SECTION("Different modules") {
        request1.set_type<Area>();
        request2.set_type<Area>();
        request1.change(make_module());
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

TEST_CASE("SubmoduleRequest : change") {
    SubmoduleRequest r;
    r.set_type<Area>();
    auto ptr  = make_module();
    auto ptr2 = make_module();
    r.change(ptr);
    REQUIRE(r.value() == *ptr2);
}

TEST_CASE("SubmoduleRequest : set_type") {
    SubmoduleRequest r;
    r.set_type<Area>();
    SECTION("Type already set") {
        REQUIRE_THROWS_AS(r.set_type<Area>(), std::runtime_error);
    }
}

TEST_CASE("SubmoduleRequest : ready") {
    SubmoduleRequest r;
    SECTION("No module") { REQUIRE(!r.ready()); }
    SECTION("Ready module") {
        r.set_type<Area>();
        r.change(make_module());
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
        r.change(make_module());
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
        r.change(make_module());
        r.hash(h);
        auto hv = bphash::hash_to_string(h.finalize());
        REQUIRE(hv == "1b740793eacb5cdf38dc426f626ab5ab");
    }
}
