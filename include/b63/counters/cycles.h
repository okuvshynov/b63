#ifndef _B63_COUNTERS_CYCLES_H_
#define _B63_COUNTERS_CYCLES_H_

#include <stdint.h>

#ifdef __APPLE__
#ifdef __ARM64_ARCH_8__

B63_COUNTER(cycles) {
  uint64_t res;
  __asm__ volatile("mrs %0, cntvct_el0" : "=r" (res));
  return res;
}

#endif
#endif

#endif
