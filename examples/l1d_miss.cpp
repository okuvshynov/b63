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
#include "../include/b63/counters/osx_kperf.h"

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <numeric>
#include <vector>

const size_t kSize = (1 << 16);
const size_t kMask = kSize - 1;

B63_BASELINE(sequential, n) {
  std::vector<uint32_t> v;
  B63_SUSPEND {
    v.resize(kSize);
    std::iota(v.begin(), v.end(), 0);
  }
  int32_t res = 0;
  for (size_t i = 0; i < n; i++) {
    for (size_t j = 0; j < kSize; j++) {
      res += v[v[j] & kMask];
    }
  }

  B63_KEEP(res);
}

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
  B63_RUN_WITH("time", argc, argv);
  return 0;
}
