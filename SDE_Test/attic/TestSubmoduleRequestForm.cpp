#include <SDE/attic/SubmoduleRequestForm.hpp>
#include <catch2/catch.hpp>
#include <queue>

using namespace SDE;

static void check_state(SubmoduleRequestForm& requests,
                        std::queue<SubmoduleRequest> corr ) {
    REQUIRE(requests.size() == corr.size());
    REQUIRE(requests.empty() == corr.empty());

    const SubmoduleRequestForm& const_requests = requests;

    while(!requests.empty()){
        REQUIRE(requests.front() == corr.front());
        REQUIRE(const_requests.front() == corr.front());
        REQUIRE(requests.back() == corr.back());
        REQUIRE(const_requests.back() == corr.back());
        REQUIRE(requests.size() == corr.size());
        REQUIRE(requests.empty() == corr.empty());
        requests.pop();corr.pop();
    }

}

TEST_CASE("SubmoduleRequestForm") {
    SubmoduleRequestForm requests;

    SECTION("Default Ctor") {
        check_state(requests, std::queue<SubmoduleRequest>{});
    }

    SubmoduleRequest r1{std::type_index(typeid(double)), "key1", "key2"};
    SubmoduleRequest r2{std::type_index(typeid(int)), "key3", ""};
    requests.push<double>("key1", "key2");
    requests.push(r1);
    requests.push<int>("key3");

    std::queue<SubmoduleRequest> corr;
    corr.push(r1);
    corr.push(r1);
    corr.push(r2);

    SECTION("Filled Form"){
        check_state(requests, corr);
    }

    SECTION("Copy Ctor"){
        SubmoduleRequestForm copy(requests);
        check_state(copy, corr);
    }

    SECTION("Move Ctor"){
        SubmoduleRequestForm moved(std::move(requests));
        check_state(moved, corr);
    }

    SECTION("Copy Assignment"){
        SubmoduleRequestForm copy;
        auto& pcopy = (copy = requests);
        check_state(copy, corr);
        REQUIRE(&pcopy == &copy);
    }

    SECTION("Move Assignment"){
        SubmoduleRequestForm moved;
        auto& pmoved = (moved = std::move(requests));
        check_state(moved, corr);
        REQUIRE(&pmoved == &moved);
    }

    SECTION("Swap"){
        SubmoduleRequestForm empty;
        empty.swap(requests);
        check_state(requests, std::queue<SubmoduleRequest>{});
        check_state(empty, corr);
    }
}
