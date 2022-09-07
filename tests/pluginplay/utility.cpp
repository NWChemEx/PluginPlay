/*
 * Copyright 2022 NWChemEx-Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <catch2/catch.hpp>
#include <memory>
#include <pluginplay/utility.hpp>

using namespace pluginplay;

TEST_CASE("IsCString") {
    STATIC_REQUIRE_FALSE(detail_::IsCString<std::string>::value);
    STATIC_REQUIRE_FALSE(detail_::IsCString<int>::value);
    STATIC_REQUIRE(detail_::IsCString<decltype("a string literal")>::value);
}

TEST_CASE("IsConstRef") {
    STATIC_REQUIRE_FALSE(detail_::IsConstRef<int>::value);
    STATIC_REQUIRE_FALSE(detail_::IsConstRef<int&>::value);
    STATIC_REQUIRE_FALSE(detail_::IsConstRef<const int>::value);
    STATIC_REQUIRE(detail_::IsConstRef<const int&>::value);
}

TEST_CASE("IsSharedPtr") {
    STATIC_REQUIRE_FALSE(detail_::IsSharedPtr<int>::value);
    STATIC_REQUIRE_FALSE(detail_::IsSharedPtr<int*>::value);
    STATIC_REQUIRE(detail_::IsSharedPtr<std::shared_ptr<int>>::value);
}

TEST_CASE("Reference wrapper equality") {
    int ione = 1, itwo = 2;
    double done = 1.0, dtwo = 2.0;
    REQUIRE(detail_::operator==(std::cref(ione), std::cref(ione)));
    REQUIRE(!detail_::operator==(std::cref(ione), std::cref(itwo)));
    REQUIRE(detail_::operator==(std::cref(ione), std::cref(done)));
    REQUIRE(!detail_::operator==(std::cref(ione), std::cref(dtwo)));
}
