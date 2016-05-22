#pragma once

#include <initializer_list>

namespace step7
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
    explicit vector(const Allocator& alloc = Allocator()) 
      : alloc_(alloc) {}
    explicit vector(size_type n) 
      : vector(n, T{}, Allocator{}) {}
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

    template <class InputIterator, class = typename std::iterator_traits<InputIterator>::value_type>
    vector(InputIterator first, InputIterator last, const Allocator& allocator = Allocator())
      : vector(allocator)
    {
      auto alloc = get_allocator();
      const auto n = std::distance(first, last);
      if (alloc.max_size() <= (size_type)n) throw length_error("vector: requested size is bigger then max_size()");

      end_ = begin_ = alloc.allocate(n);
      capacity_ = begin_ + n;

      for (auto it = first; it != last; ++it, ++end_) {
        alloc.construct(end_, *it);
      }
    }

    vector(const vector<T, Allocator>& x) 
      : vector(x, Allocator{}) {}
    
    //vector(vector&& x) : vector(std::move(x), Allocator{});
    vector(vector&& other)
    {
      swap(other);
    }

    vector(const vector& other, const Allocator& allocator)
      : vector(allocator)
    {
      auto alloc = get_allocator();
      const auto n = other.size();
      if (alloc.max_size() <= n) throw length_error("vector: requested size is bigger then max_size()");

      end_ = begin_ = alloc.allocate(n);
      capacity_ = begin_ + n;

      for (pointer p = other.begin_; p != other.end_; ++p, ++end_) {
        alloc.construct(end_, *p);
      }
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
      vector v(x);
      swap(v);
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

    template <class InputIterator>
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

    void assign(std::initializer_list<T> il)
    {
      assign(il.begin(), il.end());
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
    // source
    std::vector<std::string> s1 = { "1", "2", "3" };
    vector<std::string> s2(100, "A");
    vector<std::string> s3 = s2;
    
    // 9 different constructors
    vector<std::string> v1;
    vector<std::string> v2(2);
    vector<std::string> v3(3, "42");
    vector<std::string> v4(s1.begin(), s1.end());
    vector<std::string> v5 = v3;
    vector<std::string> v6 = std::move(s2);
    vector<std::string> v7(v3, std::allocator<std::string>{});
    vector<std::string> v8(std::move(s3), std::allocator<std::string>{});
    vector<std::string> v9 = { "1", "2", "3" };
  }

  void f2()
  {
    // source
    vector<std::string> s = { "1", "2", "3" };

    // 3 assignment operators
    vector<std::string> v;
    v = s;
    v = std::move(s);
    v = { "42", "3.14", "2.71" };
  }

  void f3()
  {
    // source
    std::vector<std::string> s = { "1", "2", "3" };

    // 3 assignment operators
    vector<std::string> v;
    v.assign(s.begin(), s.end());
    v.assign(3, "42");
    v.assign({ "42", "3.14", "2.71" });
  }

}
