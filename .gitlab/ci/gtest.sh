#!/bin/sh

set -e

readonly version="1.10.0"
readonly sha256sum="9dc9157a9a1551ec7a7e43daea9a694a0bb5fb8bec81235d8a1e6ef64c716dcb"
readonly tarball="release-$version.tar.gz"
readonly target="googletest-release-$version"

case "$( uname -s )" in
    Linux)
        shatool="sha256sum"
        ;;
    Darwin)
        shatool="shasum -a 256"
        ;;
    *)
        echo "Unrecognized platform $( uname -s )"
        exit 1
        ;;
esac
readonly shatool

cd .gitlab

echo "$sha256sum  $tarball" > gtest.sha256sum
curl -OL "https://github.com/google/googletest/archive/$tarball"
$shatool --check gtest.sha256sum

cmake -E tar xf "$tarball"
cd "$target"
mkdir build
cd build

cmake -GNinja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX="$CI_PROJECT_DIR/.gitlab/gtest" \
    -DBUILD_GMOCK=OFF \
    -Dgtest_disable_pthreads=ON \
    ..
cmake --build .
cmake --build . --target install
