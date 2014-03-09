#include<unordered_map>
#include<map>
#include<utility>

/*               map    ordered_hash   hash/unordered_map
 *    insert:   logN        logN              1
 *     erase:   logN        logN (1?)         1
 *      find:   logN         1                1
 * next/prev:   logN        logN (1?)         N
 *    sorted:     N          N              NlogN
 * */
template <typename _Key, typename _Tp>
class ordered_hash {
 public:
  typedef _Key                                key_type;
  typedef _Tp                                 data_type;
  typedef std::pair<key_type, data_type>      value_type;

 private:
  typedef std::map<key_type, data_type>       ordered_t;
  typedef std::unordered_map<key_type,
          typename ordered_t::iterator>       unordered_t;

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

  const_iterator find(const key_type &key) {
    auto it = H.find(key);
    return it != H.end() ? it->second : M.end();
  }

  void erase(const key_type &key) {
    M.erase(key); // TODO: not erasing from M
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

