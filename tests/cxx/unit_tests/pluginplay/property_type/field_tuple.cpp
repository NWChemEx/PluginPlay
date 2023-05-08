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
#include <pluginplay/fields/fields.hpp>
#include <pluginplay/property_type/field_tuple.hpp>

using namespace pluginplay;
using namespace pluginplay::detail_;

using PropertyTypeInputBuilder =
  pluginplay::FieldTuple<pluginplay::ModuleInput>;

template<typename T>
void check_state(T& builder, std::vector<std::string> corr_keys) {
    REQUIRE(builder.size() == corr_keys.size());
    auto keyi = corr_keys.begin();
    for(auto [k, v] : builder) {
        REQUIRE(k == *keyi++);
        REQUIRE(builder.count(k));
    }
}

/*
 * We know FieldTupleTraits works from its unit test. This unit test ensures
 * that we forward the types correctly through the FieldTuple class. In other
 * words we need to check we obtain the correct traits_type for various template
 * parameter combinations and then for one of those combinations we need to make
 * sure that the types are forwarded correctly.
 */
TEST_CASE("FieldTuple : types") {
    SECTION("FieldTuple<ModuleInput>::traits_type") {
        using traits_type = typename FieldTuple<ModuleInput>::traits_type;
        using corr        = detail_::FieldTupleTraits<ModuleInput>;
        STATIC_REQUIRE(std::is_same_v<traits_type, corr>);
    }

    SECTION("FieldTuple<ModuleInput, int>::traits_type") {
        using traits_type = typename FieldTuple<ModuleInput, int>::traits_type;
        using corr        = detail_::FieldTupleTraits<ModuleInput, int>;
        STATIC_REQUIRE(std::is_same_v<traits_type, corr>);
    }

    SECTION("FieldTuple<ModuleInput, int, const int&>::traits_type") {
        using traits_type =
          typename FieldTuple<ModuleInput, int, const int&>::traits_type;
        using corr = detail_::FieldTupleTraits<ModuleInput, int, const int&>;
        STATIC_REQUIRE(std::is_same_v<traits_type, corr>);
    }

    SECTION("FieldTuple<ModuleResult>::traits_type") {
        using traits_type = typename FieldTuple<ModuleResult>::traits_type;
        using corr        = detail_::FieldTupleTraits<ModuleResult>;
        STATIC_REQUIRE(std::is_same_v<traits_type, corr>);
    }

    SECTION("FieldTuple<ModuleResult, int>::traits_type") {
        using traits_type = typename FieldTuple<ModuleResult, int>::traits_type;
        using corr        = detail_::FieldTupleTraits<ModuleResult, int>;
        STATIC_REQUIRE(std::is_same_v<traits_type, corr>);
    }

    SECTION("FieldTuple<ModuleResult, int, double>::traits_type") {
        using traits_type =
          typename FieldTuple<ModuleResult, int, double>::traits_type;
        using corr = detail_::FieldTupleTraits<ModuleResult, int, double>;
        STATIC_REQUIRE(std::is_same_v<traits_type, corr>);
    }

    SECTION("Type forwarding") {
        using field_tuple = FieldTuple<ModuleInput, int>;
        using traits_type = typename field_tuple::traits_type;

        SECTION("key_type") {
            using key_type = typename field_tuple::key_type;
            using corr     = typename traits_type::key_type;
            STATIC_REQUIRE(std::is_same_v<key_type, corr>);
        }

        SECTION("mapped_type") {
            using mapped_type = typename field_tuple::mapped_type;
            using corr        = typename traits_type::mapped_type;
            STATIC_REQUIRE(std::is_same_v<mapped_type, corr>);
        }

        SECTION("value_type") {
            using value_type = typename field_tuple::value_type;
            using corr       = typename traits_type::value_type;
            STATIC_REQUIRE(std::is_same_v<value_type, corr>);
        }

        SECTION("iterator") {
            using iterator = typename field_tuple::iterator;
            using corr     = typename traits_type::iterator;
            STATIC_REQUIRE(std::is_same_v<iterator, corr>);
        }

        SECTION("const_iterator") {
            using const_iterator = typename field_tuple::const_iterator;
            using corr           = typename traits_type::const_iterator;
            STATIC_REQUIRE(std::is_same_v<const_iterator, corr>);
        }
    }
}

TEST_CASE("FieldTuple : Default ctor") {
    std::vector<std::string> keys;
    PropertyTypeInputBuilder builder;
    check_state(builder, keys);
    using corr_type = std::tuple<>;
    using fields    = typename decltype(builder)::traits_type::tuple_of_fields;
    STATIC_REQUIRE(std::is_same_v<corr_type, fields>);
}

TEST_CASE("FieldTuple : add_field") {
    PropertyTypeInputBuilder builder;
    std::vector<std::string> keys{"key1"};
    auto new_builder = builder.add_field<int>("key1");
    SECTION("Resulting state") {
        check_state(new_builder, keys);
        using corr_type = std::tuple<int>;
        using fields =
          typename decltype(new_builder)::traits_type::tuple_of_fields;
        STATIC_REQUIRE(std::is_same_v<corr_type, fields>);
    }
    SECTION("Can't reuse the same key") {
        REQUIRE_THROWS_AS(new_builder.add_field<int>("key1"),
                          std::invalid_argument);
    }

    SECTION("Can get field") {
        pluginplay::ModuleInput input;
        input.set_type<int>();
        REQUIRE(new_builder.at("key1") == input);
    }

    SECTION("Can add second field") {
        auto newer_builder = new_builder.add_field<double>("key2");
        keys.emplace_back("key2");
        check_state(newer_builder, keys);
    }
}

TEST_CASE("FieldTuple == FieldTuple") {
    FieldTuple<ModuleResult> result0;
    FieldTuple<ModuleInput> input0;

    SECTION("Empty == Empty") {
        REQUIRE(result0 == FieldTuple<ModuleResult>{});
        REQUIRE_FALSE(result0 != FieldTuple<ModuleResult>{});
        REQUIRE(input0 == FieldTuple<ModuleInput>{});
        REQUIRE_FALSE(input0 != FieldTuple<ModuleInput>{});
    }

    SECTION("Empty with different types") {
        REQUIRE_FALSE(result0 == input0);
        REQUIRE(result0 != input0);
    }

    SECTION("One key, no default") {
        auto input1 = input0.add_field<int>("field 1");
        SECTION("Same key, same type, no default") {
            auto other = input0.add_field<int>("field 1");
            REQUIRE(input1 == other);
            REQUIRE_FALSE(input1 != other);
        }

        SECTION("Same key, same type, default") {
            auto other = input0.add_field<int>("field 1", 1);
            REQUIRE_FALSE(input1 == other);
            REQUIRE(input1 != other);
        }

        SECTION("Same key, different type, no default") {
            auto other = input0.add_field<double>("field 1");
            REQUIRE_FALSE(input1 == other);
            REQUIRE(input1 != other);
        }

        SECTION("Different key, same type, no default") {
            auto other = input0.add_field<int>("field 2");
            REQUIRE_FALSE(input1 == other);
            REQUIRE(input1 != other);
        }
    }

    SECTION("One key with default") {
        auto input1 = input0.add_field<int>("field 1", 1);
        SECTION("Same key, same type, same default") {
            auto other = input0.add_field<int>("field 1", 1);
            REQUIRE(input1 == other);
            REQUIRE_FALSE(input1 != other);
        }

        SECTION("Same key, same type, different defaults") {
            auto other = input0.add_field<int>("field 1", 2);
            REQUIRE_FALSE(input1 == other);
            REQUIRE(input1 != other);
        }

        SECTION("Same key, different type, no default") {
            auto other = input0.add_field<double>("field 1", 1.0);
            REQUIRE_FALSE(input1 == other);
            REQUIRE(input1 != other);
        }

        SECTION("Different key, same type, same default") {
            auto other = input0.add_field<int>("field 2", 1);
            REQUIRE_FALSE(input1 == other);
            REQUIRE(input1 != other);
        }
    }

    SECTION("Two keys, no defaults") {
        auto input1 = input0.add_field<int>("field 1");
        auto input2 = input1.add_field<double>("field 2");

        SECTION("Same key, same type, no default") {
            auto other1 = input0.add_field<int>("field 1");
            auto other2 = other1.add_field<double>("field 2");
            REQUIRE(input2 == other2);
            REQUIRE_FALSE(input2 != other2);
        }

        SECTION("Same, but in reverse order") {
            auto other1 = input0.add_field<double>("field 2");
            auto other2 = other1.add_field<int>("field 1");
            REQUIRE_FALSE(input2 == other2);
            REQUIRE(input2 != other2);
        }

        SECTION("Same key, same type, different defaults") {
            SECTION("First has default") {
                auto other1 = input0.add_field<int>("field 1", 1);
                auto other2 = other1.add_field<double>("field 2");
                REQUIRE_FALSE(input2 == other2);
                REQUIRE(input2 != other2);
            }
            SECTION("Second has default") {
                auto other1 = input0.add_field<int>("field 1");
                auto other2 = other1.add_field<double>("field 2", 1.0);
                REQUIRE_FALSE(input2 == other2);
                REQUIRE(input2 != other2);
            }
            SECTION("Both have defaults") {
                auto other1 = input0.add_field<int>("field 1", 1);
                auto other2 = other1.add_field<double>("field 2", 1.0);
                REQUIRE_FALSE(input2 == other2);
                REQUIRE(input2 != other2);
            }
        }

        SECTION("Same key, different type, no defaults") {
            SECTION("First has different type") {
                auto other1 = input0.add_field<double>("field 1");
                auto other2 = other1.add_field<double>("field 2");
                REQUIRE_FALSE(input2 == other2);
                REQUIRE(input2 != other2);
            }
            SECTION("Second has different type") {
                auto other1 = input0.add_field<int>("field 1");
                auto other2 = other1.add_field<int>("field 2");
                REQUIRE_FALSE(input2 == other2);
                REQUIRE(input2 != other2);
            }
        }

        SECTION("Different key, same type, no default") {
            SECTION("First has different key") {
                auto other1 = input0.add_field<int>("field 3");
                auto other2 = input1.add_field<double>("field 2");
                REQUIRE_FALSE(input2 == other2);
                REQUIRE(input2 != other2);
            }
            SECTION("Second has different key") {
                auto other1 = input0.add_field<int>("field 1");
                auto other2 = input1.add_field<double>("field 3");
                REQUIRE_FALSE(input2 == other2);
                REQUIRE(input2 != other2);
            }
        }
    }
}

TEST_CASE("FieldTuple + FieldTuple") {
    FieldTuple<ModuleInput> input0;
    auto input1 = input0.add_field<int>("field 1");
    SECTION("Empty + Empty") {
        auto rv = input0 + input0;
        REQUIRE(rv == input0);
    }

    SECTION("Empty + non-empty") {
        auto rv = input0 + input1;
        REQUIRE(rv == input1);
    }

    SECTION("Non-empty + empty") {
        auto rv = input1 + input0;
        REQUIRE(rv == input1);
    }

    SECTION("Both non-empty") {
        auto input2 = input0.add_field<double>("field 2");

        SECTION("input1 + input2") {
            auto rv   = input1 + input2;
            auto corr = input1.add_field<double>("field 2");
            REQUIRE(rv == corr);
        }

        SECTION("input2 + input1") {
            auto rv   = input2 + input1;
            auto corr = input2.add_field<int>("field 1");
            REQUIRE(rv == corr);
        }
    }
}
