#!/bin/sh

set -e

# Install extra dependencies for ParaView
dnf install -y --setopt=install_weak_deps=False \
    git-core git-lfs

# MPI dependencies
dnf install -y --setopt=install_weak_deps=False \
    openmpi-devel mpich-devel

# Development tools
dnf install -y --setopt=install_weak_deps=False \
    libasan libtsan libubsan clang-tools-extra \
    gcc gcc-c++ \
    ninja-build

# Python dependencies
dnf install -y --setopt=install_weak_deps=False \
  python3-devel python-numpy python3-mpi4py-mpich

# This is needed for archs other than x86_64 and aarch64 since cmake does not
# provide upstream binary distributions.
case "$(uname -m)" in
  x86_64|aarch64)
    ;;
  *)
    dnf install -y --setopt=install_weak_deps=False cmake
    ;;
esac

dnf clean all
