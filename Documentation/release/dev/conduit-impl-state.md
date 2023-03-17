## conduit-impl-state

The state of Catalysts's Conduit being provided by Catalyst or by an external
build is now part of the implementation structure. Implementations built
against a Catalyst with a different state of Conduit are not compatible with
each other. The new `catalyst_status_error_conduit_mismatch` status is returned
if a Conduit mismatch is detected.
