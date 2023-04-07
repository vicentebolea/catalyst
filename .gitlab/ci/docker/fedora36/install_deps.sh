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
  python-devel python-numpy

dnf clean all
