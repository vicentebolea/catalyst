#!/bin/sh

set -e

rpm --import https://repo.almalinux.org/almalinux/RPM-GPG-KEY-AlmaLinux

. /root/setup_intel_compilers.sh

dnf install -y --setopt=install_weak_deps=False \
    unzip

# Upgrade libarchive (for CMake)
dnf upgrade -y --setopt=install_weak_deps=False \
    libarchive

# Python dependencies
dnf install -y --setopt=install_weak_deps=False \
  python3-devel python3-numpy

pip3 install --upgrade pip
pip3 install 'mpi4py<4'

dnf clean all
