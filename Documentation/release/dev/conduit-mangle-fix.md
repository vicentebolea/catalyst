## conduit-mangle-fix

The `conduit_node_fetch_node_data_ptr` function had been improperly mangled as
`catalyst_conduit_node_fetch_path_data_ptr`. The function is now properly
mangled and the improper mangled name is provieded for compatibility with older
Catalyst versions.
