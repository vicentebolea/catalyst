## `catalyst_replay` now sets an RPATH on macOS

`catalyst_replay` appended an install RPATH only on non-Apple UNIX platforms,
leaving macOS builds with no `LC_RPATH` at all. Since `libcatalyst` has an
`@rpath`-relative install name, the installed `catalyst_replay` failed to start
with `Library not loaded: @rpath/libcatalyst.3.dylib ... Reason: no LC_RPATH's
found`.

It now uses `@loader_path` on Apple platforms, matching what the Catalyst Python
module already did. The stub implementation's identical guard is corrected too,
though nothing depended on it.
