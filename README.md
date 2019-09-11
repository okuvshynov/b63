# B63

Light-weight micro-benchmarking tool for C.

## Motivation
Why was it built, given that quite a few already exist?
- quick and easy benchmarking for C, not C++ only;
- benchmarking custom counters, rather than time/cycles only, specifically:
  - CPU Performance Monitoring Unit counters, for example number of cache misses or branch mispredictions;
  - jemalloc memory allocations;
  - custom measurements, like number of hash collisions.

## Examples
The easiest way to get a sense of how it could be used is to look at and 
run benchmarks from examples/ folder. The library is header-only, so examples only need to include:
- b63.h header;
- individual counter headers, if needed.

This is how benchmarking time, cpu cycles and cache misses might look like on Linux:

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
 * This is another benchmark, which will be compared to baseline
 */
B63_BENCHMARK(random, n) {
  std::vector<uint32_t> v;
  B63_SUSPEND {
    /* b63_seed is passed implicitly to every benchmark */
    std::srand(b63_seed);
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
   * This call starts benchmarking.
   * Comma-separated list of counters to measure is passed explicitly here,
   * but one can provide command-line flag -c to override.
   * In this case, we are measuring 3 counters:
   *  * lpe:cycles - CPU cycles spent in benchmark outside of B63_SUSPEND, as measured with Linux perf_events;
   *  * lpe:L1-dcache-load-misses - CPU L1 Data cache misses during benchmark run outside of B63_SUSPEND;
   *  * time - wall time outside of B63_SUSPEND.
   */
  B63_RUN_WITH("time,lpe:cycles,lpe:L1-dcache-load-misses", argc, argv);
  return 0;
}
```

Build and run:

This is the output of the sample run:
```
$ g++ -O3 bm_seed.cpp -o bm
$ ./bm -i # i for interactive mode
sequential                    time                : 52852.595
random                        time                :*148815.260 (+181.567%)
sequential                    lpe:cycles          : 132263.717
random                        lpe:cycles          :*371542.108 (+180.910%)
sequential                    lpe:L1-dcache-load-misses: 15250.451
random                        lpe:L1-dcache-load-misses:*80782.223 (+429.704%)
```
Currently B63 repeats the run for every counter to reduce side-effects of measurement, but this might change in the future.
The way to read the results: for benchmark 'sequential', which is baseline version, we spent 52 milliseconds per iteration;
For 'random' version, we see clear increase in time and equivalent increase in CPU cycles (+181%), and a much more prominent increase in L1 data cache misses (+1724%). The asteriks means: p99 confidence interval for the difference between benchmark and baseline does not contain 0, thus, you can be 99% confident that it is derectionally correct result.

Extra examples can be found in examples/ folder:
1. Measuring time / iteration ([examples/basic.c](examples/basic.c));
2. Suspending tracking ([examples/suspend.c](examples/suspend.c));
3. Comparing implementations with baseline ([examples/baseline.c](examples/baseline.c));
4. Using custom counter, number of function calls in this case ([examples/custom.c](examples/custom.c));
5. Using cache miss counter from linux perf_events ([examples/l1d_miss.cpp](examples/l1d_miss.cpp));
6. Using raw counter from linux perf_events ([examples/raw.c](examples/raw.c));
7. Measuring jemalloc allocation stats ([examples/jemalloc.cpp](examples/jemalloc.cpp));
8. Utilizing seed to keep benchmark results reproducible ([examples/bm_seed.cpp](examples/bm_seed.cpp));
9. Multiple comparisons, including A/A test: ([examples/baseline_multi.c](examples/baseline_multi.c)).

## Comparison and baselines
Within the benchmark suite, there's a way to define 'baseline', and compare all other benchmarks against it. When comparing, 99% confidence interval is computed using differences between individual epochs.

## Output Modes
Two output modes are supported:
 - plaintext mode (default), which produces output suitable for scripting/parsing, printing out each epoch individually to leave an option for more advanced data studies.
 ```
 $ ./_build/bm_baseline
basic,time,16777215,233781738
basic,time,16777215,228961470
basic,time,16777215,230559174
basic,time,16777215,228707363
basic,time,16777215,228769396
basic_half,time,33554431,227525646
basic_half,time,33554431,228749848
basic_half,time,33554431,228985440
basic_half,time,33554431,228123909
basic_half,time,33554431,228560855
```
 - interactive mode turned on with -i flag. There isn't much interactivity really, but the output is formatted and colored for human consumption, rather than other tool consumption.
 ```
 $ ./_build/bm_baseline -i
basic                         time                : 13.808
basic_half                    time                :* 6.830 (-50.535%)

```

## Configuration

### CLI Flags
Following CLI flags are supported:
- -i if provided, interactive output mode will be used;
- -c counter1[,counter2,counter3,...] -- override default counters for all benchmarks;
- -e epochs_count -- override how many epochs to run the benchmark for;
- -t timelimit_per_benchmark - time limit in seconds for how long to run the benchmark; includes time benchmark is suspended.
- -d delimiter to use for plaintext. Comma is default.
- -s seed. Optional, needed for reproducibility and A/B testing across binaries, for example, different versions of code or difference hardware. If not provided, seed will be generated.

### Configuration in code
It's possible to configure the counters to run within the code itself, by using B63_RUN_WITH("list,of,counters", argc, argv);

## Counters
In addition to measuring time, B63 allows to define and use custom counters, for example CPU perf events. Some counters are already built and provided in counters/ folder, but framework is flexible and makes it easy to define new ones.

For now following counters are implemented:
1) time - most basic counter, measures time in microseconds. [Linux, FreeBSD, MacOS]
2) jemalloc - measures bytes allocated by jemalloc. [Linux, FreeBSD, MacOS]
3) perf_events - measures custom CPU counters, like cache misses, branch mispredictions, etc. [Linux only, 2.6.31+]

### Notes for building custom counters:
Counters are expected to be additive and monotonic;
Implementation of the counting and suspension lives in [src/run.h](src/run.h); [examples/custom.c](examples/custom.c) is a simple case of custom counter definition. All counters shipped with the library can be used as examples, as they do not rely on anything internal from b63. 

Counters header files should be included from benchmark c/cpp file directly; only default timer counter is included from
b63 itself. It is done to avoid having an insane amount of ifdefs in the code and compilicated build rules, as counters have to be gated by compiler/os/libraries installed and used.
When benchmarks are configured to run with multiple counters, each benchmark is re-run for each counter. This is an easy way to deal with measurement side effects, but has obvious disadvantages:
- benchmark needs to run longer;
- in cases when the variance between benchmarks runs is high, results might look confusing.

The suspension is an important case to understand and interpret correctly. To illustrate this, let's look at the following example [benchmark](examples/suspend.c):

```
$ ./_build/bm_suspend
with_suspend,time,8388607,117749190
with_suspend,time,8388607,117033209
with_suspend,time,8388607,114440936
with_suspend,time,8388607,114655889
with_suspend,time,8388607,114215822
basic,time,16777215,228015817
basic,time,16777215,230814726
basic,time,16777215,227958139
basic,time,16777215,228723995
basic,time,16777215,229286180
$ ./_build/bm_suspend -i
with_suspend                  time                : 13.672
basic                         time                : 13.528
```

In interactive mode, the rate of events per iteration is reported, while in plaintext mode number of iterations and number of events is printed out directly. Time limit for running the benchmark is taking time spent in suspension into account, to make run time predictable. Thus, the way to interpret the output is: 'with_suspend' is equivalent to 'basic' in non-suspended time, thus the time/iteration is very close. However, the suspended activity takes a while, so we had to run fewer iterations overall. 

### Existing counters:
#### Linux perf_events ("lpe:...")
The acronym/prefix used is 'lpe'.
This family of counters uses perf_events interface, same as Linux perf tool. It allows counting performance events either by predefined names for popular counters (cycles, cache-misses, branches, page-faults) or custom CPU-specific raw codes in r<Mask><Event> format. This makes answering questions like 'how many cache misses will different version of the code have?' or
'how different execution ports on CPU are used across several implementation of the algorithm?' much easier compared to building separate binaries, running them with perf tool (or equivalent) drilling down to the function in question, etc. 
  
Example usage:
```
$ ./bm_raw -c lpe:cycles,lpe:r04a1
```

#### Jemalloc thread allocations ("jemalloc_thread_allocated")
This counter tracks the number of bytes allocated by jemalloc in the calling thread. Example usage:
```
$ ./bm_jemalloc -c jemalloc_thread_allocated
```

#### Time ("time")
Default counter, counts microseconds.

## Dependencies and compatibility

B63 requires following C compiler attributes available:
- \_\_attribute\_\_((cleanup))
- \_\_attribute\_\_((used)) 
- \_\_attribute\_\_((section))

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
The library consists of a core part responsible for running the benchmarks, and pluggable counters. The library is header-only, thus, there isn't much encapsulation going on.  Every global symbol is prefixed with b63\_.

Main internal data structures are:
1) b63_benchmark. Each function defined with a 'B63_BENCHMARK' or 'B63_BASELINE' macro corresponds to one benchmark instance.
2) b63_suite. Set of all benchmarks defined in the translation unit.
3) b63_ctype. Counter Type. Defines a type/family of a counter, for example, 'linux_perf_event' or 'jemalloc'
4) b63_counter. Instance of a counter, which has to be of one of the defined counter types. 
5) b63_counter_list. Set of all counters to run benchmarks for.
6) b63_run. Individual benchmark execution.

## Next steps:
- a convenient way to measure outliers. For example, as hash maps usually have amortized O(1) cost for lookup, what does p99 lookup time looks like for some lookup distribution? What can be done to improve?
- paired t-test for comparison;
- support CPU perf counters sources beyond Linux perf_events, for example [Intel's PCM](https://github.com/opcm/pcm) and [BSD pmcstat](https://www.freebsd.org/cgi/man.cgi?query=pmcstat).
- GPU perf counters (at least for Nvidia).
- [low-pri] disk access and network.
