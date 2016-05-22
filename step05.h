#pragma once

namespace step5
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
    vector<int> v1;
    vector<int> v2(std::allocator<int>{});
    vector<int> v3(5);
    vector<int> v4(10, 42);

    assert(v1.size() == 0);
    assert(v1.capacity() == 0);
    assert(v2.size() == 0);
    assert(v2.capacity() == 0);
    assert(v3.size() == 5);
    assert(v3.capacity() == 5);
    assert(v3[0] == 0);
    assert(v3[4] == 0);
    assert(v4.size() == 10);
    assert(v4.capacity() == 10);
    assert(v4[0] == 42);
    assert(v4[9] == 42);
  }

}
