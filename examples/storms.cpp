#include "../src/b63.h"
#include "../src/counters/osx_kperf.h"

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <pthread.h>
#include <random>
#include <vector>

struct A {
    A* next;
    int64_t payload;
};

constexpr bool is_pow2(int a) {
    return a && ((a & (a - 1)) == 0);
}

struct Test {
    Test(int64_t len, int64_t rep) : rep_(rep * len) {
        l.resize(len);
        for (int64_t i = 0; i < len; i++) {
            l[i].payload = i;
        }
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(l.begin(), l.end(), g);

        for (int64_t i = 0; i + 1 < len; i++) {
            l[i].next = &l[i+1];
        }
        l[len - 1].next = &l[0];

        std::sort(l.begin(), l.end(), [](const A& a, const A& b){
            return a.payload < b.payload;
        });
    }
    
    template<int unroll=2>
    int64_t run() {
        static_assert(is_pow2(unroll), "unroll factor must be power of 2");
        A* curr = &l[0];
        int64_t res = 0;
        #pragma clang loop unroll_count(unroll)
        for (int64_t r = 0; r < rep_; r++) {
            curr = curr->next;
            if (curr->payload % 3 == 0) {
              res += curr->payload;
            }
        }
        return res;
    }

    private:
        std::vector<A> l;
        int64_t rep_;
};

const size_t kSize = (1 << 10);

#define BM_UNROLLED(name, qos, unroll)     \
  B63_BENCHMARK(name##_u##unroll, n) {     \
    pthread_set_qos_class_self_np(qos, 0); \
    Test* t;                               \
    B63_SUSPEND {                          \
      t = new Test(kSize, n);              \
    }                                      \
    int64_t res = t->run<unroll>();        \
    B63_KEEP(res);                         \
    B63_SUSPEND {                          \
      delete t;                            \
    }                                      \
  }

#define FIRESTORM_UNROLLED(unroll)         \
  BM_UNROLLED(firestorm, QOS_CLASS_USER_INTERACTIVE, unroll)
#define ICESTORM_UNROLLED(unroll)          \
  BM_UNROLLED(icestorm, QOS_CLASS_BACKGROUND, unroll)

FIRESTORM_UNROLLED(1)
FIRESTORM_UNROLLED(2)
FIRESTORM_UNROLLED(4)
FIRESTORM_UNROLLED(8)
FIRESTORM_UNROLLED(16)
FIRESTORM_UNROLLED(32)
FIRESTORM_UNROLLED(64)
FIRESTORM_UNROLLED(128)

ICESTORM_UNROLLED(1)
ICESTORM_UNROLLED(2)
ICESTORM_UNROLLED(4)
ICESTORM_UNROLLED(8)
ICESTORM_UNROLLED(16)
ICESTORM_UNROLLED(32)
ICESTORM_UNROLLED(64)
ICESTORM_UNROLLED(128)

int main(int argc, char **argv) {
  srand(time(0));
  B63_RUN_WITH("time", argc, argv);
  return 0;
}
