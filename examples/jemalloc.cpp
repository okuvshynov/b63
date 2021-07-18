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

#include "../include/b63/counters/jemalloc.h"
#include "../include/b63/b63.h"

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <numeric>
#include <vector>

const size_t kSize = (1 << 16);

B63_BASELINE(allocate, n) {
  std::vector<uint32_t> v;
  v.resize(kSize * n);
  std::iota(v.begin(), v.end(), 0);
  int32_t res = accumulate(v.begin(), v.end(), 0);
  B63_KEEP(res);
}

B63_BENCHMARK(allocate_more, n) {
  std::vector<uint32_t> v;
  v.resize((kSize + 1) * n);
  std::iota(v.begin(), v.end(), 0);
  int32_t res = accumulate(v.begin(), v.end(), 0);
  B63_KEEP(res);
}

int main(int argc, char **argv) {
  srand(time(0));
  B63_RUN_WITH("jemalloc_thread_allocated", argc, argv);
  return 0;
}
