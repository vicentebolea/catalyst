#!/bin/sh

set -e
# enable Intel compilers
. /opt/intel/oneapi/setvars.sh
export CC=icx
export CXX=icpx
export FC=ifx
