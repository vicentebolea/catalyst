## Implementation path info in catalyst_about

The `catalyst_about` function now returns information about the loaded
Catalyst implementation:

- `catalyst/version`: The libcatalyst runtime version
- `catalyst/implementation_path`: Full path to the loaded library
  (or "(stub)" for the built-in stub implementation)
- `catalyst/implementation_source`: How the implementation was found
  ("environment", "params", "default", or "stub")
- `catalyst/implementation_abi_version`: The ABI version of the loaded implementation

This information is useful for debugging and understanding which backend
is active.
