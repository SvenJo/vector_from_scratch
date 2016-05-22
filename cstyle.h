#pragma once

namespace cstyle {
  
  using std::cout;
  using std::endl;
  using std::boolalpha;

  void f1()
  {
    // raw pointers
    const int i_ptr_size = 10;
    int* i_ptr = new int[i_ptr_size]();

    for (int i = 0; i < i_ptr_size; ++i) {
      i_ptr[i] = 2 + i;
    }

    for (int i = 0; i < i_ptr_size; ++i) {
      cout << i_ptr[i] << " ";
    }
    cout << endl;

    for (int* pi = i_ptr; pi < i_ptr + i_ptr_size; ++pi) {
      cout << *pi << " ";
    }
    cout << endl;

    // no range based for loop

    delete[] i_ptr;
  }

  void f2()
  {
    // C style arrays
    int i_arr[] = { 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 }; // at compile time

    const int i_ptr_size = sizeof(i_arr) / sizeof(int);

    for (int i = 0; i < i_ptr_size; ++i) {
      cout << i_arr[i] << " ";
    }
    cout << endl;

    for (int i : i_arr) {
      cout << i << " ";
    }
    cout << endl;
  }

  void f3()
  {
    // std::vector
    std::vector<int> v = { 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 }; // initializer lists

    for (size_t i = 0; i < v.size(); ++i) {
      cout << v[i] << " ";
    }
    cout << endl;

    for (auto it = v.cbegin(); it != v.cend(); ++it) {
      cout << *it << " ";
    }

    for (int i : v) {
      cout << i << " ";
    }
    cout << endl;
  }

  void f4()
  {
    // std::array
    std::array<int, 10> a = { { 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 } }; // initializer lists

    for (size_t i = 0; i < a.size(); ++i) {
      cout << a[i] << " ";
    }
    cout << endl;

    for (int i : a) {
      cout << i << " ";
    }
    cout << endl;
  }
}
