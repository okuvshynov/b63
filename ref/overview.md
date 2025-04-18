# B63 - Lightweight Micro-Benchmarking Library

B63 is a lightweight micro-benchmarking tool for C programming language. It provides a simple yet powerful framework for measuring and comparing performance of code snippets with minimal overhead.

## Key Features

- **C-Focused:** Designed specifically for C (not limited to C++ like many other benchmarking tools)
- **Custom Counters:** Measure various performance metrics beyond just time
  - CPU cycles
  - CPU Performance Monitoring Unit counters
  - Cache misses, branch mispredictions
  - jemalloc memory allocation tracking
- **Reproducibility:** Support for seed-based reproducible results
- **Multiple Output Modes:** 
  - Plain text (suitable for scripting and data processing)
  - Interactive mode (for human-readable analysis)

## Core Concepts

### Benchmarks and Baselines

B63 organizes benchmarks around the concept of measuring how performance changes relative to a baseline. This is particularly useful for:

- Comparing different implementations of the same algorithm
- Measuring performance impact of code changes
- Testing optimization effectiveness

### Counters

The library provides a flexible counter system that allows measuring different metrics:

- **Time:** Wall-clock time in nanoseconds
- **Cycles:** CPU cycle count (architecture-dependent)
- **Memory:** When using jemalloc, track memory allocation
- **Custom:** Easily create your own counters for application-specific metrics

### Runtime Configuration

B63 offers configuration through command-line arguments:

- `-i`: Interactive mode for human-readable output
- `-c`: Override default counters
- `-e`: Set epoch count for each benchmark
- `-t`: Set time limit per benchmark
- `-d`: Set delimiter for plaintext output
- `-s`: Set seed for reproducibility