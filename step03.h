#pragma once

namespace step3
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

  void other(const vector<int>&v)
  {
    using std::cout;
    using std::endl;

    // call const functions
    for (size_t i = 0; i < v.size(); ++i) {
      cout << v[i] << " ";
    }
    cout << endl;

    for (size_t i = 0; i < v.size(); ++i) {
      cout << v.at(i) << " ";
    }
    cout << endl;
  }

  void f1()
  {
    using std::cout;
    using std::endl;

    vector<int> v(10);

    for (size_t i = 0; i < v.size(); ++i) {
      v[i] = 2 + (int)i;
    }

    for (size_t i = 0; i < v.size(); ++i) {
      cout << v[i] << " ";
    }
    cout << endl;

    for (size_t i = 0; i < v.size(); ++i) {
      cout << v.at(i) << " ";
    }
    cout << endl;

    other(v);

    try {
      v.at(100) = 1234;
    }
    catch (const std::exception& ex) {
      cout << ex.what() << endl;
    }
    cout << endl;
  }

}
