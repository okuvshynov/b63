#include "../include/b63/b63.h"
#include "../include/b63/counters/perf_events.h"
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
  /*
   * This call starts benchmarking.
   * Comma-separated list of counters to measure is passed explicitly here,
   * but one can provide command-line flag -c to override.
   * In this case, we are measuring 3 counters:
   *  * lpe:cycles - CPU cycles spent in benchmark outside of B63_SUSPEND, as
   * measured with Linux perf_events;
   *  * lpe:L1-dcache-load-misses - CPU L1 Data cache misses during benchmark
   * run outside of B63_SUSPEND;
   *  * time - wall time outside of B63_SUSPEND.
   */
  B63_RUN_WITH("time,lpe:cycles,lpe:L1-dcache-load-misses", argc, argv);
  return 0;
}
