# Copyright 2023 NWChemEx-Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
ARG dep_repo_tag

FROM ghcr.io/nwchemex-project/release_parallelzone:$dep_repo_tag
FROM ghcr.io/nwchemex-project/release_utilities:$dep_repo_tag

ARG base_tag

FROM ghcr.io/nwchemex-project/base_pluginplay:$base_tag

COPY --from=0 /install /install

COPY --from=1 /install /install

COPY build_test.sh /build_test.sh

RUN ["chmod", "+x", "/build_test.sh"]

ARG cmake_version
ARG gcc_version
ARG clang_version
ARG ninja_build
ARG use_clang
ARG cmaize_github_token
ARG install
ARG unit_test
ARG int_test
ARG branch_name


ENV env_cmake_version=$cmake_version
ENV env_gcc_version=$gcc_version
ENV env_clang_version=$clang_version
ENV env_ninja_build=$ninja_build
ENV env_use_clang=$use_clang
ENV env_cmaize_github_token=$cmaize_github_token
ENV env_install=$install
ENV env_unit_test=$unit_test
ENV env_int_test=$int_test
ENV env_branch_name=$branch_name

ENTRYPOINT ["/build_test.sh"]
