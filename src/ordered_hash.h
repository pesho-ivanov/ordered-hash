#include<unordered_map>
#include<map>
#include<utility>
#include<boost/functional/hash.hpp>

/*
 *               map    ordered_hash   hash/unordered_map
 *    insert:   logN        logN              1
 *     erase:   logN       logN(1?)           1
 *      find:   logN         1                1
 * next/prev:  logN(am)    logN(1?)          n/a(N)
 *    sorted:    N(am)       N(am)         n/a(NlogN)
 *
 *    TODO:    H: key->iterator    ---->    H: key_hash->iterator
 *          add to std
 *          change ordered_hash to ordered_hash_map
 * */

namespace std {

template <typename _Key, typename _Tp>
class ordered_hash {
 public:
  typedef _Key                                key_type;
  typedef _Tp                                 mapped_type;
  typedef std::pair<key_type, mapped_type>    value_type;

 private:
  typedef std::map<key_type, mapped_type>     ordered_t;
  typedef std::unordered_map<
            key_type,
            typename ordered_t::iterator,
            boost::hash<key_type>
          >                                   unordered_t;

  // memory comparison
  // containters
  unordered_t H;
  ordered_t M;

 public:
  typedef typename ordered_t::iterator        iterator;
  typedef typename ordered_t::const_iterator  const_iterator;

  const_iterator begin() { return M.begin(); }
  const_iterator end() { return M.end(); }
      
  void insert(const value_type &val) {
    std::pair<iterator, bool> el = M.insert(val);
    H.insert( make_pair(val.first, el.first) );
  }

  iterator find(const key_type &key) {
    auto it = H.find(key);
    return it != H.end() ? it->second : M.end();
  }

  const_iterator find(const key_type &key) const {
    auto it = H.find(key);
    return it != H.end() ? it->second : M.end();
  }

  mapped_type& operator[](const key_type &key) {
    return M[key];
  }

  // anti code-duplication: doesn't work yet
  //iterator& find(const key_type &key) {
  //  auto &it = static_cast<const ordered_hash &>( *this ).find(key);
  //  return const_cast<iterator &>( it );
  //}

  void erase(const key_type &key) {
    M.erase(key); // this O(logN) line is missing in ordered_hash_faster
    H.erase(key);
  }

  int size() {
    //assert(M.size() == H.size());
    return H.size();
  }

  void clear() {
    H.clear();
    M.clear();
  }
};

} // namespace std
