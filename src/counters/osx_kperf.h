/*
 * Copyright 2021 Oleksandr Kuvshynov
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

/*
 * Based on the work by 
 * D. Lemire, Duc Tri Nguen and Dougall Johnson
 * References:
 *   - https://dougallj.github.io/applecpu/firestorm.html
 *   - https://github.com/lemire/Code-used-on-Daniel-Lemire-s-blog/tree/master/2021/03/24
 */

#ifndef _B63_COUNTERS_OSX_KPERF_H_
#define _B63_COUNTERS_OSX_KPERF_H_

#ifdef __APPLE__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

#include "../counter.h"

#define B63_KPERF_FUNCTIONS                                                    \
  F(uint32_t, kpc_get_counter_count, uint32_t)                                 \
  F(uint32_t, kpc_get_config_count, uint32_t)                                  \
  F(int, kpc_force_all_ctrs_set, int)                                          \
  F(int, kpc_set_counting, uint32_t)                                           \
  F(int, kpc_set_thread_counting, uint32_t)                                    \
  F(int, kpc_set_config, uint32_t, void *)                                     \
  F(int, kpc_get_thread_counters, int, unsigned int, void *)

#define F(ret, name, ...)                                                      \
  typedef ret name##proc(__VA_ARGS__);                                         \
  static name##proc *name;
B63_KPERF_FUNCTIONS
#undef F

#define KPC_CLASS_FIXED (0)
#define KPC_CLASS_CONFIGURABLE (1)
#define KPC_CLASS_FIXED_MASK (1u << KPC_CLASS_FIXED)
#define KPC_CLASS_CONFIGURABLE_MASK (1u << KPC_CLASS_CONFIGURABLE)
#define KPC_MASK (KPC_CLASS_CONFIGURABLE_MASK | KPC_CLASS_FIXED_MASK)

#define B63_KPERF_COUNTER_SIZE 10
#define B63_KPERF_CONFIG_SIZE 8

struct b63_kperf_counter_event_map {
  const char *event_name;
  uint64_t config;
} b63_kperf_counter_events_flat_map[] = {
  {"cycles", 0x02},
  {"instructions", 0x8c},
  {"branches", 0x8d},
  {"L1-dcache-load-misses", 0xbf},
  {"L1-dcache-store-misses", 0xc0},
  {"dTLB-load-misses", 0xc1},
  {"branch-misses", 0xcb},
  {"L1-icache-load-misses", 0xd3},
  {"iTLB-load-misses", 0xd4},
};

static uint64_t b63_kperf_pick_event(const char* event_name) {
  for (size_t i = 0; i < sizeof(b63_kperf_counter_events_flat_map) /
                             sizeof(struct b63_kperf_counter_event_map);
       ++i) {
    if (strcmp(b63_kperf_counter_events_flat_map[i].event_name, event_name) == 0) {
      return b63_kperf_counter_events_flat_map[i].config;
    }
  }
  return 0;
}

typedef struct b63_counter_kperf {
  uint64_t counters[B63_KPERF_COUNTER_SIZE];
  uint64_t config[B63_KPERF_CONFIG_SIZE];
} b63_counter_kperf;

static int8_t b63_counter_kperf_create(const char* conf, void **impl) {
  void *kperf = dlopen(
      "/System/Library/PrivateFrameworks/kperf.framework/Versions/A/kperf",
      RTLD_LAZY);
  if (!kperf) {
    fprintf(stderr, "kperf library not loaded\n");
    return 0;
  }
#define F(ret, name, ...)                                                      \
  name = (name##proc *)(dlsym(kperf, #name));                                  \
  if (!name) {                                                                 \
    fprintf(stderr, "%s = %p\n", #name, (void *)name);                         \
    return 0;                                                                  \
  }
  B63_KPERF_FUNCTIONS
#undef F

  if (kpc_get_counter_count(KPC_MASK) != B63_KPERF_COUNTER_SIZE) {
    printf("wrong fixed counters count\n");
    return -1;
  }

  if (kpc_get_config_count(KPC_MASK) != B63_KPERF_CONFIG_SIZE) {
    printf("wrong fixed config count\n");
    return -1;
  }

  b63_counter_kperf *res = (b63_counter_kperf *)malloc(sizeof(b63_counter_kperf));
  if (res == NULL) {
    fprintf(stderr, "memory allocation failed for kperf counter\n");
    return 0;
  }

  const char *event_name = conf + strlen("kperf:");
  uint64_t event = b63_kperf_pick_event(event_name);
  if (event == 0) {
    fprintf(stderr, "event %s not found", conf);
    return 0;
  }

  const uint64_t CFGWORD_EL0A64EN_MASK = 0x20000;
  res->config[4] = event | CFGWORD_EL0A64EN_MASK;

  *impl = res;
  return 1;
}

static void b63_counter_kperf_cleanup(void* impl) {
  if (impl != NULL) {
    /* TODO: unload the dl library? */
  }
}

/* this function will be called right before the measurements for 
 * this counter. It is needed to support multiple counters from kperf set,
 * for example, both l1d misses and dtlb misses. Apple's kperf doesn't seem to
 * multiplex/maintain internal state, thus we can only configure 
 * a constant set of counters to measure. 
 */
static void b63_counter_kperf_activate(void* impl) {
  if (impl != NULL) {
    b63_counter_kperf *kperf = (b63_counter_kperf *)impl;
    if (kpc_set_config(KPC_MASK, kperf->config)) {
      fprintf(stderr, "kpc_set_config failed\n");
      return;
    }

    if (kpc_force_all_ctrs_set(1)) {
      fprintf(stderr, "kpc_force_all_ctrs_set failed\n");
      return;
    }

    if (kpc_set_counting(KPC_MASK)) {
      fprintf(stderr, "kpc_set_counting failed\n");
      return;
    }

    if (kpc_set_thread_counting(KPC_MASK)) {
      fprintf(stderr, "kpc_set_thread_counting failed\n");
      return;
    }
  }
}

/* impl is 'passed' implicitly */
B63_COUNTER(kperf, b63_counter_kperf_create, b63_counter_kperf_cleanup, b63_counter_kperf_activate) {
  b63_counter_kperf *kperf = (b63_counter_kperf *)impl;
  if (kpc_get_thread_counters(0, B63_KPERF_COUNTER_SIZE, kperf->counters)) {
    fprintf(stderr, "kpc_get_thread_counters failed\n");
  }
  return kperf->counters[4 + 2];
}

#endif
#endif
