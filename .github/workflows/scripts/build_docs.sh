#!/bin/sh

################################################################################
#
# The master build_docs.sh script lives at
# NWChemEx-Project/DeveloperTools/.gihub/workflows/scripts. The build_docs.sh
# script contained in all other NWChemEx-Project repositories is a synchronized
# copy of the master script. Thus to make changes to the build_docs.sh file
# please make them to the master script.
#
################################################################################

# This script encapsulates the process of building the documentation for an NWX
# repo.
#
# Usage:
#   build_docs
# Enviornment Variables:
#   cmake_version: The version of CMake used, in the format x.y.z
#
# Notes:
#   - This script assumes Sphinx is installed via PIP into a virtual environment
#     called "venv"

set - e # Exit with error if any command fails

arch="Linux-x86_64"
cmake_command=cmake-${cmake_version}-${arch}/bin/cmake

# Step 1: Build the Doxygen documentation
${cmake_command} -H. -Bbuild -DBUILD_DOCS=ON
${cmake_command} --build build --target cxx_api

# Step 2: Migrate the Doxygen documentation to the docs source
mkdir docs/build
mkdir docs/build/html
mv build/html docs/build/html/cxx_api

#Step 3: Build the Sphinx documentation
. venv/bin/activate
cd docs
make html
