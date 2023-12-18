#!/bin/sh

set -e

. /root/setup_intel_compilers.sh

dnf install -y --setopt=install_weak_deps=False \
    cmake

dnf clean all
# if version changes, make sure to update all places where "conduit_version" is used
readonly conduit_version="0.8.7"
readonly conduit_tarball="conduit-v$conduit_version-src-with-blt.tar.gz"
readonly conduit_sha256sum="f3bf44d860783f4e0d61517c5e280c88144af37414569f4cf86e2d29b3ba5293"

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

cmake_args=""

update_cmake_args()
{
    cmake_args="\
-GNinja \
-DCMAKE_INSTALL_PREFIX=$INSTALL_PATH \
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
-DENABLE_FORTRAN=$ENABLE_WRAPPERS \
-DENABLE_FRUIT=OFF \
-DENABLE_GTEST=OFF \
-DENABLE_PYTHON=$ENABLE_WRAPPERS \
-DENABLE_RELAY_WEBSERVER=OFF \
-DENABLE_SPHINX=OFF \
-DENABLE_TESTS=OFF \
-DENABLE_UNCRUSTIFY=OFF \
-DENABLE_UTILS=OFF \
-DENABLE_VALGRIND=OFF \
-DENABLE_YAPF=OFF \
$conduit_src/src
"
}

# install non-wrapped version of conduit
INSTALL_PATH="/opt/conduit/basic"
ENABLE_WRAPPERS="OFF"
update_cmake_args

cmake $cmake_args
ninja
ninja install

# install wrapped version of conduit
INSTALL_PATH="/opt/conduit/wrapped"
ENABLE_WRAPPERS="ON"
update_cmake_args

ninja clean

cmake $cmake_args
ninja
ninja install

cd

rm -rf "$conduit_root"
