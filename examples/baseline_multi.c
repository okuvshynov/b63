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

B63_BASELINE(basic, n) {
  int i = 0, res = 0;
  for (i = 0; i < n; i++) {
    res += rand();
  }
  B63_KEEP(res);
}

B63_BENCHMARK(basic_half, n) {
  int i = 0, res = 0;
  for (i = 0; i < n; i += 2) {
    res += rand();
  }
  B63_KEEP(res);
}

B63_BENCHMARK(basic_twice, n) {
  int i = 0, res = 0;
  for (i = 0; i < n * 2; i++) {
    res += rand();
  }
  B63_KEEP(res);
}

B63_BENCHMARK(basic_5x_more, n) {
  int i = 0, res = 0;
  for (i = 0; i < 5 * n; i++) {
    res += rand();
  }
  B63_KEEP(res);
}

B63_BENCHMARK(basic_5x_less, n) {
  int i = 0, res = 0;
  for (i = 0; i < n; i += 5) {
    res += rand();
  }
  B63_KEEP(res);
}

B63_BENCHMARK(basic_20_percent_more, n) {
  int i = 0, res = 0;
  for (i = 0; i < 6 * n; i += 5) {
    res += rand();
  }
  B63_KEEP(res);
}

B63_BENCHMARK(basic_20_percent_less, n) {
  int i = 0, res = 0;
  for (i = 0; i < 4 * n; i += 5) {
    res += rand();
  }
  B63_KEEP(res);
}

B63_BENCHMARK(basic_10_percent_more, n) {
  int i = 0, res = 0;
  for (i = 0; i < 11 * n; i += 10) {
    res += rand();
  }
  B63_KEEP(res);
}

B63_BENCHMARK(basic_10_percent_less, n) {
  int i = 0, res = 0;
  for (i = 0; i < 9 * n; i += 10) {
    res += rand();
  }
  B63_KEEP(res);
}

B63_BENCHMARK(basic_same, n) {
  int i = 0, res = 0;
  for (i = 0; i < n; i++) {
    res += rand();
  }
  B63_KEEP(res);
}

int main(int argc, char **argv) {
  B63_RUN(argc, argv);
  return 0;
}
