#pragma once

/**
TODO: 
* emplace (only dummy)
* erase (2x, imcomplete)
* insert (incomplete)

* allocator state
* copy, if move throws
* unify different ctors

*/

#include <initializer_list>

#include "memory.hpp"
#include "stdexcept.hpp"
#include "detail/vector_iterator.hpp"
#include "detail/algorithm.hpp"

#include <tuple>
#include <cassert>
#include <utility> // forward
#include <algorithm> // lexicographical_compare

namespace std2
{
  using std::distance;
  using std::initializer_list;
  using std::iterator_traits;
  using std::lexicographical_compare;
  using std::make_tuple;
  using std::move;
  using std::numeric_limits;
  using std::reverse_iterator;
  using std::size_t;
  using std::swap;
  using std::tie;
  using std::tuple;

  template<typename T , class Allocator = allocator<T> >
  class vector
  {
  public:
    // types:
    /*
    typedef T value_type;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef size_t size_type; // see 23.2 // implementation-defined!
    typedef ptrdiff_t difference_type;// see 23.2 // implementation-defined
    /*/
    // use the defined types instead of implementation-defined!
    typedef typename Allocator::value_type value_type;
    typedef typename Allocator::reference reference;
    typedef typename Allocator::const_reference const_reference;
    typedef typename Allocator::size_type size_type;
    typedef typename Allocator::difference_type difference_type;
    //*/
    
    /*
    // use pointer as simple iterators
    typedef T* iterator; // see 23.2 // implementation-defined!
    typedef const T*  const_iterator; // see 23.2 // implementation-defined!
    /*/
    // use iterator class
    typedef detail::vector_iterator<vector> iterator; // see 23.2 // implementation-defined!
    typedef const detail::vector_iterator<vector>  const_iterator; // see 23.2 // implementation-defined!
    //*/

    typedef Allocator allocator_type;
    typedef typename allocator_traits<Allocator>::pointer pointer;
    typedef typename allocator_traits<Allocator>::const_pointer const_pointer;
    typedef typename std::reverse_iterator<iterator> reverse_iterator;
    typedef typename std::reverse_iterator<const_iterator> const_reverse_iterator;
 
    // 23.3.6.2, construct/copy/destroy:
    vector() noexcept(noexcept(Allocator())) : vector(Allocator()) { }
    explicit vector(const Allocator&) noexcept {} // missing state of allocator
    explicit vector(size_type n, const Allocator& = Allocator())
    {
      _realloc(n, n, [] { return T(); });
    }
    vector(size_type n, const T& value , const Allocator& = Allocator())
    {
      _realloc(n, n, [value] { return value; });
    }
    
    template <class InputIterator 
      //, class = typename enable_if<!is_same<typename iterator_traits<InputIterator>::value_type, void>::value>::type>
      , class = typename iterator_traits<InputIterator>::value_type>
      //>
    vector(InputIterator first, InputIterator last /*, const Allocator& = Allocator()*/)
    {      
      _realloc2(first, last, [](InputIterator it) { return *it; });
    }
    
    vector(const vector& other)
      : vector(other.begin(), other.end())
    {
    }
    
    vector(vector&& other) noexcept
    {
      std::swap(begin_, other.begin_);
      std::swap(end_, other.end_);
      std::swap(capacity_, other.capacity_);
    }

    vector(const vector& other, const Allocator&)
      : vector(other.begin(), other.end())
    {
    }

    vector(vector&& other, const Allocator&)
    {
      swap(other);
    }
    
    vector(initializer_list<T> il, const Allocator& = Allocator())
    {
      _realloc2(il.begin(), il.end(), [](typename initializer_list<T>::iterator it) { return *it; });
    }
    
    ~vector()
    {
      _shrink(0);
      _dealloc(begin_, end_, capacity_);
    }
    
    vector& operator=(const vector& x)
    {
      // exception save, no self reference check, 
      // but slower / no reuse of memory
      vector v(x);
      swap(v);
      return *this;
    }

    vector& operator=(vector&& x) /*noexcept(allocator_traits<Allocator>::propagate_on_container_move_assignment::value || allocator_traits<Allocator>::is_always_equal::value);*/
    {
      vector v(move(x));
      swap(v);
      return *this;
    }

    vector& operator=(initializer_list<T> il)
    {
      vector v(il);
      swap(v);
      return *this;
    }

    template <class InputIterator
      //, class = typename enable_if<!is_same<typename iterator_traits<InputIterator>::value_type, void>::value>::type>
      , class = typename iterator_traits<InputIterator>::value_type>
      //>
    void assign(InputIterator first, InputIterator last)
    {
      vector v(first, last);
      swap(v);
    }

    void assign(size_type n, const T& u)
    {
      vector v(n, u);
      swap(v);
    }

    void assign(initializer_list<T> il)
    {
      vector v(il);
      swap(v);
    }
    
    allocator_type get_allocator() const noexcept { return _current_allocator(); }
    
    // ...
    // iterators:
    iterator begin() noexcept
    {
      return iterator(begin_);
    }
    
    const_iterator begin() const noexcept
    {
      return const_iterator(begin_);
    }
    
    iterator end() noexcept
    {
      return iterator(end_);
    }
    
    const_iterator end() const noexcept
    {
      return const_iterator(end_);
    }
    
    reverse_iterator rbegin() noexcept
    {
      if (begin_ == end_) return reverse_iterator(iterator(end_)); // also nullptr check
      return reverse_iterator(iterator(end_));
    }
    const_reverse_iterator rbegin() const noexcept
    {
      if (begin_ == end_) return const_reverse_iterator(const_iterator(end_)); // also nullptr check
      return const_reverse_iterator(const_iterator(end_));
    }
    reverse_iterator rend() noexcept
    {
      if (begin_ == end_) return reverse_iterator(iterator(begin_)); // also nullptr check
      return reverse_iterator(iterator(begin_));
    }
    const_reverse_iterator rend() const noexcept
    {
      if (begin_ == end_) return const_reverse_iterator(const_iterator(begin_)); // also nullptr check
      return const_reverse_iterator(const_iterator(begin_));
    }

    const_iterator cbegin() const noexcept
    {
      return const_iterator(begin_);
    }
    
    const_iterator cend() const noexcept
    {
      return const_iterator(end_);
    }
    const_reverse_iterator crbegin() const noexcept
    {
      if (begin_ == end_) return const_reverse_iterator(const_iterator(end_)); // also nullptr check
      return const_reverse_iterator(const_iterator(end_));
    }
    const_reverse_iterator crend() const noexcept
    {
      if (begin_ == end_) return const_reverse_iterator(const_iterator(begin_)); // also nullptr check
      return const_reverse_iterator(const_iterator(begin_));
    }

    // 23.3.6.3, capacity:
    bool empty() const noexcept
    {
      return begin_ == end_;
    }

    size_type size() const noexcept
    {
      return end_ - begin_; // implicit conversion from ptr_diff_type (signed) to site_t (unsigned)
    }

    size_type max_size() const noexcept
    {
      // TODO: call allocate::max_size()
      return numeric_limits<size_type>::max() / sizeof(T);
    }

    size_type capacity() const noexcept
    {
      return capacity_ - begin_;
    }

    void resize(size_type n)
    {
      if (n == size()) return;

      // 0 <= n < size();
      if (_shrink(n)) return;

      // size() < n <= capacity();
      if (_expand(n, [] { return T(); })) return;

      // n > capacity => _alloc memory
      _realloc(n, n, [] { return T(); });
    }

    void resize(size_type n, const T& c)
    {
      // 0 <= n < size();
      if (_shrink(n)) return;

      // size() < n <= capacity();
      if (_expand(n, [] { return T(); })) return;

      // n > capacity => _alloc memory
      _realloc(n, n, [&c] { return c; });
    }

    void reserve(size_type n)
    {
      if (capacity() >= n) return;

      //if (n > max_size()) throw length_error("vector: requested size is bigger then max_size()"); 

      _realloc(size(), n, [] { assert(!"don't default initialization"); return T(); });
    }

    void shrink_to_fit()
    {
      if (capacity() == size()) return;

      _realloc(size(), size(), [] { assert(!"don't default initialization"); return T(); });
    }

    // element access:
    reference operator[](size_type n)
    {
      return *(begin_ + n);
    }
    const_reference operator[](size_type n) const
    {
      return *(begin_ + n);
    }
    const_reference at(size_type n) const
    {
      if (n >= size()) throw out_of_range("out_of_range: vector");
      return *(begin_ + n);
    }
    reference at(size_type n)
    {
      if (n >= size()) throw out_of_range("out_of_range: vector");
      return *(begin_ + n);
    }
    reference front()
    {
      return *begin_;
    }
    const_reference front() const
    {
      return *begin_;
    }
    
    reference back()
    {
      return *(end_ -1);
    }
    const_reference back() const
    {
      return *(end_ -1);
    }
    
    // 23.3.6.4, data access
    T* data() noexcept
    {
      return begin_;
    }
    const T* data() const noexcept
    {
      return begin_;
    }
    
    // 23.3.6.5, modifiers:
    template <class... Args> void emplace_back(Args&&... args)
    {
      const auto n = size() + 1;
      // size() < n <= capacity();
      if (n <= capacity()) {
        _current_allocator().construct(begin_ + n, std::forward<Args>(args)...);
      }

      // n > capacity => _alloc memory
      const auto new_capacity = size() + size() / 2 + 1;
      _realloc3(new_capacity, std::forward<Args>(args)...);

    }
    void push_back(const T& c)
    {
      /*
      const auto n = size() + 1; 
      // size() < n <= capacity();
      if (_expand(n, [&c] { return c; })) return;

      // n > capacity => _alloc memory
      _realloc(n, n, [&c] { return c; });
      /*/
      const auto n = size() + 1; 
      // size() < n <= capacity();
      if (_expand(n, [&c] { return c; })) return;

      // n > capacity => _alloc memory
      const auto new_capacity = size() + size() / 2 + 1; 
      _realloc(n, new_capacity, [&c] { return c; });
      //*/
    }
    
    void push_back(T&& c)
    {
      /*
      const auto n = size() + 1; 
      // size() < n <= capacity();
      if (_expand(n, [&c] { return c; })) return;

      // n > capacity => _alloc memory
      _realloc(n, n, [&c] { return c; });
      /*/
      const auto n = size() + 1; 
      // size() < n <= capacity();
      if (_expand(n, [&c] { return move(c); })) return;

      // n > capacity => _alloc memory
      const auto new_capacity = size() + size() / 2 + 1; 
      _realloc(n, new_capacity, [&c] { return move(c); });
      //*/
    }
    
    void pop_back()
    {
      resize(size()-1);
    }
    
    template <class... Args>
    iterator emplace(const_iterator position, Args&&... args)
    {
      return insert(position, args...);
      //return iterator(begin_);
    }

    // _move2 -> overlapping ranges, move from end
    // 1. move, if T::move don't throw or T::not_copyable
    // 2. fall back to copy (not implemented yet)
    void _move2(const pointer xstart, const pointer xend, const pointer ystart)
    {
      for (pointer p1 = xend - 1, p2 = ystart + (xend - xstart) - 1; p1 >= xstart; --p1, --p2) {
        *p2 = move(*p1); // TODO use copy if move throws
      }
    }

    template<typename ITER, typename INIT>
    iterator _insert(const_iterator position, ITER first, ITER last, INIT init_elem)
    {
      const size_type pos = position - iterator(begin_); // store position pointer independent for return value
      const auto n = last - first;
      const size_type new_size = size() + n;

      if (new_size <= capacity()) {
        // no realloc, should not throw (missing test for )
        _move2(begin_ + pos, end_, begin_ + pos + n);
        pointer p = begin_ + pos;
        for (ITER it = first; it < last; ++it, ++p) {
          _current_allocator().construct(p, init_elem(it));
        }
        end_ = begin_ + new_size; // add n elements
      }
      else {
        // need realloc
        //*
        T *b{ nullptr }, *e{ nullptr }, *c{ nullptr };

        tie(b, c) = _alloc(new_size);
        e = b;
        mini_guart guart = { b, e, c, _current_allocator() };

        // copy all elements 0 - pos-1
        for (pointer p1 = begin_, p2 = b; p1 < begin_ + pos; ++p1, ++p2) {
          _current_allocator().construct(p2, *p1);
          e = std::max(e, p2 + 1);
        }        

        // copy all element pos - oldsize to pos+1 - oldsize+1
        for (pointer p1 = begin_ + pos, p2 = b + pos + n; p1 < end_; ++p1, ++p2) {
          _current_allocator().construct(p2, *p1);
          e = std::max(e, p2 + 1);
        }

        pointer p = b + pos;
        for (ITER it = first; it < last; ++it, ++p) {
          _current_allocator().construct(p, init_elem(it));
          e = std::max(e, p + 1);
        }

        std::swap(b, begin_);
        std::swap(e, end_);
        std::swap(c, capacity_);
      }
      return iterator(begin_ + pos);
    }

    iterator insert(const_iterator position, const T& x)
    {
      return _insert(position, 0, 1, [&x] (size_type) { return x; });
    }

    iterator insert(const_iterator position, T&& x)
    {
      return _insert(position, 0, 1, [&x] (size_type) { return move(x); });
    }

    iterator insert(const_iterator position, size_type n, const T& x)
    {
      return _insert(position, size_type(0), n, [&x] (size_type) { return x; });
    }

    template <class InputIterator, class = typename iterator_traits<InputIterator>::value_type >
    iterator insert(const_iterator position, InputIterator first, InputIterator last)
    {
      return _insert(position, first, last, [](InputIterator it) { return *it; });
    }
    
    iterator insert(const_iterator position, initializer_list<T> il)
    {
      return _insert(position, il.begin(), il.end(), [] (typename initializer_list<T>::iterator it) { return *it; });
    }

    iterator erase(const_iterator position)
    {
      const size_type pos = position - iterator(begin_); // store position pointer independent for return value

      for (pointer p = begin_ + pos + 1; p != end_; ++p) {
        std::swap(*(p - 1), *p);
      }
      _current_allocator().destroy(--end_);

      return iterator(begin_ + pos);

    }
    
    iterator erase(const_iterator first, const_iterator last)
    {
      const size_type pos1 = first - iterator(begin_); // store position pointer independent for return value
      const size_type pos2 = last - iterator(begin_); 

      pointer p1 = begin_ + pos1;
      pointer p2 = begin_ + pos2;

      for (; p2 != end_; ++p1, ++p2) {
        std::swap(*p1, *p2);
      }

      for (size_type i = 0; i < size_type(p2 - p1); ++i) {
        --end_;
        _current_allocator().destroy(end_);
      }

      return iterator(begin_ + pos1);
    }

    void swap(vector& other) //noexcept(allocator_traits<Allocator>::propagate_on_container_swap::value ||
                             //allocator_traits<Allocator>::is_always_equal::value);
    {
      std::swap(begin_, other.begin_);
      std::swap(end_, other.end_);
      std::swap(capacity_, other.capacity_);
    }

    
    void clear() noexcept
    {
      _shrink(0);
    }

  private:
    T* begin_ = nullptr;
    T* end_ = nullptr;
    T* capacity_ = nullptr;

    allocator_type& _current_allocator()
    {
      static allocator_type alloc;
      return alloc;
    }

    tuple<T*, T*> _alloc(size_type n)
    {
      if (_current_allocator().max_size() <= n) throw length_error("vector: requested size is bigger then max_size()");

      T* b = n ? _current_allocator().allocate(n) : nullptr;
      T* c = n ? b + n : nullptr; // is nullptr + 0 a valid operation?

      return make_tuple(b, c);
    }

    void _dealloc(T*& b, T*&e, T*& c)
    {
      _current_allocator().deallocate(b, capacity());
      c = e = b = nullptr;
    }
    
    // internal implementation:

    template<typename INIT>
    void _realloc(size_type new_size, size_type new_capacity, INIT init_elem)
    {
      T *b{ nullptr }, *e{ nullptr }, *c{ nullptr };

      tie(b, c) = _alloc(new_capacity);
      e = b;
      mini_guart guart = { b, e, c, _current_allocator() };

      // copy/move existing values
      for (size_type i = 0; i < size(); ++i) {
        // b[i] = move(begin_[i]); only if T::copy-ctor == notrow...
        _current_allocator().construct(b + i, begin_[i]);
        e = b + i + 1;
      }

      // init new add values
      for (size_type i = size(); i < new_size; ++i) {
        _current_allocator().construct(b + i, init_elem());
        e = b + i + 1;
      }

      std::swap(b, begin_);
      std::swap(e, end_);
      std::swap(c, capacity_);
    }


    template<typename ITER, typename INIT>
    void _realloc2(ITER first, ITER last, INIT init_elem)
    {
      T *b{ nullptr }, *e{ nullptr }, *c{ nullptr };

      const auto new_size = size() + distance(first, last);

      tie(b, c) = _alloc(new_size);
      e = b + new_size;
      mini_guart guart = { b, e, c, _current_allocator() };

      // copy/move existing values
      for (size_type i = 0; i < size(); ++i) {
        // b[i] = move(begin_[i]); only if T::copy-ctor == notrow...
        _current_allocator().construct(b + i, begin_[i]);
        e = b + i + 1;
      }

      // init new add values
      pointer p = b + size();
      for (ITER it = first; it != last; ++it, ++p) {
        _current_allocator().construct(p, init_elem(it));
        e = p+ 1;
      }

      std::swap(b, begin_);
      std::swap(e, end_);
      std::swap(c, capacity_);
    }

    /*
    special case for emplace_back!
    */
    template <class... Args>
    void _realloc3(size_type new_capacity, Args&&... args)
    {
      const size_type old_size = size();
      T *b{ nullptr }, *e{ nullptr }, *c{ nullptr };

      tie(b, c) = _alloc(new_capacity);
      e = b;
      mini_guart guart = { b, e, c, _current_allocator() };

      // copy/move existing values
      for (size_type i = 0; i < size(); ++i) {
        // b[i] = move(begin_[i]); only if T::copy-ctor == notrow...
        _current_allocator().construct(b + i, begin_[i]);
        e = b + i + 1;
      }

      // init new add values
      _current_allocator().construct(e, std::forward<Args>(args)...);
      ++e;

      std::swap(b, begin_);
      std::swap(e, end_);
      std::swap(c, capacity_);
    }


    bool _shrink(size_type n) noexcept
    {
      if (n < size()) {
        for (pointer p = begin_+n; p != end_; ++p) {
          _current_allocator().destroy(p);
        }
        end_ = begin_ + n;
        return true;
      }
      return false;
    }

    // don't realloc, size() < n <= capacity()
    // return false, if realloc necessary 
    template<typename INIT>
    bool _expand(size_type n, INIT init_elem)
    {
      assert(n >= size());

      if (n <= capacity()) {
        for (size_type i = size(); i < n; i++) {
          _current_allocator().construct(begin_ + i, init_elem());
        }
        end_ = begin_ + n;
        return true;
      }
      return false;
    }

    struct mini_guart
    {
      ~mini_guart() 
      { 
        for (T* p = b; p != e; ++p)
          allocator.destroy(p);
        allocator.deallocate(b, c-b); 
      }
      T*& b;
      T*& e;
      T*& c;
      allocator_type& allocator;
    };
  };


  

  template <class T, class Allocator>
  bool operator==(const vector<T, Allocator>& x, const vector<T, Allocator>& y)
  {
    // use std algorithms as in standard
    // return distance(x.begin(), x.end()) == distance(y.begin(), y.end())
    //  && std::equal(x.begin(), x.end(), y.begin()); // 'std::_Equal2': Function call with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct. To disable this warning, use -D_SCL_SECURE_NO_WARNINGS. See documentation on how to use Visual C++ 'Checked Iterators'

    return x.size() == y.size() && detail::_equal(x.begin(), x.end(), y.begin(), y.end());
  }
  template <class T, class Allocator>
  bool operator< (const vector<T ,Allocator>& x, const vector<T,Allocator>& y)
  {
    return lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
  }
  template <class T, class Allocator>
  bool operator!=(const vector<T, Allocator>& x, const vector<T, Allocator>& y)
  {
    return !(x == y);
  }

  template <class T, class Allocator>
  bool operator> (const vector<T, Allocator>& x, const vector<T, Allocator>& y)
  {
    return y < x;
  }
  
  template <class T, class Allocator>
  bool operator>=(const vector<T, Allocator>& x, const vector<T, Allocator>& y)
  {
    return !(x < y);
  }

  template <class T, class Allocator>
  bool operator<=(const vector<T, Allocator>& x, const vector<T, Allocator>& y)
  {
    return !(x > y);
  }

  template <class T, class Allocator>
  void swap(vector<T ,Allocator>& x, vector<T ,Allocator>& y)
  {
    x.swap(y);
  }

  //template <class /*Allocator*/> class vector<bool,Allocator>;  

  // hash support
  //template <class T> struct hash;
  //template <class Allocator> struct hash<vector<bool, Allocator> >;
}
