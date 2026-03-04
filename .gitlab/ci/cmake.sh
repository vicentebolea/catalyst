#!/bin/sh

set -e

readonly version="3.29.8"

case "$( uname -s )" in
    Linux)
        case "$( uname -m )" in
            x86_64)
                sha256sum="0ed430e41059eab130010e74c6a779b651340f3755eb9525e2a573a989f0994e"
                platform="linux-x86_64"
                ;;
            aarch64)
                sha256sum="d8e9f789253c75aabc565d3702759c17c1c2f57e8deb639a8f4ffe4b89aefc2e"
                platform="linux-aarch64"
                ;;
            *)
                echo "Unrecognized architecture $( uname -m )"
                exit 1
                ;;
        esac
        shatool="sha256sum"
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
