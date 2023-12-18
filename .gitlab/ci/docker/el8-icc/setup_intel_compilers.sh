#!/bin/sh

set -e
# enable Intel compilers
source /opt/intel/oneapi/setvars.sh
export CC=icc
export CXX=icpc
export FC=ifort
# silence remark about classic compiler getting deprecated
export CXXFLAGS="-diag-disable=10441"
export CFLAGS="-diag-disable=10441"
