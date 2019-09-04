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

#include <stdint.h>
#include <stdlib.h>
#include <time.h>

B63_BENCHMARK(basic, n) {
  int i = 0, res = 0;
  for (i = 0; i < n; i++) {
    res += rand();
  }
  B63_KEEP(res);
}

B63_BENCHMARK(with_suspend, n) {
  int i = 0, res = 0;
  for (i = 0; i < n; i++) {
    res += rand();
  }
  /* This block will not be counted */
  B63_SUSPEND {
    for (i = 0; i < n; i++) {
      res += rand();
    }
  }
  B63_KEEP(res);
}

int main(int argc, char **argv) {
  srand(time(0));
  B63_RUN(argc, argv);
  return 0;
}
