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

#ifndef _B63_PRINTER_
#define _B63_PRINTER_

#include "benchmark.h"
#include "suite.h"

#include <inttypes.h>
#include <stdio.h>

const char *B63_CLR_RED = "\033[0;31m";
const char *B63_CLR_GREEN = "\033[0;32m";
const char *B63_CLR_RESET = "\033[0m";

/* Called when benchmark starts. In case of plaintext, does nothing. */
static void b63_print_start(b63_benchmark *b) {
  if (b->suite->printer_config.plaintext != 0) {
    return;
  }
  printf("[....] %-30s: %s", b->name, b->suite->counter->name);
  fflush(stdout);
}

/* Called when benchmark is finished. */
static void b63_print_done(b63_benchmark *b) {
  /* plaintext output */
  if (b->suite->printer_config.plaintext != 0) {
    char sep = b->suite->printer_config.separator;
    printf("%s%c%s%c%" PRId64 "%c%" PRId64 "\n", b->name, sep,
           b->suite->counter->name, sep, b->result.iterations, sep,
           b->result.events);
    fflush(stdout);
    return;
  }
  double events_rate = 1.0 * b->result.events / b->result.iterations;

  /* no baseline defined in the suite */
  if (b->suite->baseline == NULL) {
    printf("\r[DONE] %-30s: %s : %lf per iteration\n", b->name,
           b->suite->counter->name, events_rate);
    fflush(stdout);
    return;
  }

  /* this benchmark is a baseline */
  if (b->is_baseline) {
    printf("\r[DONE] %-30s: %s : %lf per iteration (baseline)\n", b->name,
           b->suite->counter->name, events_rate);
    fflush(stdout);
    return;
  }

  /* there's another baseline and we print comparison */
  double baseline_events_rate = 1.0 * b->suite->baseline->result.events /
                                b->suite->baseline->result.iterations;
  printf("\r");
  if (events_rate <= baseline_events_rate) {
    /* not worse than baseline */
    printf("%s[DONE]%s %-30s: %s : %lf per iteration (%s-%.3lf%%%s)\n",
           B63_CLR_GREEN, B63_CLR_RESET, b->name, b->suite->counter->name,
           events_rate, B63_CLR_GREEN,
           100 - 100.0 * events_rate / baseline_events_rate, B63_CLR_RESET);
  } else {
    /* worse than baseline */
    printf("%s[DONE]%s %-30s: %s : %lf per iteration (%s+%.3lf%%%s)\n",
           B63_CLR_RED, B63_CLR_RESET, b->name, b->suite->counter->name,
           events_rate, B63_CLR_RED,
           100.0 * events_rate / baseline_events_rate - 100, B63_CLR_RESET);
  }
  fflush(stdout);
}

#endif
