# B63 API Reference

## Core Macros

### Benchmark Definition

```c
B63_BENCHMARK(name, n_param)
```
- Defines a benchmark function with name `name`
- `n_param` is the parameter for iteration count
- Must contain code to be measured

```c
B63_BASELINE(name, n_param)
```
- Similar to `B63_BENCHMARK` but marks this as the baseline for comparison
- Other benchmarks will be compared against this baseline

### Counter Definition

```c
B63_COUNTER(name)
```
- Defines a custom counter with name `name`
- Must return an `int64_t` value representing the counter's current value

### Benchmark Control

```c
B63_KEEP(value)
```
- Prevents compiler from optimizing away calculations
- Should be used on the result of benchmarked operations

```c
B63_SUSPEND { ... }
```
- Temporarily suspends measurement
- Useful for excluding setup/teardown operations from benchmarks

### Benchmark Execution

```c
B63_RUN(argc, argv)
```
- Runs all registered benchmarks with default counters
- Processes command-line arguments from `argc` and `argv`

```c
B63_RUN_WITH(counters, argc, argv)
```
- Runs all registered benchmarks with specified counters
- `counters` is a comma-separated string of counter names

## Data Structures

### Benchmark Structure

```c
typedef struct {
  // Function pointer to benchmark implementation
  void (*fn)(int64_t iterations, void *opaque);
  // Benchmark name
  const char *name;
  // User data
  void *udata;
} b63_benchmark;
```

### Counter Structure

```c
typedef struct {
  // Initialize counter
  void (*init)(b63_counter *c);
  // Clean up counter resources
  void (*clean)(b63_counter *c);
  // Read current counter value
  int64_t (*read)(void);
  // User data for counter
  void *udata;
  // Counter name
  const char *name;
} b63_counter;
```

### Suite Structure

```c
typedef struct {
  // List of benchmarks
  b63_benchmark_list benchmarks;
  // List of counters
  b63_counter_list counters;
  // Printer for results
  b63_printer printer;
  // Configuration
  b63_config config;
} b63_suite;
```

### Configuration Structure

```c
typedef struct {
  // Whether to run in interactive mode
  int interactive;
  // Number of epochs to run
  int epoch_count;
  // Time limit for benchmarks (seconds)
  double time_limit;
  // Output delimiter
  const char *delimiter;
  // Random seed
  unsigned int seed;
} b63_config;
```

## Functions

### Suite Management

```c
// Initialize suite with default configuration
void b63_suite_init(b63_suite *suite);

// Process command-line arguments into configuration
int b63_suite_parse_args(b63_suite *suite, int argc, char **argv);

// Register a benchmark with the suite
void b63_suite_register(b63_suite *suite, b63_benchmark *bm);

// Register a counter with the suite
void b63_suite_add_counter(b63_suite *suite, b63_counter *counter);

// Run all benchmarks in the suite
void b63_suite_run(b63_suite *suite);
```

### Counter Management

```c
// Initialize a counter list
void b63_counter_list_init(b63_counter_list *list);

// Add a counter to a list
void b63_counter_list_add(b63_counter_list *list, b63_counter *counter);

// Find a counter by name
b63_counter *b63_counter_list_find(b63_counter_list *list, const char *name);

// Clean up a counter list
void b63_counter_list_clean(b63_counter_list *list);
```

### Utility Functions

```c
// Get current time in nanoseconds
int64_t b63_now_ns(void);

// Calculate statistics (mean, stdev, etc.) from samples
void b63_stats_calc(double *samples, int n, b63_stats *stats);

// Format a table row for output
void b63_ttable_row(b63_ttable *table, const char *format, ...);
```