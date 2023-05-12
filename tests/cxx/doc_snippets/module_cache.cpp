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

#include <cmath>
#include <pluginplay/pluginplay.hpp>

namespace pluginplay_examples {

using pair_type = std::pair<float, float>;

DECLARE_PROPERTY_TYPE(RootFinder);

PROPERTY_TYPE_INPUTS(RootFinder) {
    auto rv = pluginplay::declare_input()
                .add_field<pair_type>("Initial Interval")
                .template add_field<pair_type>("Current Interval");
    return rv;
}

PROPERTY_TYPE_RESULTS(RootFinder) {
    return pluginplay::declare_result().add_field<pair_type>("New interval");
}

DECLARE_MODULE(CosBisection);

MODULE_CTOR(CosBisection) {
    description("Computes a root of cosine by bisecting an interval.");
    satisfies_property_type<RootFinder>();
}

MODULE_RUN(CosBisection) {
    // Get 0-th and n-th inputs
    const auto& [i0, in] = RootFinder::unwrap_inputs(inputs);

    // Get the cache
    auto& my_cache = get_cache();

    // If i0 == in we look for a cached value, otherwise we use in
    auto [a, b] = in;
    if(i0 == in && my_cache.count(i0)) {
        std::tie(a, b) = my_cache.uncache<pair_type>(i0);
    }

    // Compute value of cos on interval's enpoints
    auto fa = std::cos(a);
    auto fb = std::cos(b);

    // Make sure one of the endpoints is negative
    if((fa < 0.0 && fb < 0.0) || (fa > 0.0 && fb > 0.0))
        throw std::runtime_error("Must have opposite signs on endpoints");

    // Compute midpoint of interval and the value of cos at that point
    auto c  = (a + b) / 2.0;
    auto fc = std::cos(c);

    // The new interval is c and either a or b, whichever has the opposite sign
    // of c
    pair_type new_interval;
    if(fc < 0.0) { // fc is negative, keep old positive endpoint
        new_interval.first  = c;
        new_interval.second = (fa < 0.0) ? b : a;
    } else { // fc is positive (or zero), keep old negative endpoint
        new_interval.first  = (fa < 0.0) ? a : b;
        new_interval.second = c;
    }

    // Store our newest interval and return it
    my_cache.cache(i0, new_interval);

    auto rv = results();
    return RootFinder::wrap_results(rv, new_interval);
}

} // namespace pluginplay_examples
