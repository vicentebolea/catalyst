$erroractionpreference = "stop"

$version = "4.1.4"

if ("$env:PROCESSOR_ARCHITECTURE" -eq "AMD64") {
    $sha256sum = "8C31AABDF2223A7BB5759209ADA56A9A72C6A1C605C221F23967130D1989E171"
    $platform = "windows-x86_64"
} elseif ("$env:PROCESSOR_ARCHITECTURE" -eq "ARM64") {
    $sha256sum = "82967CB4F33BD56C0D3DBCEB13AA887156DEAC31F5B3A3DB07BDFA96B11A7229"
    $platform = "windows-arm64"
} else {
    throw ('unknown PROCESSOR_ARCHITECTURE: ' + "$env:PROCESSOR_ARCHITECTURE")
}

$filename = "cmake-$version-$platform"
$tarball = "$filename.zip"

$outdir = $pwd.Path
$outdir = "$outdir\.gitlab"
$ProgressPreference = 'SilentlyContinue'
Invoke-WebRequest -Uri "https://github.com/Kitware/CMake/releases/download/v$version/$tarball" -OutFile "$outdir\$tarball"
$hash = Get-FileHash "$outdir\$tarball" -Algorithm SHA256
if ($hash.Hash -ne $sha256sum) {
    exit 1
}

Add-Type -AssemblyName System.IO.Compression.FileSystem
[System.IO.Compression.ZipFile]::ExtractToDirectory("$outdir\$tarball", "$outdir")
Move-Item -Path "$outdir\$filename" -Destination "$outdir\cmake$env:CMAKE_SUFFIX"
Remove-Item "$outdir\$tarball"
