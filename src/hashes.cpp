#include <cstdio>
#include <string>
#include <vector>
#include <algorithm>

#include <cassert>
#include <ctime>
#include <typeinfo>

#include "ordered_hash.h"
using namespace std;

const int S_LEN       = 15;
const int MAX_INT     = int(1e9);

typedef string                      my_key_t;
typedef int                         my_data_t;
typedef pair<my_key_t, my_data_t>   my_value_t;

typedef ordered_hash<my_key_t, my_data_t>   ordered_hash_t;
typedef map<my_key_t, my_data_t>            map_t;
typedef unordered_map<my_key_t, my_data_t>  hash_t;

template<typename T>
T gen() {
  assert(false);
  return T();
}

template<>
string gen<string>() {
  string s;

  for(int i=0; i<S_LEN; i++)
    s.push_back('A' + rand()%26 + (rand()%2 ? 0 : ('a' - 'A')));

  return s;
}

template<>
int gen<int>() {
  return rand() % MAX_INT;
}

void gen_test(int n, vector<my_value_t> &sample, int seed) {
  srand(seed);

  for(int i=0; i<n; i++) {
    auto key  = gen<typename my_value_t::first_type>();
    auto data = gen<typename my_value_t::second_type>();
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
  
  if (typeid(T).name() == typeid(map_t).name())
    return "map";

  if (typeid(T).name() == typeid(hash_t).name())
    return "hash";

  return "other";
}

template<typename T>
inline void print_time(int n, string what, double dt) {
  printf("%12s %10d %s, %5.2lf s, %8.0lf %s/s\n",
      get_type<T>().c_str(), n, what.c_str(), dt, 1.0*n/dt, what.c_str());
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
  print_time<T>(n, "ins", dt);
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
  print_time<T>(n, "fnd", dt);
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
  print_time<T>(n, "era", dt);
}

template<typename T>
void benchmark(int n, int seed) {
  T my;

  vector<my_value_t> sample;
  gen_test  ( n, sample, seed);

  insert_all(my, sample, seed);
  find_all  (my, sample, seed);
  erase_all (my, sample, seed);
}

template<typename T1, typename T2>
void verify(int n, int seed) {
  T1 my1;
  T2 my2;

  vector<my_value_t> sample;
  gen_test(n, sample, seed);

  for(auto t : sample) {
    my1.insert(t);
    my2.insert(t);
    assert( equal(my1.begin(), my1.end(), my2.begin()) );
  }

  assert(my1.find("alabalanica") == my1.end());
  assert(my2.find("alabalanica") == my2.end());
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

void compare(int n, int seed) {
  benchmark< map_t >           (n, seed);
  benchmark< ordered_hash_t >  (n, seed);
  benchmark< hash_t >          (n, seed);
}

int main() {
  verify<map_t, ordered_hash_t>(200, 42+1);

  compare(400000, 42 + 0);
//  compare(1000000, 42 + 1);
  
  return 0;
}
