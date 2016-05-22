#pragma once

namespace step6
{
  class out_of_range : public std::logic_error
  {
  public:
    explicit out_of_range(const std::string& what_arg) : std::logic_error(what_arg) {}
    explicit out_of_range(const char* what_arg) : std::logic_error(what_arg) {}
  };

  class length_error : public std::logic_error
  {
  public:
    explicit length_error(const std::string& what_arg) : std::logic_error(what_arg) {}
    explicit length_error(const char* what_arg) : std::logic_error(what_arg) {}
  };

  template<typename T, class Allocator = std::allocator<T> >
  class vector
  {
  public:
    // some typedefs
    typedef T& reference;
    typedef const T& const_reference;
    typedef Allocator allocator_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef size_t size_type; // implementation-defined!

    // 23.3.6.2, construct/copy/destroy:
    explicit vector(const Allocator& alloc = Allocator()) : alloc_(alloc) {}
    explicit vector(size_type n, const Allocator& alloc = Allocator())
      : vector(n, T(), alloc)
    {}

    explicit vector(size_type n, const T& value, const Allocator& allocator = Allocator())
      : vector(allocator)
    {
      auto alloc = get_allocator();
      if (alloc.max_size() <= n) throw length_error("vector: requested size is bigger then max_size()");

      end_ = begin_ = alloc.allocate(n);
      capacity_ = begin_ + n;

      for (size_type i = 0; i < n; ++i /*, ++end_*/) {
        alloc.construct(end_, value);
        ++end_;
      }
    }

    ~vector()
    {
      auto alloc = get_allocator();
      for (pointer p = begin_; p != end_; ++p)
        alloc.destroy(p);
      alloc.deallocate(begin_, capacity());
    }

    // ...
    allocator_type get_allocator() const noexcept 
    { 
      return alloc_; 
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
      return std::numeric_limits<size_type>::max() / sizeof(T);
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
      if (alloc.max_size() < n) 
        throw length_error("vector: requested size is bigger then max_size()");

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
  void swap(vector<T, Allocator>& x, vector<T, Allocator>& y)
  {
    x.swap(y);
  }

  void f1()
  {
    vector<std::string> v(3);
    v.resize(1); assert(v.size() == 1); assert(v.capacity() == 3);
    v.resize(1); assert(v.size() == 1); assert(v.capacity() == 3);
    v.resize(2); assert(v.size() == 2); assert(v.capacity() == 3);
    v.resize(2); assert(v.size() == 2); assert(v.capacity() == 3);
    v.resize(2); assert(v.size() == 2); assert(v.capacity() == 3);
    v.resize(3); assert(v.size() == 3); assert(v.capacity() == 3);
    v.resize(4); assert(v.size() == 4); assert(v.capacity() >= 4);
    v.resize(4); assert(v.size() == 4); assert(v.capacity() >= 4);    
    v.clear(); assert(v.size() == 0); assert(v.capacity() >= 4);
  }

  void f2()
  {
    vector<std::string> v(3);
    v.reserve(1); assert(v.size() == 3); assert(v.capacity() == 3);
    v.reserve(1); assert(v.size() == 3); assert(v.capacity() == 3);
    v.reserve(2); assert(v.size() == 3); assert(v.capacity() == 3);
    v.reserve(2); assert(v.size() == 3); assert(v.capacity() == 3);
    v.reserve(3); assert(v.size() == 3); assert(v.capacity() == 3);
    v.reserve(4); assert(v.size() == 3); assert(v.capacity() >= 4);
    v.clear(); assert(v.size() == 0); assert(v.capacity() >= 4);
    v.reserve(5); assert(v.size() == 0); assert(v.capacity() >= 5);
  }

  void f3()
  {
    vector<std::string> v;
    v.push_back("1"); assert(v.size() == 1); assert(v.capacity() >= 1);
    v.reserve(2);
    v.push_back("2"); assert(v.size() == 2); assert(v.capacity() >= 2);
    v.clear();
    v.push_back("3"); assert(v.size() == 1); assert(v.capacity() >= 2);
    v.push_back("4"); assert(v.size() == 2); assert(v.capacity() >= 2);
    v.push_back("5"); assert(v.size() == 3); assert(v.capacity() >= 3);
    v.push_back("6"); assert(v.size() == 4); assert(v.capacity() >= 4);
    v.push_back("7"); assert(v.size() == 5); assert(v.capacity() >= 5);
    v.clear(); assert(v.size() == 0); assert(v.capacity() >= 4);
  }

}
