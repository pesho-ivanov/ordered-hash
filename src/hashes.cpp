#include <cstdio>
#include <string>
#include <vector>
#include <algorithm>
#include <set>

#include <cassert>
#include <ctime>
#include <typeinfo>
#include "memory_check.h"

#include "ordered_hash.h"
#include "ordered_hash_faster.h"
using namespace std;

const int MAX_INT     = int(1e9);

typedef int                         my_key_t;
typedef string                      my_data_t;
//typedef string                      my_key_t;
//typedef int                         my_data_t;
typedef pair<my_key_t, my_data_t>   my_value_t;

typedef ordered_hash<my_key_t, my_data_t>         ordered_hash_t;
typedef ordered_hash_faster<my_key_t, my_data_t>  ordered_hash_faster_t;
typedef map<my_key_t, my_data_t>                  map_t;
typedef unordered_map<my_key_t, my_data_t>        hash_t;

string gen_string(int len) {
  string s;

  for(int i=0; i<len; i++)
    s.push_back('A' + rand()%26 + (rand()%2 ? 0 : ('a' - 'A')));

  return s;
}

int gen_int() {
  return rand() % MAX_INT;
}

void gen_test(int n, int len, vector<my_value_t> &sample, int seed) {
  set<my_key_t> S;
  srand(seed);

  for(int i=0; i<n; i++) {
    int key;
    string data;

    do {
      key  = gen_int();
      //key  = gen_string(len);
    } while (S.find(key) != S.end());
    
    S.insert(key);

    data = gen_string(len);
    //data = gen_int();
    sample.push_back( my_value_t(key, data) );
  }
}

inline double delay(clock_t start_time) {
  return 1.0 * (clock() - start_time) / CLOCKS_PER_SEC;
}

template<typename T>
string get_type() {
  if (typeid(T).name() == typeid(ordered_hash_t).name())
    return "ordered_hash";

  if (typeid(T).name() == typeid(ordered_hash_faster_t).name())
    return "ordered_hash_faster";
  
  if (typeid(T).name() == typeid(map_t).name())
    return "map";

  if (typeid(T).name() == typeid(hash_t).name())
    return "hash";

  return "other";
}

template<typename T>
inline void print_benchmark(int n, int len) {
  printf("%13s elements: %10d, key_len: %8d\n", get_type<T>().c_str(), n, len);
}

template<typename T>
inline void print_delay(int n, string what, double dt) {
  printf("%5.2lf s, %8.0lf %s/s\n", dt, 1.0*n/dt, what.c_str());
}

void print_memory() {
  double vm, rss;
  process_mem_usage(vm, rss);
  printf(" VM: %.0lfmb; RSS: %.0lfmb --> ", vm/1024.0, rss/1024);
}

template<typename T>
void insert_all(T &my, const vector<my_value_t> &sample, int seed) {
  srand(seed);

  int n = sample.size();
  assert((int)my.size() == 0);

  clock_t start_time = clock();
  for(auto t : sample)
    my.insert(t);
  double dt = delay(start_time);

  assert((int)my.size() == n);
  print_delay<T>(n, "ins", dt);
}

template<typename T>
void find_all(T &my, const vector<my_value_t> &sample, int seed) {
  srand(seed);

  int n = sample.size();
  assert((int)my.size() == n);

  clock_t start_time = clock();
  for(auto t : sample)
    my.find(t.first);
  double dt = delay(start_time);

  assert((int)my.size() == n);
  print_delay<T>(n, "fnd", dt);
}

template<typename T>
void trace_all(T &my) {
  int n = my.size();

  clock_t start_time = clock();
  for(auto t : my)
    ;
  double dt = delay(start_time);

  print_delay<T>(n, "inc", dt);
}

template<typename T>
void erase_all(T &my, vector<my_value_t> sample, int seed) {
  srand(seed);

  int n = sample.size();
  random_shuffle(sample.begin(), sample.end());
  assert((int)my.size() == n);

  clock_t start_time = clock();
  for(auto t : sample)
    my.erase(t.first);
  double dt = delay(start_time);

  assert((int)my.size() == 0);
  print_delay<T>(n, "era", dt);
}

template<typename T>
void benchmark(int n, int len, int seed) {
  T my;

  vector<my_value_t> sample;
  gen_test(n, len, sample, seed);
  print_benchmark<T>(n, len);

  print_memory();
  insert_all(my, sample, seed);
  print_memory();
  find_all  (my, sample, seed);
  print_memory();
  trace_all (my);
  print_memory();
  erase_all (my, sample, seed);
}

template<typename T1, typename T2>
void verify(int n, int len, int seed) {
  T1 my1;
  T2 my2;

  vector<my_value_t> sample;
  gen_test(n, len, sample, seed);

  for(auto t : sample) {
    my1.insert(t);
    my2.insert(t);
    assert( equal(my1.begin(), my1.end(), my2.begin()) );
  }

  //assert(my1.find("alabalanica") == my1.end());
  //assert(my2.find("alabalanica") == my2.end());
  for(auto t : sample) {
    assert(my1.find(t.first) != my1.end());
    assert(my1.find(t.first)->first == t.first);
    assert(my2.find(t.first) != my2.end());
    assert(my2.find(t.first)->first == t.first);
  }

  random_shuffle(sample.begin(), sample.end());
  for(auto t : sample) {
    my1.erase(t.first);
    my2.erase(t.first);
    assert( equal(my1.begin(), my1.end(), my2.begin()) );
  }
}

void compare(int n, int len, int seed) {
  benchmark< map_t >                  (n, len, seed);
  benchmark< ordered_hash_t >         (n, len, seed);
  benchmark< ordered_hash_faster_t >  (n, len, seed);
  benchmark< hash_t >                 (n, len, seed);
}

int main() {
  verify<map_t, ordered_hash_t>(200, 10, 42-1);
  //verify<map_t, ordered_hash_faster_t>(200, 10, 42-1);

  //compare( 1000000,   10, 42+0 );
  //compare( 1000000,  100, 42+1 );
  compare( 1000000,    500, 42+3 );
  
  return 0;
}
