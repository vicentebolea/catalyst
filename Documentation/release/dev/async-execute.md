## Asynchronous execution support

Catalyst now supports asynchronous execution, enabling visualization work to
overlap with GPU compute phases. When enabled, `catalyst_execute()` returns
immediately after deep-copying data to a bounded queue, while a background
worker thread processes the visualization pipeline.

**No simulation code changes required.** Configure via environment variables:
```bash
export CATALYST_ASYNC_ENABLED=1
export CATALYST_ASYNC_QUEUE_DEPTH=2        # default: 2
export CATALYST_ASYNC_AFFINITY_MODE=auto   # auto, manual, none
export CATALYST_ASYNC_WORKER_CORES="1,2,3" # for manual mode
export CATALYST_ASYNC_VERBOSE=1            # enable debug output
```

Or via JSON configuration file loaded at initialization:
```json
{
  "catalyst": {
    "async": {
      "enabled": 1,
      "queue_depth": 2,
      "affinity": { "mode": "auto" },
      "verbose": 1
    }
  }
}
```
```c
conduit_node* params = conduit_node_create();
conduit_node_load(params, "catalyst_config.json", "json");
catalyst_initialize(params);
```

Or equivalently via Conduit params set programmatically:
```
catalyst/async/enabled = 1
catalyst/async/queue_depth = 2
catalyst/async/affinity/mode = "auto"
```

Key features:
- Transparent to adaptors and implementations (zero code changes)
- Automatic GPU pointer detection and copy (CUDA/HIP via dlopen, no build dependency)
- MPI-synchronized skip decisions prevent collective deadlocks
- Thread affinity support (Linux pthread, Windows SetThreadAffinityMask, hwloc)
- Configurable via environment variables, JSON files, or Conduit params
- No new public API functions (ABI stable)

Optional params-based control (for advanced use cases):
- Query async status: `catalyst_about()` returns `catalyst/async/enabled`
- Flush queue: `catalyst_execute()` with `catalyst/async/flush = 1`

See `docs/async_execution.rst` for detailed documentation.
