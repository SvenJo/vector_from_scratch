#pragma once

namespace step0
{
  // Memory Layout
  template<typename T /*, class Allocator = allocator<T> */ >
  class vector
  {
  public:
    // some typedefs
    vector() {}
    // ...

  private:
    T* begin_ = nullptr;
    T* end_ = nullptr;
    T* capacity_ = nullptr;
  };

  void f1()
  {
    vector<int> v;
  }
}
