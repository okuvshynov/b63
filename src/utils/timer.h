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

#ifndef _B63_UTILS_TIME_H_
#define _B63_UTILS_TIME_H_

#include <time.h>

/*
 * This is used NOT for benchmarking itself, but to track
 * time passed and control the number of iterations.
 * See src/run.h -> b63_run_benchmark implementation for usage;
 */
int64_t b63_now_ms() {
  struct timespec t;
  clock_gettime(CLOCK_MONOTONIC, &t);
  int64_t res = 1000 * t.tv_sec + t.tv_nsec / 1000000;
  return res;
}

#endif
