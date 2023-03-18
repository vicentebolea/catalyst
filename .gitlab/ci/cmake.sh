#!/bin/sh

set -e

readonly version="3.26.0"

case "$( uname -s )" in
    Linux)
        shatool="sha256sum"
        sha256sum="69b55523145b2e619f637e7766c413cb1b7de1f06269ea1eab4a655d59847d59"
        platform="linux-x86_64"
        ;;
    Darwin)
        shatool="shasum -a 256"
        sha256sum="9c4634a421458c63ce90cb4152f6663021e045f8d9c481fd949e8b19d562b2bb"
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
