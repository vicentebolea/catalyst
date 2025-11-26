$erroractionpreference = "stop"

$version = "1.17.0"
$sha256sum = "65FAB701D9829D38CB77C14ACDC431D2108BFDBF8979E40EB8AE567EDF10B27C"
$filename = "googletest-$version"
$tarball = "$filename.tar.gz"

$outdir = $pwd.Path
$outdir = "$outdir\.gitlab"
$ProgressPreference = "SilentlyContinue"
Invoke-WebRequest -Uri "https://github.com/google/googletest/releases/download/v$version/$tarball" -OutFile "$outdir\$tarball"
$hash = Get-FileHash "$outdir\$tarball" -Algorithm SHA256
if ($hash.Hash -ne $sha256sum) {
    exit 1
}

cd .gitlab
cmake -E tar xf "$tarball"

cd "$filename"
New-Item -Path "build" -Type Directory
cd build

if ($env:CMAKE_BUILD_TYPE -eq "") {
    $build_type = "Release"
} else {
    $build_type = "$env:CMAKE_BUILD_TYPE"
}

cmake -GNinja `
    -DCMAKE_BUILD_TYPE="$build_type" `
    -DCMAKE_INSTALL_PREFIX="$env:CI_PROJECT_DIR/.gitlab/gtest" `
    -DBUILD_GMOCK=OFF `
    ..
cmake --build .
cmake --build . --target install

cd "$outdir\.."
