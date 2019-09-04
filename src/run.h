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

#ifndef _B63_RUN_H_
#define _B63_RUN_H_

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

#include "benchmark.h"
#include "printer.h"
#include "suite.h"
#include "utils/section_ptr_list.h"
#include "utils/timer.h"

#include "counters/time.h"

/* Runs individual benchmark for several epochs. */
static void b63_benchmark_run(b63_benchmark *b) {
  /* total runtime including suspended time for each benchmark */
  const int64_t epoch_timelimit_ms =
      1000LL * b->suite->timelimit_s / b->suite->epochs;
  b63_counter *counter = b->suite->counter;

  const int64_t max_iterations_per_epoch = (1LL << 31LL);

  /* print out status */
  b63_print_start(b);

  int64_t epoch, n;
  for (epoch = 0; epoch < b->suite->epochs; epoch++) {
    /* init context */
    b63_run r;
    r.events = 0LL;
    r.iterations = 0LL;
    r.suspension_done = 0;
    r.benchmark = b;
    int64_t started, done = 0LL;
    /*
     * For each epoch run as many iterations as fit within the time budget
     */
    int64_t epoch_started_ms = b63_now_ms();
    for (n = 1; r.iterations < max_iterations_per_epoch; n *= 2) {

      /* Here the 'measured' function is called */
      started = counter->type->read(counter->impl);
      b->run(&r, n);
      done = counter->type->read(counter->impl);

      r.events += (done - started);

      r.iterations += n;
      if ((b63_now_ms() - epoch_started_ms) > epoch_timelimit_ms) {
        break;
      }
    }

    /*
     * 'best' event rate is picked.
     * Best is defined as 'the fewer events, the better'.
     * More sophisticated approach might be implemented here, for example
     * paired t-test with each epoch being 'sample';
     * this requires us to pass some seed around to make epoch reproducible.
     * Another option is to just print out each epoch, and let something else
     * to handle any analysis if needed.
     */

    /* TODO: can this overflow */
    if (epoch == 0 ||
        r.events * b->result.iterations < b->result.events * r.iterations) {
      b->result = r;
    }
  }
  b63_print_done(b);
}

/* Run all registered benchmarks within a passed suite */
static void b63_suite_run(b63_suite *suite) {
  B63_FOR_EACH_COUNTER(suite->counter_list, counter) {
    suite->baseline = NULL;
    suite->counter = counter;
    /* Run baseline benchmark only. */
    B63_LIST_FOR_EACH(b63_benchmark, b) {
      (*b)->suite = suite;
      if ((*b)->is_baseline) {
        if (suite->baseline != NULL) {
          fprintf(stderr, "two or more baselines defined.\n");
          exit(EXIT_FAILURE);
        }
        suite->baseline = *b;
        b63_benchmark_run(*b);
      }
    }

    /* Run non-baselines */
    B63_LIST_FOR_EACH(b63_benchmark, b) {
      if ((*b)->is_baseline) {
        continue;
      }
      b63_benchmark_run(*b);
    }
  }
}

/* Reads config, creates suite and executes it */
static void b63_go(int argc, char **argv, const char *default_counter) {
  b63_suite suite;

  b63_suite_init(&suite, argc, argv);

  if (suite.counter_list.size == 0) {
    b63_counter_list_init(&suite.counter_list, default_counter);
  }

  b63_suite_run(&suite);

  b63_counter_list_cleanup(&suite.counter_list);
}

/*
 * Runs all registered benchmarks with counter. If '-c' is provided, it
 * overrides the setting in the code.
 * default_counter is a string literal, to allow
 * configuration to be passed easily, for example "lpe:cycles".
 */
#define B63_RUN_WITH(default_counter, argc, argv)                              \
  b63_go(argc, argv, default_counter);

/* shortcut for 'default' timer-based counter. */
#define B63_RUN(argc, argv) B63_RUN_WITH("time", argc, argv)

/*
 * B63_KEEP implements a way to prevent compiler from optimizing out the
 * variable. Example:
 *
 * int res = 0;
 * ... some computations ...
 * B63_KEEP(res);
 */
#define B63_KEEP(v) __asm__ __volatile__("" ::"m"(v))

/*
 * B63_SUSPEND allows to 'exclude' the counted events (or time)
 * from part of the code. It is more convenient way to do set up, for example:
 *
 * B63_BENCHMARK(sort_benchmark, n) {
 *   std::vector<int> a;
 *   B63_SUSPEND { // this block will be excluded
 *     a.resize(n);
 *     std::generate(a.begin(), a.end(), gen); // some random generator
 *   }
 *   std::sort(a.begin(), a.end());
 * }
 */

typedef struct b63_suspension {
  int64_t start;
  b63_run *run;
} b63_suspension;

/*
 * This is a callback to execute when suspend context gets out of scope.
 * Total number of events during 'suspension loop' is accumulated.
 */
static void b63_suspension_done(b63_suspension *s) {
  s->run->events -= (s->run->benchmark->suite->counter->type->read(
                         s->run->benchmark->suite->counter->impl) -
                     s->start);
}

/*
 * Use __attribute__((cleanup)) to trigger the measurement when sctx goes out of
 * scope. This allows to compute 'how many events happened during suspension'.
 */
#define B63_SUSPEND                                                            \
  b63run->suspension_done = 0;                                                 \
  for (b63_suspension b63s __attribute__((cleanup(b63_suspension_done))) =     \
           {                                                                   \
               .start = b63run->benchmark->suite->counter->type->read(         \
                   b63run->benchmark->suite->counter->impl),                   \
              .run = b63run,                                                   \
           };                                                                  \
       b63run->suspension_done == 0; b63run->suspension_done = 1)

#endif
