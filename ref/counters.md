# B63 Counters Reference

Counters are the core measurement mechanism in B63. They provide a flexible way to measure different aspects of code performance.

## Counter Interface

All counters in B63 implement a common interface defined in `counter.h`:

```c
typedef struct {
  void (*init)(b63_counter *c);  // Initialize counter
  void (*clean)(b63_counter *c); // Clean up counter resources
  int64_t (*read)(void);         // Read current counter value
  void *udata;                   // User data for counter
  const char *name;              // Counter name
} b63_counter;
```

## Built-in Counters

### Time Counter (`time.h`)

Measures wall-clock time in nanoseconds:

- Uses platform-specific high-resolution timing (clock_gettime, mach_absolute_time, etc.)
- Provides baseline performance measurement for all platforms

### CPU Cycles Counter (`cycles.h`)

Measures CPU cycles consumed:

- Architecture-specific implementation (ARM64, x86)
- Provides lower-level performance measurement than wall-clock time
- More consistent for CPU-bound operations

### Performance Events (`perf_events.h`)

Linux-specific performance counters using the perf_events subsystem:

- CPU cache misses
- Branch mispredictions
- Instructions executed
- And many other hardware performance counters

### Memory Allocation (`jemalloc.h`)

Tracks memory allocation when using jemalloc memory allocator:

- Thread-specific allocation tracking
- Measures bytes allocated/deallocated
- Requires linking against jemalloc with appropriate flags

## Custom Counters

You can define custom counters using the `B63_COUNTER` macro:

```c
int64_t my_metric = 0;

B63_COUNTER(my_counter) {
  return my_metric;
}
```

Custom counters can measure application-specific metrics:

- Function call counts
- Business-specific operations
- Cache hit ratios
- Resource utilization

## Counter Selection

Counters can be specified at runtime using the `-c` flag:

```
./benchmark -c time,cycles,my_counter
```

Or programmatically with `B63_RUN_WITH`:

```c
int main(int argc, char **argv) {
  B63_RUN_WITH("time,cycles,my_counter", argc, argv);
  return 0;
}
```

## Platform-Specific Counters

B63 conditionally compiles certain counters based on the target platform:

- **Linux**: perf_events counters for detailed hardware metrics
- **macOS**: kperf-based counters for Apple hardware
- **All Platforms**: Time and custom counters work everywhere