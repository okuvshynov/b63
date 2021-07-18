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

#include <stdint.h>
#include <stdlib.h>
#include <time.h>

int n = 0;
int64_t callcount = 0LL;

/* in this example custom counter is used. */
B63_COUNTER(calls) { return callcount; }

static int f() {
  callcount++;
  return n++;
}

B63_BASELINE(call_normal, n) {
  int i = 0, res = 0;
  for (i = 0; i < n; i++) {
    res += f();
  }
  B63_KEEP(res);
}

B63_BENCHMARK(call_twice, n) {
  int i = 0, res = 0;
  for (i = 0; i < n; i++) {
    res += (f() + f());
  }
  B63_KEEP(res);
}

int main(int argc, char **argv) {
  B63_RUN_WITH("calls", argc, argv);
  return 0;
}
