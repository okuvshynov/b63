# B63 Examples Reference

The B63 project includes several example programs that demonstrate different features and usage patterns. Here's an overview of the key examples:

## Basic Usage (`basic.c`)

Demonstrates the most simple usage of B63:

```c
#include "../include/b63/b63.h"

B63_BENCHMARK(basic, n) {
  int i = 0, res = 0;
  for (i = 0; i < n; i++) {
    res += rand();
  }
  B63_KEEP(res);
}

int main(int argc, char **argv) {
  B63_RUN(argc, argv);
  return 0;
}
```

This example:
- Defines a single benchmark
- Measures the time it takes to call `rand()` multiple times
- Uses `B63_KEEP` to prevent the compiler from optimizing away the results

## Custom Counters (`custom.c`)

Shows how to define and use a custom counter:

```c
#include "../include/b63/b63.h"

int64_t callcount = 0LL;

// Define custom counter
B63_COUNTER(calls) { return callcount; }

static int f() {
  callcount++;
  return /* some value */;
}

B63_BASELINE(call_normal, n) {
  int i = 0, res = 0;
  for (i = 0; i < n; i++) {
    res += f();
  }
  B63_KEEP(res);
}

B63_BENCHMARK(call_twice, n) {
  int i = 0, res = 0;
  for (i = 0; i < n; i++) {
    res += (f() + f());
  }
  B63_KEEP(res);
}

int main(int argc, char **argv) {
  B63_RUN_WITH("calls", argc, argv);
  return 0;
}
```

This example:
- Defines a custom counter that tracks function calls
- Compares baseline (single call) with an alternative implementation (double call)
- Uses `B63_RUN_WITH` to specify which counter to use

## Baseline Comparison (`baseline.c`)

Demonstrates comparing different implementations against a baseline:

```c
B63_BASELINE(some_baseline, n) {
  // Baseline implementation
}

B63_BENCHMARK(another_implementation, n) {
  // Alternative implementation
}

B63_BENCHMARK(yet_another_implementation, n) {
  // Another alternative implementation
}
```

## Suspending Measurement (`suspend.c`)

Shows how to exclude setup/teardown code from measurement:

```c
B63_BENCHMARK(with_suspend, n) {
  // Setup (not measured)
  B63_SUSPEND {
    // Setup code here
  }
  
  // Measured section
  for (int i = 0; i < n; i++) {
    // Code to measure
  }
  
  // Teardown (not measured)
  B63_SUSPEND {
    // Cleanup code here
  }
}
```

## Additional Examples

- **assert.c**: Demonstrates using assertions in benchmarks
- **baseline_multi.c**: Multiple baseline comparisons
- **jemalloc.cpp**: Memory allocation tracking with jemalloc
- **l1d_miss.cpp**: Measuring cache misses
- **raw.c**: Low-level benchmark implementation
- **storms.cpp**: Complex benchmarking scenario

## Building and Running Examples

The examples directory includes a Makefile for building the examples:

```bash
# Navigate to examples directory
cd examples

# Build all examples
make

# Run a specific example
./basic
./custom
./suspend

# Run with options
./basic -i -c time,cycles -e 10
```