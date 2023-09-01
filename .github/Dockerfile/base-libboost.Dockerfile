ARG PARENT_IMAGE_NAME

FROM ${PARENT_IMAGE_NAME}:latest

ARG LIBBOOST_VERSION

# Install libboost ##
RUN apt-get update \
    && DEBIAN_FRONTEND=noninteractive apt-get install -y libboost-all-dev \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*
