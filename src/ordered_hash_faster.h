#include <unordered_map>
#include <map>
#include <utility>
#include <boost/iterator/filter_iterator.hpp>

/*               map  ordered_hash_faster  hash/unordered_map
 *    insert:   logN        logN              1
 *     erase:   logN         1                1
 *      find:   logN         1                1
 * next/prev:  logN(am)    logN(1?)          n/a(N)
 *    sorted:    N(am)       N(am)         n/a(NlogN)
 * */
template <typename _Key, typename _Tp>
class ordered_hash_faster {
 public:
  typedef _Key                                key_type;
  typedef _Tp                                 data_type;
  typedef std::pair<key_type, data_type>      value_type;

 private:
  typedef std::map<key_type, data_type>               ordered_t;
  typedef typename ordered_t::iterator                full_iterator;
  typedef std::unordered_map<key_type, full_iterator> unordered_t;

  class is_in_hash {
    unordered_t * const _H;

   public:
    is_in_hash(unordered_t &__H) : _H(&__H) {}

    bool operator()(const value_type &x) {
      return _H->find(x.first) != _H->end();
    }
  };
  
  // containters
  unordered_t H;
  ordered_t M;

  // predicate for erased elements
  is_in_hash predicate;

 public:
  ordered_hash_faster() : predicate(H) {}

  typedef boost::filter_iterator<is_in_hash, full_iterator> iterator;
  
  iterator begin() { return iterator(predicate, M.begin(), M.end()); }
  iterator end() { return iterator(predicate, M.end(), M.end()); }
      
  void insert(const value_type &val) {
    std::pair<full_iterator, bool> el = M.insert(val);
    H.insert( make_pair(val.first, el.first) );
  }

  iterator find(const key_type &key) {
    typename unordered_t::iterator it = H.find(key);

    if (it != H.end())
      return iterator(predicate, it->second, M.end());
    else
      return iterator(predicate, M.end(), M.end());
  }

  void erase(const key_type &key) {
    H.erase(key);
  }

  int size() {
    return H.size();
  }

  void clear() {
    H.clear();
    M.clear();
  }
};

