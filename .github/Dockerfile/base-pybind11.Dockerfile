ARG PARENT_IMAGE_NAME

FROM ${PARENT_IMAGE_NAME}:latest

# Install pybind11 ##
RUN apt-get update \
    && DEBIAN_FRONTEND=noninteractive apt-get install -y python-pybind11 \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*
