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

#include "math.h"

B63_BENCHMARK(basic_w_assert_fail, n) {
  int64_t res = 0LL;
  for (int64_t i = 0; i < n; i++) {
    res += i;
  }
  B63_ASSERT(res == n * n);
}

B63_BASELINE(basic_w_assert_b, n) {
  double res = 0;
  for (int64_t i = 0; i < n; i++) {
    for (int64_t x = 1; x < 1234567; x++) {
      /* ln(2) series */
      res += (x % 2 == 1 ? 1.0 : -1.0) / x;
    }
  }
  B63_ASSERT(fabs(res - log(2.0) * n) <= 1e-4);
}

B63_BENCHMARK(basic_w_assert, n) {
  double res = 0;
  for (int64_t i = 0; i < n; i++) {
    for (int64_t x = 1; x < 1234567; x++) {
      /* ln(2) series */
      res += (x % 2 == 1 ? 1.0 : -1.0) / x;
    }
  }
  B63_ASSERT(fabs(res - log(2.0) * n) <= 1e-4);
}

int main(int argc, char **argv) {
  B63_RUN(argc, argv);
  return 0;
}
