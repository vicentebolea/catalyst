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

pip3 install mpi4py

dnf clean all

# we need newer ninja to support Fortran
readonly version="1.10.2"
readonly shatool="sha256sum"
readonly sha256sum="763464859c7ef2ea3a0a10f4df40d2025d3bb9438fcb1228404640410c0ec22d"
readonly platform="linux"
readonly filename="ninja-$platform"
readonly tarball="$filename.zip"
echo "$sha256sum  $tarball" > ninja.sha256sum
curl -OL "https://github.com/ninja-build/ninja/releases/download/v$version/$tarball"
$shatool --check ninja.sha256sum
unzip "$tarball"
mv ninja /bin/
