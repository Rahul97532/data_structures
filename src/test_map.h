#pragma once
#include <iostream>
#include <cassert>
#include <cstdlib>
#include <ctime>
#include <climits>

namespace gtl {

/*
 * Smoketest class for map implementations
 */
template <typename T> struct smoketest_map {
public:
  void smoketest();
  void value_semantics();

private:
  void search();
};

template <typename T>
void smoketest_map<T>::smoketest() {
  T map;
  assert(map.size() == 0);
  assert(!map.contains_key(1));
  assert(map.add(1, 100));
  assert(map.size() == 1);
  assert(map.contains_key(1));
  int * value = map.lookup(1);
  assert(*value == 100);
  assert(!map.add(1, 200));
  int * new_value = map.lookup(1);
  assert(*new_value == 200);
  assert(map.remove(1));
  assert(!map.remove(1));
  assert(!map.remove(14));
  assert(map.size() == 0);
  int * empty_value = map.lookup(1);
  assert(empty_value == nullptr);
  search();
}

template <typename T>
void smoketest_map<T>::search() {
  T map;
  int MAX = 10;
  for (size_t i = 0; i < MAX*MAX; ++i) {
    int element = rand() % MAX;
    map.add(element, element);
  }
  size_t steps = 2;
  size_t x = map.capacity() - steps + 1;
  size_t last_step;
  for (size_t i = 1; i <= steps; ++i) {
    last_step = x + map.capacity()*i;
    map.add(last_step, last_step);
  }
  assert(map.contains_key(last_step));
}

template <typename T>
void value_semantics_memory_test(T & obj) {
  /*
   * Memcheck type has to be used in order to run this test
   */
  size_t old_counter = memcheck::get_counter();
  T obj2(obj);
  assert(memcheck::get_counter() == old_counter*2);
  T obj3(std::move(obj));
  assert(memcheck::get_counter() == old_counter*2);
  T obj4;
  obj4 = obj3;
  assert(memcheck::get_counter() == old_counter*3);
  T obj5;
  obj5 = std::move(obj3);
  assert(memcheck::get_counter() == old_counter*3);
}

template <typename T>
void smoketest_map<T>::value_semantics() {
  T vect_map;
  size_t n = 100;
  for (size_t i = 1; i <= n; ++i) {
    memcheck el;
    vect_map.add(i, el);
  }
  value_semantics_memory_test(vect_map);
}

/*
 * Comparison test class for map implementations
 */
template <typename T, typename U> struct map_comparison_test {
  void compare_random_queries();
};

template <typename T, typename U>
void map_comparison_test<T, U>::compare_random_queries() {
  T map1;
  U map2;
  int MAX = 1000;
  for (size_t i = 0; i < MAX*MAX; ++i) {
    int element = rand() % MAX;
    assert(map1.add(element, element) == map2.add(element, element));
    assert(map1.add(element, element*2) == map2.add(element, element*2));
  }
  for (size_t i = 0; i < MAX/2; ++i) {
    int element = rand() % MAX;
    assert(map1.remove(element) == map2.remove(element));
  }
  for (size_t i = 0; i < MAX*2; ++i) {
    int element = rand() % MAX;
    if (bool(map1.lookup(element))) {
      assert(*map1.lookup(element) == *map2.lookup(element));
    } else {
      assert(map1.lookup(element) == map2.lookup(element));
    }
    assert(map1.contains_key(element) == map2.contains_key(element));
  }
}

/*
 *  Benchmarking for map operations
 */
size_t max_number = 8000;
size_t n_operations = 1000;

template <typename T> struct addition
{
  void operator() (T & map) {
    for (size_t i = 0; i < 14*n_operations; ++i) {
      map.add(rand() % max_number, 0);
    }
  }
};

template <typename T> struct deletion
{
  void operator() (T & map) {
    for (size_t i = 0; i < 14*n_operations; ++i) {
      map.remove(rand() % max_number);
    }
  }
};

template <typename T> struct search
{
  void operator() (T const & map) {
    for (size_t i = 0; i < 14*n_operations; ++i) {
      map.contains_key(rand() % max_number);
    }
  }
};

template <typename T> struct mixed
{
  void operator() (T & map) {
    for (size_t i = 0; i < 10*n_operations; ++i) {
      map.lookup(rand() % max_number);
    }
    for (size_t i = 0; i < 3*n_operations; ++i) {
      int el = rand() % max_number;
      map.add(el, 0);
    }
    for (size_t i = 0; i < n_operations; ++i) {
      int el = rand() % max_number;
      map.remove(el);
    }
  }
};

template <typename T, typename U>
clock_t benchmark_operation(T & map) {
  U op;
  size_t n_runs = 100;
  clock_t min_time = LONG_MAX;
  for (size_t i = 0; i < n_runs; ++i) {
    clock_t start_time = std::clock();
    op(map);
    clock_t end_time = std::clock();
    clock_t t = (end_time - start_time) % CLOCKS_PER_SEC;
    if (t < min_time) min_time = t;
  }
  return min_time;
}

template <typename T>
std::string benchmark() {
  T map;
  return
    std::to_string(benchmark_operation<T, addition<T>>(map)) + " " +
    std::to_string(benchmark_operation<T, search<T>>(map)) + " " +
    std::to_string(benchmark_operation<T, mixed<T>>(map)) + " " +
    std::to_string(benchmark_operation<T, deletion<T>>(map)) + " " +
    "\n";
}
}
