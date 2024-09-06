#!/bin/sh

set -e

readonly version="3.29.8"

case "$( uname -s )" in
    Linux)
        shatool="sha256sum"
        sha256sum="0ed430e41059eab130010e74c6a779b651340f3755eb9525e2a573a989f0994e"
        platform="linux-x86_64"
        ;;
    Darwin)
        shatool="shasum -a 256"
        sha256sum="7461028490d0d907b66fd8e3cb6871b6368a52605938fbcdb19b38b9738e4c32"
        platform="macos-universal"
        ;;
    *)
        echo "Unrecognized platform $( uname -s )"
        exit 1
        ;;
esac
readonly shatool
readonly sha256sum
readonly platform

readonly filename="cmake-$version-$platform"
readonly tarball="$filename.tar.gz"

cd .gitlab

echo "$sha256sum  $tarball" > cmake.sha256sum
curl -OL "https://github.com/Kitware/CMake/releases/download/v$version/$tarball"
$shatool --check cmake.sha256sum
tar xf "$tarball"
mv "$filename" cmake

if [ "$( uname -s )" = "Darwin" ]; then
    ln -s CMake.app/Contents/bin cmake/bin
fi
