#!/bin/sh

set -e

dnf install -y --setopt=install_weak_deps=False \
    cmake

dnf clean all

readonly conduit_version="0.8.6"
readonly conduit_tarball="conduit-v$conduit_version-src-with-blt.tar.gz"
readonly conduit_sha256sum="8ca5d37033143ed7181c7286dd25a3f6126ba0358889066f13a2b32f68fc647e"

readonly conduit_root="$HOME/conduit"
readonly conduit_src="$conduit_root/src"
readonly conduit_build="$conduit_root/build"

mkdir -p "$conduit_root"
cd "$conduit_root"

curl -OL "https://github.com/LLNL/conduit/releases/download/v$conduit_version/$conduit_tarball"
echo "$conduit_sha256sum  $conduit_tarball" > conduit.sha256sum
sha256sum --check conduit.sha256sum

mkdir -p "$conduit_src"
tar --strip-components=1 -C "$conduit_src" -xf "$conduit_tarball"

mkdir -p "$conduit_build"
cd "$conduit_build"

cmake \
    -G Ninja \
    -DCMAKE_INSTALL_PREFIX=/usr/local \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_GTEST=OFF \
    -DENABLE_ASTYLE=OFF \
    -DENABLE_CLANGFORMAT=OFF \
    -DENABLE_CLANGQUERY=OFF \
    -DENABLE_CLANGTIDY=OFF \
    -DENABLE_CMAKEFORMAT=OFF \
    -DENABLE_CPPCHECK=OFF \
    -DENABLE_DOCS=OFF \
    -DENABLE_DOXYGEN=OFF \
    -DENABLE_EXAMPLES=OFF \
    -DENABLE_FRUIT=OFF \
    -DENABLE_GTEST=OFF \
    -DENABLE_RELAY_WEBSERVER=OFF \
    -DENABLE_SPHINX=OFF \
    -DENABLE_TESTS=OFF \
    -DENABLE_UNCRUSTIFY=OFF \
    -DENABLE_UTILS=OFF \
    -DENABLE_VALGRIND=OFF \
    -DENABLE_YAPF=OFF \
    "$conduit_src/src"
ninja
ninja install

cd

rm -rf "$conduit_root"
