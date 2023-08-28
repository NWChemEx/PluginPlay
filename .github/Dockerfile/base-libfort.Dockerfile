ARG PARENT_IMAGE_NAME

FROM ${PARENT_IMAGE_NAME}:latest

ARG LIBFORT_VERSION

# Install libfort#
ENV INSTALL_PATH=../install

RUN wget --no-check-certificate --content-disposition https://codeload.github.com/seleznevae/libfort/tar.gz/refs/tags/v${LIBFORT_VERSION} \
    && tar -zxf libfort-${LIBFORT_VERSION}.tar.gz \
    && cd libfort-${LIBFORT_VERSION} \
    && cmake -GNinja -H. -Bbuild -DCMAKE_INSTALL_PREFIX=${INSTALL_PATH} \
    && cmake --build build \
    && cmake --build build --target install \
    && cd .. \
    && rm -rf libfort-${LIBFORT_VERSION} libfort-${LIBFORT_VERSION}.tar.gz

ENV PATH="`pwd`/install:${PATH}"
