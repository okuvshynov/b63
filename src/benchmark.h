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

#ifndef _B63_DATA_H_
#define _B63_DATA_H_

#include "counter_list.h"

struct b63_run;
struct b63_benchmark;
struct b63_sink;
struct b63_suite;

/* This structure represents indivudual benchmark run. */
typedef struct b63_run {
  struct b63_benchmark *benchmark;
  int64_t iterations;
  int64_t events;
  int8_t suspension_done;
} b63_run;

/* benchmarked function template, it needs to support 'run n iterations' */
typedef void (*b63_target_fn)(struct b63_run *, uint64_t);

/*
 * This struct represents individual benchmark.
 */
typedef struct b63_benchmark {
  /* benchmark name */
  const char *name;
  /* function to benchmark */
  const b63_target_fn run;
  /* is this benchmark a baseline? */
  const int8_t is_baseline;
  /* [weak] pointer to suite config */
  struct b63_suite *suite;

  /* final result */
  b63_run result;
} b63_benchmark;

#endif
