#!/usr/bin/env bash

# Enable gcc 9
source /opt/rh/devtoolset-9/enable

# NOTE: cmake and boost must be installed first since the remaining packages depend on them
./tools/scripts/lib_install/install-cmake.sh 3.21.2
./tools/scripts/lib_install/install-boost.sh 1.76.0

./tools/scripts/lib_install/libarchive.sh 3.5.1
./tools/scripts/lib_install/zstandard.sh 1.4.9
