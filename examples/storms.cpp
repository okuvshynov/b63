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
    
    int64_t run() {
        A* curr = &l[0];
        int64_t res = 0;
        for (int64_t r = 0; r < rep_; r++) {
            curr = curr->next;
            if (curr->payload % 3 == 0) {
              res += curr->payload;
            }
        }
        return res;
    }

    int64_t run_unrolled() {
        A* curr = &l[0];
        int64_t res = 0;
        for (int64_t r = 0; r < rep_; r += 4) {
            curr = curr->next;
            if (curr->payload % 3 == 0) {
              res += curr->payload;
            }
            curr = curr->next;
            if (curr->payload % 3 == 0) {
              res += curr->payload;
            }
            curr = curr->next;
            if (curr->payload % 3 == 0) {
              res += curr->payload;
            }
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

B63_BENCHMARK(icestorm, n) {
  pthread_set_qos_class_self_np(QOS_CLASS_BACKGROUND, 0);
  Test* t;
  B63_SUSPEND {
    t = new Test(kSize, n);
  }

  int64_t res = t->run();

  B63_KEEP(res);
  B63_SUSPEND {
    delete t;
  }
}

B63_BENCHMARK(icestorm_unrolled, n) {
  pthread_set_qos_class_self_np(QOS_CLASS_BACKGROUND, 0);
  Test* t;
  B63_SUSPEND {
    t = new Test(kSize, n);
  }

  int64_t res = t->run_unrolled();

  B63_KEEP(res);
  B63_SUSPEND {
    delete t;
  }
}

B63_BASELINE(firestorm, n) {
  pthread_set_qos_class_self_np(QOS_CLASS_USER_INTERACTIVE, 0);
  Test* t;
  B63_SUSPEND {
    t = new Test(kSize, n);
  }

  int64_t res = t->run();

  B63_KEEP(res);
  B63_SUSPEND {
    delete t;
  }
}

B63_BENCHMARK(firestorm_unrolled, n) {
  pthread_set_qos_class_self_np(QOS_CLASS_USER_INTERACTIVE, 0);
  Test* t;
  B63_SUSPEND {
    t = new Test(kSize, n);
  }

  int64_t res = t->run_unrolled();

  B63_KEEP(res);
  B63_SUSPEND {
    delete t;
  }
}

int main(int argc, char **argv) {
  srand(time(0));
  B63_RUN_WITH("time", argc, argv);
  return 0;
}
