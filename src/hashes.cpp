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

// TODO: delete
typedef ordered_hash<int, string>         ordered_hash_t;
typedef ordered_hash_faster<int, string>  ordered_hash_faster_t;
typedef map<int, string>                  map_t;
typedef unordered_map<int, string>        hash_t;

template<typename T>
T gen(int len) {
  assert(false);
  return T();
}

template<>
string gen<string>(int len) {
  string s;

  for(int i=0; i<len; i++)
    s.push_back('A' + rand()%26 + (rand()%2 ? 0 : ('a' - 'A')));

  return s;
}

template<>
int gen(int len) {
  if (len) ;
  return rand() % MAX_INT;
}

template<typename T>
void gen_test(int n, int len,
    vector< pair<typename T::key_type, typename T::mapped_type> > &sample,
    int seed) {
  typedef typename T::key_type    key_type;
  typedef typename T::mapped_type mapped_type;

  set<key_type> S;
  srand(seed);

  for(int i=0; i<n; i++) {
    key_type key;
    mapped_type data;

    do {
      key  = gen<key_type>(len);
    } while (S.find(key) != S.end());
    
    S.insert(key);

    data = gen<mapped_type>(len);
    sample.push_back( make_pair(key, data) );
  }
}

inline double delay(clock_t start_time) {
  return 1.0 * (clock() - start_time) / CLOCKS_PER_SEC;
}

template<typename T>
string get_type() {
  string name = typeid(T).name();

  if (name == typeid(ordered_hash_t).name()) return "ordered_hash";
  if (name == typeid(ordered_hash_faster_t).name()) return "ordered_hash_faster";
  if (name == typeid(map_t).name()) return "map";
  if (name == typeid(hash_t).name()) return "hash";
  if (name == typeid(string).name()) return "string";
  if (name == typeid(int).name()) return "int";

  return "other: " + name;
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
void insert_all(T &my,
    const vector< pair<typename T::key_type, typename T::mapped_type> > &sample,
    int seed) {
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
void find_all(T &my,
    const vector< pair<typename T::key_type, typename T::mapped_type> > &sample,
    int seed) {
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
void erase_all(T &my,
    vector< pair<typename T::key_type, typename T::mapped_type> > &sample,
    int seed) {
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

  vector< pair<typename T::key_type, typename T::mapped_type> > sample;
  gen_test<T>(n, len, sample, seed);
  printf("  Class %s\n", get_type<T>().c_str());

  print_memory();
  insert_all<T>(my, sample, seed);
  print_memory();
  find_all<T>  (my, sample, seed);
  print_memory();
  trace_all<T> (my);
  print_memory();
  erase_all<T> (my, sample, seed);

  printf("\n");
}

template<typename T1, typename T2>
void verify(int n, int len, int seed) {
  T1 my1;
  T2 my2;

  vector< pair<typename T1::key_type, typename T1::mapped_type> > sample;
  gen_test<T1>(n, len, sample, seed);

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

template<typename key_t, typename mapped_t>
void compare(int n, int len, int seed) {
  printf(" --- Comparison of %d %s -> %s mappings, len=%d ---\n", 
      n, get_type<key_t>().c_str(), get_type<mapped_t>().c_str(), len);

  benchmark< map<key_t, mapped_t> >                  (n, len, seed);
  benchmark< ordered_hash<key_t, mapped_t> >         (n, len, seed);
  benchmark< ordered_hash_faster<key_t, mapped_t> >  (n, len, seed);
  benchmark< unordered_map<key_t, mapped_t> >        (n, len, seed);
}

int main() {
  verify<map_t, ordered_hash_t>(200, 10, 42-1);
  //verify<map_t, ordered_hash_faster_t>(200, 10, 42-1);

  //compare( 1000000,   10, 42+0 );
  //compare( 1000000,  100, 42+1 );
  compare<int, string>( 1000000,    100, 42+3 );
  compare<string, int>( 1000000,    100, 42+3 );
  
  return 0;
}
