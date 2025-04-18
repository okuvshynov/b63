# B63 Usage Guide

## Basic Usage

Here's a simple example of how to use B63 for benchmarking:

```c
#include "b63/b63.h"

B63_BENCHMARK(my_benchmark, n) {
  int i = 0, res = 0;
  for (i = 0; i < n; i++) {
    res += rand();  // Operation to benchmark
  }
  B63_KEEP(res);    // Prevent compiler optimization of results
}

int main(int argc, char **argv) {
  B63_RUN(argc, argv);
  return 0;
}
```

## Comparing Implementations (Baseline Pattern)

To compare different implementations, use the baseline pattern:

```c
#include "b63/b63.h"

// Define baseline implementation
B63_BASELINE(baseline_implementation, n) {
  int i = 0, res = 0;
  for (i = 0; i < n; i++) {
    res += method1();
  }
  B63_KEEP(res);
}

// Define alternative implementation to compare against baseline
B63_BENCHMARK(alternative_implementation, n) {
  int i = 0, res = 0;
  for (i = 0; i < n; i++) {
    res += method2();
  }
  B63_KEEP(res);
}

int main(int argc, char **argv) {
  B63_RUN(argc, argv);
  return 0;
}
```

## Custom Counters

You can define custom counters to measure specific metrics:

```c
#include "b63/b63.h"

int64_t my_metric_count = 0;

// Define custom counter
B63_COUNTER(my_metric) { 
  return my_metric_count; 
}

B63_BENCHMARK(benchmark_with_custom_counter, n) {
  int i = 0, res = 0;
  for (i = 0; i < n; i++) {
    // Perform operation and update custom metric
    res += operation();
    my_metric_count++;
  }
  B63_KEEP(res);
}

int main(int argc, char **argv) {
  // Run with custom counter
  B63_RUN_WITH("my_metric", argc, argv);
  return 0;
}
```

## Preventing Compiler Optimization

Use `B63_KEEP` to prevent the compiler from optimizing away operations that don't otherwise affect the program:

```c
B63_BENCHMARK(example, n) {
  int i = 0, res = 0;
  for (i = 0; i < n; i++) {
    res += expensive_calculation();
  }
  // Prevent compiler from eliminating the loop
  B63_KEEP(res);
}
```

## Suspending Measurement

Use `B63_SUSPEND` to temporarily suspend measurement during setup or teardown operations:

```c
B63_BENCHMARK(with_suspend, n) {
  // Setup (not measured)
  B63_SUSPEND {
    setup_data_structures();
  }
  
  // Measured section
  for (int i = 0; i < n; i++) {
    perform_operation();
  }
  
  // Teardown (not measured)
  B63_SUSPEND {
    cleanup_data_structures();
  }
}
```

## Command Line Options

When running your benchmark executable, you can use these options:

```
./benchmark -i -c time,cycles -e 10 -t 5.0 -s 42
```

- `-i`: Enable interactive mode
- `-c time,cycles`: Use only time and cycles counters
- `-e 10`: Run 10 epochs for each benchmark
- `-t 5.0`: Limit each benchmark to 5 seconds
- `-s 42`: Use seed 42 for reproducibility