## Conduit configuration mismatch now produces a clear error

When `CATALYST_WITH_EXTERNAL_CONDUIT` differs between `libcatalyst` and the
loaded implementation, Catalyst now prints an explicit diagnostic to stderr
and safely refuses to proceed, instead of silently crashing in a subsequent
`catalyst_execute` call.
