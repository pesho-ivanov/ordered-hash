#include<unordered_map>
#include<map>
#include<utility>
#include<boost/functional/hash.hpp>   // hash for STL containers

/*
 *               map      ordered_hash    hash/unordered_map
 *    insert:   logN          logN                1
 *     erase:   logN          logN                1
 *      find:   logN           1                  1
 * next/prev:   logN    logN / 1(amort.)  n/a (possible in N)
 *    sorted: N(amort.)      N(amort.)   n/a (possible in NlogN)
 *
 *    TODO:    H: key->iterator    ---->    H: key_hash->iterator
 *          add to std
 *          change ordered_hash to ordered_hash_map
 * */

namespace std {

template <  typename _Key,
            typename _Tp,
            typename _Compare = std::less<_Key>,
	    typename _Hash = hash<_Key>,
	    typename _Pred = std::equal_to<_Key>,
            typename _Alloc = std::allocator<std::pair<const _Key, _Tp> >
          >
//template <typename _Key, typename _Tp>
class ordered_hash {
 public:
  typedef _Key                                key_type;
  typedef _Tp                                 mapped_type;
  typedef std::pair<key_type, mapped_type>    value_type;
  typedef _Compare                            key_compare;
  typedef _Alloc                              allocator_type;

 private:
  typedef std::map<
            key_type,
            mapped_type,
            key_compare,
            allocator_type
          >                                   ordered_t;

  typedef std::unordered_map<
            key_type,
            typename ordered_t::iterator,
            boost::hash<key_type>
          >                                   unordered_t;

  // containters
  unordered_t H;
  ordered_t M;

 public:
  // TODO
  //typedef typename _Pair_alloc_type::pointer         pointer;
  //typedef typename _Pair_alloc_type::const_pointer   const_pointer;
  //typedef typename _Pair_alloc_type::reference       reference;
  //typedef typename _Pair_alloc_type::const_reference const_reference;

  typedef typename ordered_t::iterator                iterator;
  typedef typename ordered_t::const_iterator          const_iterator;
  typedef typename ordered_t::size_type               size_type;
  typedef typename ordered_t::difference_type         difference_type;
  typedef typename ordered_t::reverse_iterator        reverse_iterator;
  typedef typename ordered_t::const_reverse_iterator  const_reverse_iterator;

  ordered_hash()
    : H(), M() {}

  //TODO
  //explicit ordered_hash(ordered_t &x) {
  //}

  ordered_hash(const ordered_hash &x)
    : H(x.H), M(x.M) { }

  ordered_hash& operator=(const ordered_hash& x) {
    H = x.H;
    M = x.M;
    return *this;
  }

  // begin & end iterators
  iterator begin() { return M.begin(); }
  const_iterator begin() const { return M.begin(); }
  iterator end() { return M.end(); }
  const_iterator end() const { return M.end(); }
      
  // inserts
  pair<iterator, bool> insert(const value_type &val) {
    std::pair<iterator, bool> el = M.insert(val);
    H.insert( make_pair(val.first, el.first) );
    return el;
  }

  iterator insert (iterator position, const value_type& val) {
    iterator it = M.insert(position, val);
    H.insert( make_pair(val.first, it) );
    return it;
  }

  template <class InputIterator>
  void insert (InputIterator first, InputIterator last) {
    for(InputIterator it = first; it != last; it++)
      H.insert(*it);
    return M.insert(first, last);
  }

  // finds
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
  
  // lower bound
  iterator lower_bound(const key_type& x) {
    return M.lower_bound(x);
  }

  const_iterator lower_bound(const key_type& x) const {
    return M.lower_bound(x);
  }

  // upper bound
  iterator upper_bound(const key_type& x) {
    return M.upper_bound(x);
  }

  const_iterator upper_bound(const key_type& x) const {
    return M.upper_bound(x);
  }

  // equal range
  std::pair<iterator, iterator> equal_range(const key_type& x) {
    return M.equal_range(x);
  }

  std::pair<const_iterator, const_iterator> equal_range(const key_type& x) const {
    return M.equal_range(x);
  }

  // erases
  void erase(iterator position) {
    H.erase(*position);
    return M.erase(position);
  }

  size_type erase(const key_type &key) {
    H.erase(key);
    return M.erase(key); // this O(logN) line is missing in ordered_hash_faster
  }

  void erase(iterator first, iterator last) {
    for(iterator it = first; it != last; it++)
      H.erase(*it);
    M.erase(first, last);
  }

  // other
  size_type count(const key_type& x) const {
    return M.find(x) == M.end() ? 0 : 1;
  }

  void swap(ordered_hash& x) {
    H.swap(x.H);
    M.swap(x.M);
  }

  bool empty() const {
    return M.empty();
  }

  size_type size() const {
    //assert(M.size() == H.size());
    return M.size();
  }

  size_type max_size() const {
    return M.max_size(); // this estimate could be lowered
  }

  void clear() {
    H.clear();
    M.clear();
  }

  key_compare key_comp() const {
    return M.key_comp();
  }

  //TODO
  //value_compare value_comp() const {
  //  return value_compare(_M_t.key_comp());
  //}
};

} // namespace std
