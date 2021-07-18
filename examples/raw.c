/*
 * Copyright 2019 Oleksandr Kuvshynov
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "../include/b63/b63.h"
#include "../include/b63/counters/perf_events.h"

#include <stdint.h>
#include <stdlib.h>
#include <time.h>

/*
 * This example illustrates using raw events from processor PMU.
 * In Linux perf_events that's achieved by passing combination
 * of 'mask' and 'event'. Those are CPU-specific, so, this example
 * might need to be run differently or not work at all on different cpus.
 * Still, it provides a decent illustration on how to use them (and why).
 *
 * In b63, -c lpe:r<mask><event> needs to be passed as command-line argument
 * in order to read counter value.
 * For example, on old Core2 Intel processor from 2009:
 *
 * mask = 0x01 && event = 0xA1 corresponds to 'uops executed on port 0'
 * mask = 0x02 && event = 0xA1 corresponds to 'uops executed on port 1'
 * mask = 0x04 && event = 0xA1 corresponds to 'uops executed on port 2'
 * mask = 0x08 && event = 0xA1 corresponds to 'uops executed on port 3'
 * mask = 0x10 && event = 0xA1 corresponds to 'uops executed on port 4'
 * mask = 0x20 && event = 0xA1 corresponds to 'uops executed on port 5'
 *
 * memory load is executed on port 2 and memory (data) store on port 4.
 *
 * Thus, running this benchmark produces result like this:
 *
   $ examples/_build/bm_raw -c lpe:r04A1 -i
   Running 2 benchmarks
   [DONE] many_writes                    : 32.571429 events per iteration
   [DONE] many_reads                     : 285749.285714 events per iteration

   $ examples/_build/bm_raw -c lpe:r10A1 -i
   Running 2 benchmarks
   [DONE] many_writes                    : 500017.428571 events per iteration
   [DONE] many_reads                     : 10.000000 events per iteration

 *
 * For more information on 'which instructions run where' and 'which codes
 correspond to which events' please refer to:
 * - Intel Optimization manual:
 https://software.intel.com/en-us/download/intel-64-and-ia-32-architectures-optimization-reference-manual
 * - Agner Fog's manual, specifically 'microarchitecture' volume:
 https://www.agner.org/optimize/#manuals
 *
 */
const int32_t kSize = 500000;
const int32_t kLookups = 50000;

B63_BENCHMARK(many_reads, n) {
  int32_t *v = NULL;
  int32_t i = 0, res = 0, j;
  B63_SUSPEND {
    v = malloc(kSize * sizeof(int32_t));
    for (i = 0; i < kSize; i++) {
      v[i] = rand();
    }
  }
  for (j = 0; j < n; j++)
    for (i = 0; i < kLookups; i++) {
      res += v[i];
    }
  B63_KEEP(res);
}

B63_BENCHMARK(many_writes, n) {
  int32_t *v = NULL;
  int32_t i = 0, res = 0, j;
  B63_SUSPEND {
    v = malloc(kSize * sizeof(int32_t));
    for (i = 0; i < kSize; i++) {
      v[i] = rand();
    }
  }
  for (j = 0; j < n; j++)
    for (i = 0; i < kLookups; i++) {
      v[i] = res;
      res += i + j;
    }
  B63_KEEP(res);
}

int main(int argc, char **argv) {
  srand(time(0));
  B63_RUN(argc, argv);
  return 0;
}
