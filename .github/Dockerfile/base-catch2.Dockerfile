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

ARG PARENT_IMAGE_NAME

FROM ${PARENT_IMAGE_NAME}:latest

ARG CATCH2_VERSION

# Install catch2 ##
RUN cd /tmp \
    && git clone -b v${CATCH2_VERSION} https://github.com/catchorg/Catch2.git \
    && cd Catch2 \
    && cmake -Bbuild -H. -DBUILD_TESTING=OFF -DCMAKE_INSTALL_PREFIX=/install \
    && cmake --build build \
    && cmake --build build --target install \
    && rm -rf /tmp/Catch2
