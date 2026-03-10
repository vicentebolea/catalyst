#!/bin/sh

set -e

readonly version="4.1.4"

case "$(uname -s)-$(uname -m)" in
    Linux-x86_64)
        shatool="sha256sum"
        sha256sum="af5c72ce7ce61607d70ff80445130f6278204aa46c45127edc0dc36e26a687b2"
        platform="linux-x86_64"
        ;;
    Linux-aarch64)
        shatool="sha256sum"
        sha256sum="efe4dfc830fbf4d897af8e210f5a0632ad2b93c015a37c9305d40a57b0e4c538"
        platform="linux-aarch64"
        ;;
    Darwin-*)
        shatool="shasum -a 256"
        sha256sum="7fc64e989af34201acff6d16628d6a8d1f72cfb74d5bd9342ab332c8c00e69ab"
        platform="macos-universal"
        ;;
    *)
        echo "Unrecognized platform $(uname -s)-$(uname -m)"
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
mv "$filename" "cmake$CMAKE_SUFFIX"
rm "$tarball" cmake.sha256sum

if [ "$( uname -s )" = "Darwin" ]; then
    ln -s CMake.app/Contents/bin "cmake$CMAKE_SUFFIX/bin"
fi
