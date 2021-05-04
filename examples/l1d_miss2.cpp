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


#include "../src/b63.h"
#include "../src/counters/perf_events.h"
#include "../src/counters/osx_kperf.h"

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <pthread.h>
#include <numeric>
#include <vector>

const size_t kSize = (1 << 16);
const size_t kMask = kSize - 1;

B63_BASELINE(sequential_icestorm, n) {
  pthread_set_qos_class_self_np(QOS_CLASS_BACKGROUND, 0);
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

B63_BENCHMARK(sequential_icestorm_unrolled, n) {
  pthread_set_qos_class_self_np(QOS_CLASS_BACKGROUND, 0);
  std::vector<uint32_t> v;
  B63_SUSPEND {
    v.resize(kSize);
    std::iota(v.begin(), v.end(), 0);
  }
  int32_t res = 0;
  for (size_t i = 0; i < n; i++) {
    for (size_t j = 0; j < kSize;) {
      res += v[v[j++] & kMask];
      res += v[v[j++] & kMask];
      res += v[v[j++] & kMask];
      res += v[v[j++] & kMask];
    }
  }

  B63_KEEP(res);
}

B63_BENCHMARK(sequential_firestorm, n) {
  pthread_set_qos_class_self_np(QOS_CLASS_USER_INTERACTIVE, 0);
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

B63_BENCHMARK(sequential_firestorm_unrolled, n) {
  pthread_set_qos_class_self_np(QOS_CLASS_USER_INTERACTIVE, 0);
  std::vector<uint32_t> v;
  B63_SUSPEND {
    v.resize(kSize);
    std::iota(v.begin(), v.end(), 0);
  }
  int32_t res = 0;
  for (size_t i = 0; i < n; i++) {
    for (size_t j = 0; j < kSize;) {
      res += v[v[j++] & kMask];
      res += v[v[j++] & kMask];
      res += v[v[j++] & kMask];
      res += v[v[j++] & kMask];
    }
  }

  B63_KEEP(res);
}

B63_BENCHMARK(random_icestorm, n) {
  pthread_set_qos_class_self_np(QOS_CLASS_BACKGROUND, 0);
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

B63_BENCHMARK(random_firestorm, n) {
  pthread_set_qos_class_self_np(QOS_CLASS_USER_INTERACTIVE, 0);
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
