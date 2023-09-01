ARG PARENT_IMAGE_NAME

FROM ${PARENT_IMAGE_NAME}:latest

ARG LIBFORT_VERSION

# Install curl to download libfort
RUN apt-get update \
    && DEBIAN_FRONTEND=noninteractive apt-get install -y curl \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# Install libfort#
ENV INSTALL_PATH=../install

RUN curl -L -o libfort-${LIBFORT_VERSION}.tar.gz https://codeload.github.com/seleznevae/libfort/tar.gz/refs/tags/v${LIBFORT_VERSION} \
    && tar -zxf libfort-${LIBFORT_VERSION}.tar.gz \
    && cd libfort-${LIBFORT_VERSION} \
    && cmake -GNinja -H. -Bbuild -DCMAKE_INSTALL_PREFIX=${INSTALL_PATH} \
    && cmake --build build \
    && cmake --build build --target install \
    && cd .. \
    && rm -rf libfort-${LIBFORT_VERSION} libfort-${LIBFORT_VERSION}.tar.gz

ENV PATH="`pwd`/install:${PATH}"
