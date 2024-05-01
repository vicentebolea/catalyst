$erroractionpreference = "stop"

$version = "1.10.0"
$sha256sum = "9DC9157A9A1551EC7A7E43DAEA9A694A0BB5FB8BEC81235D8A1E6EF64C716DCB"
$filename = "googletest-release-$version"
$tarball = "release-$version.tar.gz"

$outdir = $pwd.Path
$outdir = "$outdir\.gitlab"
$ProgressPreference = "SilentlyContinue"
Invoke-WebRequest -Uri "https://github.com/google/googletest/archive/$tarball" -OutFile "$outdir\$tarball"
$hash = Get-FileHash "$outdir\$tarball" -Algorithm SHA256
if ($hash.Hash -ne $sha256sum) {
    exit 1
}

cd .gitlab
cmake -E tar xf "$tarball"

cd "$filename"
New-Item -Path "build" -Type Directory
cd build

cmake -GNinja `
    -DCMAKE_BUILD_TYPE=Release `
    -DCMAKE_INSTALL_PREFIX="$env:GIT_CLONE_PATH/.gitlab/gtest" `
    -DBUILD_GMOCK=OFF `
    ..
cmake --build .
cmake --build . --target install

cd "$outdir\.."
