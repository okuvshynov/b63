#
# Copyright 2019 Oleksandr Kuvshynov
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

bm_basic:
	mkdir -p _build/
	cc -Wall -Wno-unused-function basic.c -I. -O3 -o _build/bm_basic -std=c99 -lm
	./_build/bm_basic -i

bm_assert:
	mkdir -p _build/
	cc -Wall -Wno-unused-function assert.c -I. -O3 -o _build/bm_assert -std=c99 -lm
	./_build/bm_assert -i

bm_suspend:
	mkdir -p _build/
	cc -Wall -Wno-unused-function suspend.c -I. -O3 -o _build/bm_suspend -std=c99 -lm
	./_build/bm_suspend -i

bm_baseline:
	mkdir -p _build/
	cc -Wall -Wno-unused-function baseline.c -I. -O3 -o _build/bm_baseline -std=c99 -lm
	./_build/bm_baseline -i

bm_baseline_multi:
	mkdir -p _build/
	cc -Wall -Wno-unused-function baseline_multi.c -I. -O3 -o _build/bm_baseline_multi -std=c99 -lm
	./_build/bm_baseline_multi -i

bm_custom:
	mkdir -p _build/
	cc -Wall -Wno-unused-function custom.c -I. -O3 -o _build/bm_custom -std=c99 -lm
	./_build/bm_custom -i

bm_l1d_miss:
	mkdir -p _build/
	c++ -Wall -Wno-unused-function l1d_miss.cpp -I. -O3 -o _build/bm_l1d_miss -std=c++17
	./_build/bm_l1d_miss -i -c lpe:L1-dcache-load-misses,time

bm_storms:
	mkdir -p _build/
	clang++ -Wall -Wno-unused-function storms.cpp -I. -O3 -o _build/storms -std=c++17 
	sudo ./_build/storms -i -c kperf:L1-dcache-load-misses,time,kperf:cycles,kperf:instructions

bm_l1d_miss_osx2:
	mkdir -p _build/
	clang++ -Wall -Wno-unused-function l1d_miss2.cpp -I. -O3 -o _build/bm_l1d_miss -std=c++17 
	sudo ./_build/bm_l1d_miss -i -c kperf:L1-dcache-load-misses,time,kperf:cycles

bm_l1d_miss_osx:
	mkdir -p _build/
	c++ -Wall -Wno-unused-function l1d_miss.cpp -I. -O3 -o _build/bm_l1d_miss -std=c++17
	sudo ./_build/bm_l1d_miss -i -c kperf:L1-dcache-load-misses,time,kperf:cycles

bm_raw:
	mkdir -p _build/
	cc -Wall -Wno-unused-function raw.c -I. -O3 -o _build/bm_raw -std=c99 -lm
	./_build/bm_raw -c lpe:r04A1,lpe:r10A1 -i

bm_jemalloc_non_bsd:
	mkdir -p _build/
	c++ jemalloc.cpp -O3 -o _build/bm_jemalloc -I`jemalloc-config --includedir` -L`jemalloc-config --libdir` -Wl,-rpath,`jemalloc-config --libdir` -ljemalloc `jemalloc-config --libs`
	./_build/bm_jemalloc -i

bm_jemalloc_bsd:
	mkdir -p _build/
	c++ jemalloc.cpp -O3 -o _build/bm_jemalloc
	./_build/bm_jemalloc -i

bm_indirect:
	mkdir -p _build/
	c++ -Wall -Wno-unused-function indirect.cpp -I. -O3 -o _build/indirect -std=c++17
	./_build/indirect -i -c time


clean:
	rm -rf _build/

format:
	find . -iname *.h -o -iname *.c -o -iname *.cpp | xargs clang-format -i
