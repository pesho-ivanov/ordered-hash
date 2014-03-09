#include<unordered_map>
#include<map>
#include<utility>

template <typename _Key, typename _Tp>
class ordered_hash {
 public:
  typedef _Key                                  key_type;
  typedef _Tp                                   data_type;
  typedef std::pair<key_type, data_type>        value_type;

 private:
  //char* comparison
  //struct str_cmp {
  //  bool operator()(const char *s1, const char *s2) {
  //    return strcmp(s1, s2) < 0;
  //  }
  //};

  typedef std::unordered_map<key_type, data_type>   unordered_t;
  typedef std::map<key_type, data_type>             ordered_t;

  unordered_t H;
  ordered_t M;

 public:
  typedef typename unordered_t::const_iterator  const_unordered_iterator;
  typedef typename ordered_t::const_iterator    const_iterator;

  const_iterator begin() { return M.begin(); }
  const_iterator end() { return M.end(); }
      
  void insert(const value_type &val) {
    H.insert(val);
    M.insert(val);
  }

  const_unordered_iterator unordered_find(const key_type &key) {
    return H.find(key);
  }

  const_iterator find(const key_type &key) {
    return M.find(key);
  }

  void erase(const key_type &key) {
    H.erase(key);
    M.erase(key);
  }

  int size() {
    assert(M.size() == H.size());
    return M.size();
  }

  void clear() {
    H.clear();
    M.clear();
  }
};

