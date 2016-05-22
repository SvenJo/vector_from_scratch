#pragma once

namespace step1
{
  template<typename T /*, class Allocator = allocator<T> */ >
  class vector
  {
  public:
    // some typedefs
    typedef size_t size_type; // implementation-defined!
    vector() {}
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

  private:
    T* begin_ = nullptr;
    T* end_ = nullptr;
    T* capacity_ = nullptr;
  };

  void f1()
  {
    using std::cout;
    using std::endl;

    vector<int> v;
    cout << "empty    : " << v.empty() << endl;
    cout << "size     : " << v.size() << endl;
    cout << "max_size : " << v.max_size() << endl;
    cout << "capacity : " << v.capacity() << endl;
  }
}
