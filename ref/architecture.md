# B63 Architecture Reference

## Core Components

B63 is organized around several key components that work together to provide benchmarking functionality:

### 1. Benchmark Definition

Benchmarks are defined using macros that create functions with a standardized signature:

- `B63_BENCHMARK`: Defines a benchmark to be measured
- `B63_BASELINE`: Defines a baseline benchmark for comparison

### 2. Suite Management

The benchmark suite handles configuration, initialization, and execution:

- Suite initialization processes command line arguments
- Counter registration and management
- Benchmark execution and result collection
- Output formatting and display

### 3. Counter System

Counters provide the metrics for measurement:

- Built-in counters (time, cycles)
- Platform-specific counters (perf_events, jemalloc)
- Custom counters for specialized metrics

### 4. Execution Flow

1. Initialize suite configuration from command line
2. Register counters specified in configuration
3. Execute each benchmark for the specified number of epochs
4. Collect and process measurements
5. Format and display results

## Key Data Structures

### Benchmark Structure (`b63_benchmark`)

Represents a single benchmark function:

- Function pointer to the benchmark implementation
- Name for identification
- Configuration parameters

### Epoch Structure (`b63_epoch`)

Represents a single measurement unit:

- Iteration count
- Counter values before and after execution

### Counter Structure (`b63_counter`)

Represents a specific performance metric:

- Function pointers for initialization, cleanup, and measurement
- Name for identification
- Configuration parameters

### Suite Structure (`b63_suite`)

Represents the entire benchmark suite:

- List of registered benchmarks
- List of registered counters
- Configuration parameters
- Output printer configuration

## File Organization

The B63 library is organized into several header files, each with specific responsibilities:

### Core Headers

- `b63.h`: Main include file that brings in all necessary components
- `benchmark.h`: Core benchmark definition and structures
- `suite.h`: Suite configuration and management
- `run.h`: Execution and control flow for benchmarks

### Counter Implementation

- `counter.h`: Counter interface definition
- `counter_list.h`: Management of counter collections
- `counters/*.h`: Individual counter implementations:
  - `cycles.h`: CPU cycle counting
  - `time.h`: Wall clock time measurement
  - `jemalloc.h`: Memory allocation tracking
  - `perf_events.h`: Linux perf events integration

### Utilities

- `utils/`: Helper functionality:
  - `section_ptr_list.h`: Pointer list management
  - `stats.h`: Statistical calculations
  - `string.h`: String manipulation
  - `timer.h`: Timing utilities
  - `ttable.h`: Text table formatting