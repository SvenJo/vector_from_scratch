#pragma once

namespace step4
{
  class out_of_range : public std::logic_error
  {
  public:
    explicit out_of_range(const std::string& what_arg) : std::logic_error(what_arg) {}
    explicit out_of_range(const char* what_arg) : std::logic_error(what_arg) {}
  };

  template<typename T /*, class Allocator = allocator<T> */ >
  class vector
  {
  public:
    // some typedefs
    typedef T& reference;
    typedef const T& const_reference;
    typedef size_t size_type; // implementation-defined!
    vector() {}
    explicit vector(size_type n /*, const Allocator& = Allocator()*/)
    {
      begin_ = new T[n]();
      capacity_ = end_ = begin_ + n;
    }
    ~vector()
    {
      delete[] begin_;
    }
    // ...

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
    }

  private:
    T* begin_ = nullptr;
    T* end_ = nullptr;
    T* capacity_ = nullptr;
  };

  template <class T>
  void swap(vector<T>& x, vector<T>& y)
  {
    x.swap(y);
  }

  void f1()
  {
    vector<int> v(2);

    v[0] = 1;
    v[1] = 2;
    assert(v.front() == 1);
    assert(v.back() == 2);

    auto* p = v.data();
    assert(p == &v[0]);
    assert(*p == v.front());

    ++p;
    assert(p == &v[1]);
    assert(*p == v.back());
  }

}
