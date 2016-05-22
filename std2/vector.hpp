#pragma once

/**
TODO: 
* insert (incomplete)

* allocator state
* copy, if move throws
*/

#include <initializer_list>

#include "memory.hpp"
#include "stdexcept.hpp"
#include "detail/vector_iterator.hpp"
#include "detail/algorithm.hpp"

#include <cassert>

#include <algorithm> // lexicographical_compare

namespace std2
{
  using std::distance;
  using std::initializer_list;
  using std::iterator_traits;
  using std::lexicographical_compare;
  using std::move;
  using std::numeric_limits;
  using std::reverse_iterator;
  using std::size_t;
  using std::swap;

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
    explicit vector(const Allocator& alloc = Allocator())
      : alloc_(alloc) {}
    explicit vector(size_type n)
      : vector(n, T{}, Allocator{}) {}
    explicit vector(size_type n, const T& value, const Allocator& allocator = Allocator())
      : vector(allocator)
    {
      assign(n, value);
    }

    template <class InputIterator, class = typename iterator_traits<InputIterator>::value_type>
    vector(InputIterator first, InputIterator last, const Allocator& alloc = Allocator())
      : vector(alloc)
    {
      assign(first, last);
    }

    vector(const vector<T, Allocator>& x)
      : vector(x, Allocator{}) {}

    //vector(vector&& x) : vector(move(x), Allocator{});
    vector(vector&& other)
    {
      swap(other);
    }

    vector(const vector& other, const Allocator& allocator)
      : vector(allocator)
    {
      _assign(other);
    }

    vector(vector&& other, const Allocator& alloc)
      : vector(alloc)
    {
      std::swap(other.begin_, begin_);
      std::swap(other.end_, end_);
      std::swap(other.capacity_, capacity_);
      // don't swap allocator 
      // 23.2.1 [container.requirements.general] 
      // Table 99 � Allocator-aware container requirements
    }

    vector(std::initializer_list<T> il, const Allocator& alloc = Allocator())
      : vector(il.begin(), il.end(), alloc)
    {
    }

    ~vector()
    {
      auto alloc = get_allocator();
      for (pointer p = begin_; p != end_; ++p)
        alloc.destroy(p);
      alloc.deallocate(begin_, capacity());
    }

    vector<T, Allocator>& operator=(const vector<T, Allocator>& x)
    {
      if (this == &x) return *this;
      _assign(x);
      return *this;
    }

    vector<T, Allocator>& operator=(vector<T, Allocator>&& other)
    {
      swap(other);
      return *this;
    }

    vector& operator=(std::initializer_list<T> il)
    {
      assign(il.begin(), il.end());
      return *this;
    }

    template <class InputIterator, class = typename iterator_traits<InputIterator>::value_type>
    void assign(InputIterator first, InputIterator last)
    {
      auto alloc = get_allocator();
      const size_type n = std::distance(first, last);
      if (alloc.max_size() <= (size_type)n) throw length_error("vector: requested size is bigger then max_size()");

      clear();
      if (n > capacity()) {
        alloc.deallocate(begin_, capacity());
        end_ = begin_ = capacity_ = nullptr;
        end_ = begin_ = alloc.allocate(n);
        capacity_ = begin_ + n;
      }

      for (auto it = first; it != last; ++it, ++end_)
        alloc.construct(end_, *it);
    }

    void _assign(const vector& other)
    {
      auto alloc = get_allocator();
      const auto n = other.size();
      if (alloc.max_size() <= (size_type)n) throw length_error("vector: requested size is bigger then max_size()");

      clear();
      if (n > capacity()) {
        alloc.deallocate(begin_, capacity());
        end_ = begin_ = capacity_ = nullptr;
        end_ = begin_ = alloc.allocate(n);
        capacity_ = begin_ + n;
      }

      for (pointer p = other.begin_; p != other.end_; ++p, ++end_)
        alloc.construct(end_, *p);
    }

    void assign(size_type n, const T& value)
    {
      auto alloc = get_allocator();
      if (alloc.max_size() <= n) throw length_error("vector: requested size is bigger then max_size()");

      clear();
      if (n > capacity()) {
        alloc.deallocate(begin_, capacity());
        end_ = begin_ = capacity_ = nullptr;
        end_ = begin_ = alloc.allocate(n);
        capacity_ = begin_ + n;
      }

      for (size_type i = 0; i < n; ++i, ++end_)
        alloc.construct(end_, value);
    }

    void assign(std::initializer_list<T> il)
    {
      assign(il.begin(), il.end());
    }

    allocator_type get_allocator() const noexcept
    {
      return alloc_;
    }

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
      return end_ - begin_;
    }

    size_type max_size() const noexcept
    {
      return alloc_.max_size();
    }

    size_type capacity() const noexcept
    {
      return capacity_ - begin_;
    }

    void reserve(size_type n)
    {
      if (capacity() >= n) return;
      auto alloc = get_allocator();
      if (alloc.max_size() <= n) throw length_error("vector: requested size is bigger then max_size()");

      vector v(alloc);
      // allocate memory
      v.end_ = v.begin_ = alloc.allocate(n);
      v.capacity_ = v.begin_ + n;

      // copy orig to new memory
      for (pointer p = begin_; p != end_; ++p, ++v.end_) {
        alloc.construct(v.end_, *p);
      }
      swap(v);
    }

    void shrink_to_fit()
    {
      if (capacity() == size()) return;

      const auto n = size();

      auto alloc = get_allocator();
      if (alloc.max_size() <= n) throw length_error("vector: requested size is bigger then max_size()");

      vector v(alloc);
      // allocate memory
      v.end_ = v.begin_ = alloc.allocate(n);
      v.capacity_ = v.begin_ + n;

      // copy orig to new memory
      for (pointer p = begin_; p != end_; ++p, ++v.end_) {
        alloc.construct(v.end_, *p);
      }
      swap(v);      
    }

    void resize(size_type n)
    {
      auto alloc = get_allocator();

      // 1. begin() <= n <= end()
      if (n <= size()) {
        while (n < size()) alloc.destroy(--end_);
        return;
      }
      // 2. end() <= n <= capacity()
      if (n <= capacity()) {
        while (end_ < begin_ + n) {
          alloc.construct(end_, T());
          ++end_;
        }
        return;
      }

      // 3. capacity <= n : alloc memory
      vector v(alloc);
      v.end_ = v.begin_ = alloc.allocate(n);
      v.capacity_ = v.begin_ + n;

      for (pointer p = begin_; p != end_; ++p, ++v.end_) {
        alloc.construct(v.end_, *p);
      }
      v.resize(n); // case 2.
      swap(v);
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
      return *(end_ - 1);
    }
    const_reference back() const
    {
      return *(end_ - 1);
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
      auto alloc = get_allocator();
      const auto n = size() + 1;

      if (n <= capacity()) {
        alloc.construct(end_, std::forward<Args>(args)...);
        ++end_;
        return;
      }

      if (alloc.max_size() <= n) throw length_error("vector: requested size is bigger then max_size()");

      vector v;

      // allocate memory
      const auto new_capacity = n + n / 2 + 1;
      v.end_ = v.begin_ = alloc.allocate(new_capacity);
      v.capacity_ = v.begin_ + new_capacity;

      // copy orig to new memory
      for (pointer p = begin_; p != end_; ++p, ++v.end_) {
        alloc.construct(v.end_, *p);
      }
      alloc.construct(v.end_, std::forward<Args>(args)...);
      ++v.end_;

      swap(v);
    }

    void push_back(const T& c)
    {
      auto alloc = get_allocator();
      const auto n = size() + 1;

      if (n <= capacity()) {
        alloc.construct(end_, c);
        ++end_;
        return;
      }

      if (alloc.max_size() <= n) throw length_error("vector: requested size is bigger then max_size()");

      vector v;

      // allocate memory
      const auto new_capacity = n + n / 2 + 1;
      v.end_ = v.begin_ = alloc.allocate(new_capacity);
      v.capacity_ = v.begin_ + new_capacity;

      // copy orig to new memory
      for (pointer p = begin_; p != end_; ++p, ++v.end_) {
        alloc.construct(v.end_, *p);
      }
      v.push_back(c);

      swap(v);
    }

    void push_back(T&& c)
    {
      auto alloc = get_allocator();
      const auto n = size() + 1;

      if (n <= capacity()) {
        alloc.construct(end_, move(c));
        ++end_;
        return;
      }

      if (alloc.max_size() <= n) throw length_error("vector: requested size is bigger then max_size()");

      vector v(alloc);

      // allocate memory
      const auto new_capacity = n + n / 2 + 1;
      v.end_ = v.begin_ = alloc.allocate(new_capacity);
      v.capacity_ = v.begin_ + new_capacity;

      // copy orig to new memory
      for (pointer p = begin_; p != end_; ++p, ++v.end_) {
        alloc.construct(v.end_, *p);
      }

      //v.push_back(move(c));
      alloc.construct(v.end_, move(c));
      ++v.end_;

      swap(v);
    }

    void pop_back()
    {
      resize(size() - 1);
    }

    template <class... Args>
    iterator emplace(const_iterator position, Args&&... args)
    {
      // store position pointer independent for return value
      const size_type pos = position - iterator(begin_);
      auto alloc = get_allocator();
      const size_type n = size() + 1;

      if (n <= capacity()) {
        T val = T(std::forward<Args>(args)...);
        for (pointer p = begin_ + pos; p != end_; ++p)
          std::swap(val, *p);

        alloc.construct(end_, val);
        ++end_;
        return iterator(begin_ + pos);
      }
      vector v(alloc);
      // allocate memory
      v.end_ = v.begin_ = alloc.allocate(n);
      v.capacity_ = v.begin_ + n;

      pointer p = begin_;
      // copy first part
      for (; p != begin_ + pos; ++p, ++v.end_) {
        alloc.construct(v.end_, *p);
      }
      // insert value
      alloc.construct(v.end_, std::forward<Args>(args)...);
      ++v.end_;

      // copy the remaining part
      for (; p != end_; ++p, ++v.end_) {
        alloc.construct(v.end_, *p);
      }
      swap(v);
      return iterator(begin_ + pos);

    }


    iterator insert_simple(const_iterator position, const T& x)
    {
      // store position pointer independent for return value
      const size_type pos = position - iterator(begin_);

      if (size() + 1 <= capacity()) {
        T val = x;
        for (pointer p = begin_ + pos; p != end_; ++p)
          std::swap(val, *p);

        get_allocator().construct(end_, val);
        ++end_;
        return begin_ + pos;
      }
      vector v(*this);
      v.reserve(size() + 1);
      v.insert_simple(v.begin_ + pos, x);
      swap(v);
      return begin_ + pos;
    }

    iterator insert(const_iterator position, const T& x)
    {
      // store position pointer independent for return value
      const size_type pos = position - iterator(begin_);
      auto alloc = get_allocator();
      const size_type n = size() + 1;

      if (n <= capacity()) {
        T val = x;
        for (pointer p = begin_ + pos; p != end_; ++p)
          std::swap(val, *p);

        alloc.construct(end_, val);
        ++end_;
        return iterator(begin_ + pos);
      }
      vector v(alloc);
      // allocate memory
      v.end_ = v.begin_ = alloc.allocate(n);
      v.capacity_ = v.begin_ + n;

      pointer p = begin_;
      // copy first part
      for (; p != begin_ + pos; ++p, ++v.end_) {
        alloc.construct(v.end_, *p);
      }
      // insert value
      alloc.construct(v.end_, x);
      ++v.end_;

      // copy the remaining part
      for (; p != end_; ++p, ++v.end_) {
        alloc.construct(v.end_, *p);
      }
      swap(v);
      return iterator(begin_ + pos);
    }

    iterator insert(const_iterator position, T&& x)
    {
      // store position pointer independent for return value
      const size_type pos = position - iterator(begin_);
      auto alloc = get_allocator();
      const size_type n = size() + 1;

      if (n <= capacity()) {
        T val = move(x);
        for (pointer p = begin_ + pos; p != end_; ++p)
          std::swap(val, *p);

        alloc.construct(end_, val);
        ++end_;

        return iterator(begin_ + pos);
      }
      vector v(alloc);
      // allocate memory
      v.end_ = v.begin_ = alloc.allocate(n);
      v.capacity_ = v.begin_ + n;

      pointer p = begin_;
      // copy first part
      for (; p != begin_ + pos; ++p, ++v.end_) {
        alloc.construct(v.end_, *p);
      }
      // insert value
      alloc.construct(v.end_, move(x));
      ++v.end_;

      // copy the remaining part
      for (; p != end_; ++p, ++v.end_) {
        alloc.construct(v.end_, *p);
      }
      swap(v);
      return iterator(begin_ + pos);
    }

    iterator insert(const_iterator position, size_type num, const T& x)
    {
      // store position pointer independent for return value
      const size_type pos = position - iterator(begin_);

      for (size_type n = 0; n < num; ++n) {
        insert(iterator(begin_ + pos), x);
      }
      // todo: fill code
      return iterator(begin_ + pos);
    }

    template <class InputIterator, class = typename iterator_traits<InputIterator>::value_type>
    iterator insert(const_iterator position, InputIterator first, InputIterator last)
    {
      // store position pointer independent for return value
      const size_type pos = position - iterator(begin_);
      const size_type num = std::distance(first, last);

      size_type n = 0;
      for (InputIterator it = first; it != last; ++n, ++it) {
        insert(iterator(begin_ + pos + n), *it);
      }

      return iterator(begin_ + pos);
    }
    iterator insert(const_iterator position, initializer_list<T> il)
    {
      return insert(position, il.begin(), il.end());
    }

    iterator erase(const_iterator position)
    {
      const size_type pos = position - iterator(begin_); // store position pointer independent for return value

      for (pointer p = begin_ + pos + 1; p != end_; ++p)
        std::swap(*(p - 1), *p);
      get_allocator().destroy(--end_);

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

      auto alloc = get_allocator();
      for (size_type i = 0; i < size_type(p2 - p1); ++i)
        alloc.destroy(--end_);

      return iterator(begin_ + pos1);
    }

    void swap(vector& v)
    {
      std::swap(v.begin_, begin_);
      std::swap(v.end_, end_);
      std::swap(v.capacity_, capacity_);
      std::swap(v.alloc_, alloc_);
    }

    void clear() noexcept
    {
      auto alloc = get_allocator();
      for (pointer p = begin_; p != end_; ++p) {
        alloc.destroy(p); // noexcept, ~T() noexcept 
      }
      end_ = begin_;
    }

  private:
    T* begin_ = nullptr;
    T* end_ = nullptr;
    T* capacity_ = nullptr;
    Allocator alloc_;
  };

  template <class T, class Allocator>
  bool operator==(const vector<T, Allocator>& x, const vector<T, Allocator>& y)
  {
    return x.size() == y.size() && std2::detail::_equal(x.begin(), x.end(), y.begin());
  }
  template <class T, class Allocator>
  bool operator< (const vector<T, Allocator>& x, const vector<T, Allocator>& y)
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
  void swap(vector<T, Allocator>& x, vector<T, Allocator>& y)
  {
    x.swap(y);
  }

  //template <class /*Allocator*/> class vector<bool,Allocator>;  

  // hash support
  //template <class T> struct hash;
  //template <class Allocator> struct hash<vector<bool, Allocator> >;
}
