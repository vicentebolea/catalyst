#!/bin/sh

set -e

readonly version="1.17.0"
readonly sha256sum="65fab701d9829d38cb77c14acdc431d2108bfdbf8979e40eb8ae567edf10b27c"
readonly filename="googletest-$version"
readonly tarball="$filename.tar.gz"

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
curl -OL "https://github.com/google/googletest/releases/download/v$version/$tarball"
$shatool --check gtest.sha256sum

cmake -E tar xf "$tarball"
cd "$filename"
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
