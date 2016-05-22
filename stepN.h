#pragma once

#include <memory>

// prepared code for empty member optimization

namespace stepN_1 {

  class Allocator {};

  template<typename T>
  class vector_plain
  {
  private:
    T* begin_;
    T* end_;
    T* capacity_;
  };

  template<typename T, typename ALLOC = Allocator>
  class vector
  {
  public:
    using allocator_type = ALLOC;
    allocator_type get_allocator() const noexcept { return allocator_; }

  private:
    T* begin_;
    T* end_;
    T* capacity_;
    ALLOC allocator_;
  };

  void f1()
  {
    using namespace std;

    cout << "Alloc               :" << sizeof(Allocator) << endl;
    cout << "T*                  :" << sizeof(int*) << endl;
    cout << "3 x T*              :" << 3 * sizeof(int*) << endl;
    cout << "vector_plain        :" << sizeof(vector_plain<int>) << endl;
    cout << "vector              :" << sizeof(vector<int>) << endl;
    cout << "Allocator in vector :" << sizeof(vector<int>) - 3 * sizeof(int*) << endl;
    cout << "std::vector         :" << sizeof(std::vector<int>) << endl;
  }

}


namespace stepN_2
{

  class Allocator {};

  template<typename ALLOC>
  class allocator_holder
  {
  public:
    using allocator_type = ALLOC;
    allocator_type get_allocator() const noexcept { return allocator_; }

  private:
    ALLOC allocator_;
  };

  template<typename T, typename ALLOC = Allocator>
  class vector : public allocator_holder<ALLOC>
  {
  private:
    T* begin_;
    T* end_;
    T* capacity_;
  };

  void f1()
  {
    using namespace std;

    cout << "Alloc               :" << sizeof(Allocator) << endl;
    cout << "T*                  :" << sizeof(int*) << endl;
    cout << "3 x T*              :" << 3 * sizeof(int*) << endl;
    cout << "allocator_holder    :" << sizeof(allocator_holder<Allocator>) << endl;
    cout << "vector              :" << sizeof(vector<int>) << endl;
    cout << "Allocator in vector :" << sizeof(vector<int>) - 3 * sizeof(int*) << endl;
    cout << "std::vector         :" << sizeof(std::vector<int>) << endl;
  }

}


namespace stepN_3
{

  template<typename T>
  class allocator { };

  template<typename Allocator>
  class allocator_holder : Allocator
   {
  public:
    using allocator_type = Allocator;
   
   protected:
     // returns reference to enable modifications
     allocator_type& _allocator() const noexcept { return (allocator_type)*this; }
  };

  template<typename T , class Allocator = allocator<T> >
  class vector : public allocator_holder<Allocator>
  {
  public:
    typedef Allocator allocator_type;
    
    // return a copy, no modification possible!
    allocator_type get_allocator() const noexcept { return Allocator::_allocator(); }
    
  private:
    T* begin_;
    T* end_;
    T* capacity_;
  };

  void f1()
  {
    using namespace std;

    cout << endl << __FUNCTION__ << endl;

    cout << "Alloc               :" << sizeof(allocator<int>) << endl;
    cout << "T*                  :" << sizeof(int*) << endl;
    cout << "3 x T*              :" << 3 * sizeof(int*) << endl;
    cout << "allocator_holder    :" << sizeof(allocator_holder<allocator<int>>) << endl;
    cout << "vector              :" << sizeof(vector<int> /*, Allocator*/) << endl;
    cout << "Allocator in vector :" << sizeof(vector<int>) - 3 * sizeof(int*) << endl;
    cout << "vector              :" << sizeof(vector<int, std::allocator<int>>) << endl;
    cout << "std::vector         :" << sizeof(std::vector<int>) << endl;
    cout << endl << endl;
  }

}

