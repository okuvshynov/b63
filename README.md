# B63

Light-weight micro-benchmarking tool for C.

## Motivation
Why was it built, given that quite a few already exist?
- quick and easy benchmarking for C, not C++ only;
- benchmarking is usually associated with measuring wall time, CPU time or CPU cycles. I needed other 'counters' as well, specifically:
  - completely custom measurements, like 'number of hash collisions';
  - CPU Performance Monitoring Unit counters, like the number of cache misses and branch mispredictions;
  - jemalloc memory allocations;
- preferably header-only library, no 'installation';

## Examples
The easiest way to get a sense of how it could be used is to look at and 
run benchmarks from examples/ folder. The library is header-only, so the only 'dependencies' which examples express are: 
- b63.h header
- individual counter headers.

For example, this is how 'benchmarking' time, cpu cycles and cache misses might look like on Linux:

```cpp
#include "../src/b63.h"
#include "../src/counters/perf_events.h"
#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <numeric>
#include <vector>

const size_t kSize = (1 << 16);
const size_t kMask = kSize - 1;

/* 
 * B63_BASELINE defines a 'baseline' function to benchmark.
 * In this definition, 'sequential' is benchmark name,
 * and 'n' is the parameter the function needs to use as 
 * 'how many iterations to run'. It is important to have this parameter
 * to be able to adjust the run time dynamically
 */
B63_BASELINE(sequential, n) {
  std::vector<uint32_t> v;

  /* 
   * Anything within 'B63_SUSPEND' will not be counted
   * towards benchmark score.
   */
  B63_SUSPEND {
    v.resize(kSize);
    std::iota(v.begin(), v.end(), 0);
  }
  int32_t res = 0;
  for (size_t i = 0; i < n; i++) {
    for (size_t j = 0; j < kSize; j++) {
      res += v[j];
    }
  }
  /* this is to prevent compiler from optimizing res out */
  B63_KEEP(res);
}

/*
 * This is another benchmark, which will be 'compared' to baseline
 */
B63_BENCHMARK(random, n) {
  std::vector<uint32_t> v;
  B63_SUSPEND {
    v.resize(kSize);
    std::generate(v.begin(), v.end(), std::rand);
  }
  int32_t res = 0;
  for (size_t i = 0; i < n; i++) {
    for (size_t j = 0; j < kSize; j++) {
      res += v[v[j] & kMask];
    }
  }
  B63_KEEP(res);
}

int main(int argc, char **argv) {
  srand(time(0));
  /* 
   * this call starts benchmarking.
   * List of 'counters' to measure is passed explicitly here,
   * but one can provide command-line flag -c to override.
   * In this case, we are measuring 4 counters:
   *  * lpe:cycles - CPU cycles spent in benchmark (outside of B63_SUSPEND), as measured with Linux perf_events.
   *  * lpe:LLC-load-misses - CPU last level cache (typically L3 these days) misses during benchmark run (outside of B63_SUSPEND).
   *  * lpe:L1-dcache-load-misses - CPU L1 Data cache misses during benchmark run (outside of B63_SUSPEND)
   *  * time - wall time (outside of B63_SUSPEND)
   */
  B63_RUN_WITH("time,lpe:cycles,lpe:LLC-load-misses,lpe:L1-dcache-load-misses", argc, argv);
  return 0;
}
```

Build and run:

This is the output of the sample run:
```
$ g++ -O3 bm.cpp -o bm
$ ./bm -i # i for 'interactive'
[DONE] sequential                    : time : 52495.468864 per iteration (baseline)
[DONE] random                        : time : 147816.263801 per iteration (+181.579%)
[DONE] sequential                    : lpe:cycles : 131864.811233 per iteration (baseline)
[DONE] random                        : lpe:cycles : 370943.605765 per iteration (+181.306%)
[DONE] sequential                    : lpe:LLC-load-misses : 0.002442 per iteration (baseline)
[DONE] random                        : lpe:LLC-load-misses : 0.007328 per iteration (+200.073%)
[DONE] sequential                    : lpe:L1-dcache-load-misses : 4410.444933 per iteration (baseline)
[DONE] random                        : lpe:L1-dcache-load-misses : 80459.789448 per iteration (+1724.301%)
```
Currently B63 repeats the run for every counter to reduce side-effects of measurement, but this might change in the future.
The way to read the results: for benchmark 'sequential', which is baseline version, we spent 52 milliseconds per 'iteration', there were only a tiny amount of LLC load misses, because entire dataset fits into L3 cache;
For 'random' version, we see clear increase in time and equivalent increase in CPU cycles (+181%), and a very prominent increase in L1 cache misses (+1724%).

Extra examples can be found in examples/ folder:
1. Measuring time / iteration ([examples/basic.c](examples/basic.c)):
2. Suspending tracking ([examples/suspend.c](examples/suspend.c)):
3. Comparing implementations with baseline ([examples/baseline.c](examples/baseline.c)):
4. Using custom counter, number of function calls in this case ([examples/custom.c](examples/custom.c))
5. Using cache miss counter from linux perf event ([examples/l1d_miss.cpp](examples/l1d_miss.cpp))
6. Using raw counter from linux perf event ([examples/raw.c](examples/raw.c)):
7. Measuring jemalloc allocation stats ([examples/jemalloc.cpp](examples/jemalloc.cpp))

## Comparison and baselines
Within the benchmark suite, there's a way to define 'baseline', and compare all other benchmarks against it. Comparison is very naive at the moment: it runs benchmarks for a few epochs, picks the best one and compares. This is ~reasonable way to deal with noise. Maybe we could support passing some seed around, making each epoch run 'reproducible' and do a more comprehensive comparison, like paired t-test.

## Output Modes
Two output modes are supported:
 - plaintext mode (default), which produces output suitable for scripting/parsing.
 - interactive mode turned on with -i flag. There isn't much interactivity really, but the output is formatted and colored for human consumption, rather than other tool consumption.

## Configuration

### CLI Flags

Following CLI flags are supported:
-i if provided, interactive output mode will be used
-c counter1[,counter2,counter3,...] -- override default counters for all benchmarks.
-e epochs_count -- override how many epochs to run the benchmark for
-t timelimit_per_benchmark - time limit in seconds for how long to run the benchmark; includes time benchmark is suspended.

### Configuration in code
It's possible to configure the counters to run within the code itself, by using B63_RUN_WITH("list,of,counters", argc, argv);

## Counters
Counters are expected to be additive and monotonic;
Implementation of the 'counting' and 'suspension' lives in src/b63_run.h; 
Counters are reported [roughly] like this:
```
total_events = (reading_after_run - reading_before_run - suspended_for);
event_rate = total_events / iterations
```

Counters header files should be included from benchmark c/cpp file directly; only default timer counter is included from
benchmarking library itself. It is done to avoid having an insane amount of ifdefs in the code, as counters have to
be gated by compiler/os/libraries installed and used.
When benchmarks are configured to run with multiple counters, each benchmark is re-run for each counter. This is an easy way to deal with measurement side effects, but has obvious disadvantages:
- benchmark needs to run longer;
- in cases when the variance between benchmarks runs is high, results might look confusing.

The suspension is an important case to understand and interpret correctly; 
In interactive mode, the rate of events is reported, while in plaintext mode number of iterations and number of events is printed out.
Time limit for running the benchmark is taking time spent in suspension into account, to make runtime ~predictable.
In cases when suspended operations are taking long time, result for [the benchmark](examples/suspend.c) might look like this:

```
$ ./_build/bm_suspend
with_suspend,time,2097151,150854030
basic,time,4194303,301687751
$ ./_build/bm_suspend -i
[DONE] with_suspend                  : time : 71.936094 per iteration
[DONE] basic                         : time : 71.967727 per iteration
```

The way to interpret it is: 'with_suspend' is equivalent in 'non-suspended' time, thus the time/iteration is very close. However, the suspended activity takes a while, so we had to run fewer iterations overall. There's a set of counters already built:

### Linux perf_events ("lpe:...")
The acronym used is 'lpe'.
This counter uses perf_events interface, same as Linux perf tool. It allows counting CPU events either by predefined names for 
popular counters (cycles, cache-misses, branches) or custom CPU-specific raw codes. Example usage:
```
$ ./bm_raw -c lpe:cycles,lpe:r04a1
```

### Jemalloc thread allocations ("jemalloc_thread_allocated")
This counter tracks the number of bytes allocated by the calling thread. Example usage:
```
$ ./bm_jemalloc -c jemalloc_thread_allocated
```

### Time ("time")
Default counter, uses CLOCK_MONOTONIC, counts microseconds.

## Dependencies and compatibility

B63 requires following C compiler attributes available:
- __attribute__((cleanup))
- __attribute__((used)) 
- __attribute__((section))

Reasonably recent GCC and Clang have them, but I'm not sure which versions started supporting them.

Individual counters can have specific requirements. For example, Linux perf_events, not surprisingly,
will only work on Linux, jemalloc counter will only work/make sense if memory allocation is done via jemalloc.

### Tested On
1. MacBook Pro 2019
  - OS: MacOS 10.14.6 (x86_64-apple-darwin18.7.0)
  - CPU: Intel(R) Core(TM) i5-8257U
  - Compiler: clang-1001.0.46.4 (Apple LLVM)
2. MacBook Pro 2009 
  - OS: Ubuntu 18.04.3 (Kernel: 4.15.0-58)
  - CPU: Intel(R) Core(TM) 2 Duo P8700
  - Compiler: GCC 7.4.0
3. Paspberry PI
  - OS: Raspbian GNU/Linux 9 (Kernel: 4.14.71-v7+)
  - CPU: ARMv7 Processor rev 4 (v7l)
  - Compiler: GCC 6.3.0
4. [VM] FreeBSD
  - OS: FreeBSD 12.0
  - Compiler: FreeBSD clang 6.0.1
5. MacMini 2007
  - OS: Ubuntu 11.10 (Kernel: 3.0.0-13-generic)
  - CPU: Intel(R) Core(TM)2 CPU T5600
  - Compiler: GCC 4.6.1
  - Caveats:
    - requires -lrt flag, as POSIX realtime extension are not (yet) in libc.
    - ref-cycles event from linux perf_events is not supported.

## Internals
The library consists of a core part responsible for running the benchmarks, and pluggable 'counters'. The library is header-only, thus, there isn't much 'encapsulation' going on.  Every global symbol is prefixed with b63\_.

Main internal data structures are:
1) b63_benchmark. Each function defined with a 'B63_BENCHMARK' or 'B63_BASELINE' macro corresponds to one benchmark instance.
2) b63_suite. Set of all benchmarks defined in the translation unit.
3) b63_ctype. Counter Type. Defines a type/family of a counter, for example, 'linux_perf_event' or 'jemalloc'
4) b63_counter. 'Instance' of a counter, which has to be of one of the defined counter types. 
5) b63_counter_list. Set of all counters to run benchmarks for.
6) b63_run. Individual benchmark execution.

## Next steps:
- a convenient way to measure outliers. For example, as hash maps usually have amortized O(1) cost for lookup, what does p99 lookup time looks like for some lookup distribution? What can be done to improve?
- support CPU perf counters sources beyond Linux perf_events, for example [Intel's PCM](https://github.com/opcm/pcm) and [BSD pmcstat](https://www.freebsd.org/cgi/man.cgi?query=pmcstat).
- measure/report both CPU and GPU perf counters (at least for Nvidia).
- [low-pri] disk access and network.
