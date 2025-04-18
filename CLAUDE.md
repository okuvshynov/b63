# B63 Project Reference

This is a reference for the B63 benchmarking library. Here are the documentation files:

- [Overview](/ref/overview.md) - High-level overview of the B63 library and its features
- [Usage Guide](/ref/usage.md) - How to use B63 for benchmarking your code
- [Architecture](/ref/architecture.md) - Core components and design of the library
- [Counters](/ref/counters.md) - Details on the counter system for measurements
- [API Reference](/ref/api.md) - Complete API documentation
- [Examples](/ref/examples.md) - Example code and usage patterns

## Key Files

- `/include/b63/b63.h` - Main include file
- `/include/b63/benchmark.h` - Core benchmark structures
- `/include/b63/counter.h` - Counter interface
- `/include/b63/run.h` - Benchmark execution

## Building Examples

```bash
cd examples
make
```

## Running a Benchmark

```bash
./examples/basic -i -c time,cycles -e 10 -t 5.0 -s 42
```

Options:
- `-i`: Interactive mode
- `-c`: Specify counters
- `-e`: Number of epochs
- `-t`: Time limit per benchmark
- `-s`: Random seed