// no pragma once !!!

#include <string>

namespace TOKENMERGE(test_, VT_NAMPESPACE)
{
  // prevent from warning: unused variable
  template<typename T>
  inline void value_sink(const T&) {}

  using namespace VT_NAMPESPACE; // std for std::vector or std2 for std::vector

  // "basic tests")
  TEST_CASE(VT_PRE "empty vector")
  {
    vector<int> v;

    REQUIRE(v.empty() == true);
    REQUIRE(v.size() == 0);
    REQUIRE(v.capacity() == 0);
    REQUIRE(v.max_size() > 10000);

    //int* p = new int[100];
  }

  TEST_CASE(VT_PRE "10 default elements")
  {
    vector<int> v(10);

    REQUIRE(v.empty() == false);
    REQUIRE(v.size() == 10);
    REQUIRE(v.capacity() == 10);
    REQUIRE(v.max_size() > 10000);

    REQUIRE(v[0] == 0);
    REQUIRE(v[9] == 0);
  }

  TEST_CASE(VT_PRE "10 elements(== -1234)")
  {
    vector<int> v(10, -1234);

    REQUIRE(v.empty() == false);
    REQUIRE(v.size() == 10);
    REQUIRE(v.capacity() == 10);
    REQUIRE(v.max_size() > 10000);

    REQUIRE(v[0] == -1234);
    REQUIRE(v[9] == -1234);
  }

  TEST_CASE(VT_PRE "element access")
  {
    vector<int> v(10, -1234);

    SECTION("operator[]")
    {
      for (int i = 0; i < 10; ++i) {
        REQUIRE(v[i] == -1234);
        v[i] = v[i] * -1; // make positive
      }
      for (int i = 0; i < 10; ++i) {
        REQUIRE(v[i] == 1234); // now all positive
      }
      // const access
      const auto& cv = v;
      for (int i = 0; i < 10; ++i) {
        REQUIRE(cv[i] == 1234);
        //cv[i] = 42; // shall not compile!!!
      }
    }

    SECTION("at()")
    {
      for (int i = 0; i < 10; ++i) {
        REQUIRE(v[i] == -1234);
        v.at(i) = v.at(i) * -1; // make positive
      }
      for (int i = 0; i < 10; ++i) {
        REQUIRE(v.at(i) == 1234); // now all positive
      }
      // const access
      const auto& cv = v;
      for (int i = 0; i < 10; ++i) {
        REQUIRE(cv.at(i) == 1234);
        //cv.at(i) = 42; // shall not compile!!!
      }
    }
  }

  template<typename FUNC>
  void test_out_of_range(FUNC func)
  {
    bool is_exception = false;
    bool wrong_exception = false;
    try {
      func();
    }
    catch (const out_of_range&) {
      is_exception = true;
    }
    catch (...) {
      wrong_exception = true;
    }
    REQUIRE(is_exception == true);
    REQUIRE(wrong_exception == false);
  }

  TEST_CASE(VT_PRE "out of range(at())")
  {
    vector<int> v10(10, -1234);
    const auto& cv10 = v10;

    test_out_of_range([&v10] { v10.at(10) = 1; });
    test_out_of_range([&v10] { int i = v10.at(10); value_sink(i); });
    //missing copy ctor: test_out_of_range([v10] { int i = v10.at(10); value_sink(i); }); // const read
    test_out_of_range([&cv10] { int i = cv10.at(10); value_sink(i); }); // const read

    vector<int> v0;
    const auto& cv0 = v10;
    test_out_of_range([&v0] { v0.at(10) = 1; });
    test_out_of_range([&v0] { int i = v0.at(10); value_sink(i); });
    //missing copy ctor: test_out_of_range([v0] { int i = v0.at(10); value_sink(i); }); // const read
    test_out_of_range([&cv0] { int i = cv0.at(10); value_sink(i); }); // const read
  }

  TEST_CASE(VT_PRE "clear")
  {
    vector<int> v(10, -1234);
    v.clear();

    REQUIRE(v.empty() == true);
    REQUIRE(v.size() == 0);
    //REQUIRE(v.capacity() >= 0); always true for unsigned

    v.reserve(10);
    v.clear();

    REQUIRE(v.empty() == true);
    REQUIRE(v.size() == 0);
    REQUIRE(v.capacity() >= 10);

    v.resize(20);
    v.clear();

    REQUIRE(v.empty() == true);
    REQUIRE(v.size() == 0);
    REQUIRE(v.capacity() >= 20);
  }

  TEST_CASE(VT_PRE "swap")
  {
    vector<int> v1(3, -1);
    vector<int> v2(4, 21);

    v1.swap(v2);

    REQUIRE(v1.empty() == false);
    REQUIRE(v1.size() == 4);
    REQUIRE(v1.capacity() ==4);
    REQUIRE(v1[0] == 21);
    REQUIRE(v1[3] == 21);

    REQUIRE(v2.empty() == false);
    REQUIRE(v2.size() == 3);
    REQUIRE(v2.capacity() == 3);
    REQUIRE(v2[0] == -1);
    REQUIRE(v2[2] == -1);
  }

  TEST_CASE(VT_PRE "swap with empty")
  {
    vector<int> v1;
    vector<int> v2(4, 21);

    v1.swap(v2);

    REQUIRE(v1.empty() == false);
    REQUIRE(v1.size() == 4);
    REQUIRE(v1.capacity() == 4);
    REQUIRE(v1[0] == 21);
    REQUIRE(v1[3] == 21);

    REQUIRE(v2.empty() == true);
    REQUIRE(v2.size() == 0);
    REQUIRE(v2.capacity() == 0);
  }

  TEST_CASE(VT_PRE "external swap")
  {
    vector<int> v1 = {1, 2, 3};
    vector<int> v2 = {4, 5, 6, 7};

    swap(v1, v2);

    REQUIRE(v1.size() == 4);
    REQUIRE(v1.capacity() == 4);
    REQUIRE(v1[0] == 4);
    REQUIRE(v1[3] == 7);


    REQUIRE(v2.size() == 3);
    REQUIRE(v2.capacity() == 3);
    REQUIRE(v2[0] == 1);
    REQUIRE(v2[2] == 3);
  }
  
  TEST_CASE(VT_PRE "move ctor")
  {
    vector<int> v1(10,-1);
    
    const auto v2(std::move(v1));
    
    REQUIRE(v2.empty() == false);
    REQUIRE(v2.size() == 10);
    REQUIRE(v2.capacity() == 10);
    REQUIRE(v2[0] == -1);
    REQUIRE(v2[9] == -1);

    REQUIRE(v1.empty() == true);
    REQUIRE(v1.size() == 0);
    REQUIRE(v1.capacity() == 0);
     
  }

  TEST_CASE(VT_PRE "move ctor (with allocator)")
  {
    vector<int> v1(10, -1);

    vector<int> v2(std::move(v1), allocator<int>());

    REQUIRE(v2.empty() == false);
    REQUIRE(v2.size() == 10);
    REQUIRE(v2.capacity() == 10);
    REQUIRE(v2[0] == -1);
    REQUIRE(v2[9] == -1);

    REQUIRE(v1.empty() == true);
    REQUIRE(v1.size() == 0);
    REQUIRE(v1.capacity() == 0);

  }
  
  TEST_CASE(VT_PRE "copy ctor")
  {
    vector<int> v1(10,-1);
    
    const auto v2 = v1;
    const auto v3(v1);
    
    REQUIRE(v1.empty() == v2.empty());
    REQUIRE(v1.size() == v2.size());
    REQUIRE(v1.capacity() <= v2.capacity());
    REQUIRE(v1[0] == v2[0]);
    REQUIRE(v1[9] == v2[9]); 

    REQUIRE(v1.empty() == v3.empty());
    REQUIRE(v1.size() == v3.size());
    REQUIRE(v1.capacity() <= v3.capacity());
    REQUIRE(v1[0] == v3[0]);
    REQUIRE(v1[9] == v3[9]);
  }

  TEST_CASE(VT_PRE "copy ctor (with allocator)")
  {
    vector<int> v1(10, -1);

    //const auto v2 = v1;
    vector<int> v3(v1, allocator<int>());

    REQUIRE(v1.empty() == v3.empty());
    REQUIRE(v1.size() == v3.size());
    REQUIRE(v1.capacity() <= v3.capacity());
    REQUIRE(v1[0] == v3[0]);
    REQUIRE(v1[9] == v3[9]);
  }
  
  TEST_CASE(VT_PRE "ctor initializer_list")
  {
    vector<int> v1 = {1, 2, 3, 4, 5 };

    REQUIRE(v1.empty() == false);
    REQUIRE(v1.size() == 5);
    REQUIRE(v1.capacity() == 5);
    REQUIRE(v1[0] == 1);
    REQUIRE(v1[1] == 2);
    REQUIRE(v1[2] == 3);
    REQUIRE(v1[3] == 4);
    REQUIRE(v1[4] == 5);
  }

  TEST_CASE(VT_PRE "vector(begin_it, end_it)")
  {
    std::string s{ "Hallo" };
    vector<char> v(s.begin(), s.end());
    
    REQUIRE(v.size() == s.size());


    std::vector<int> sv0;
    vector<int> v0(sv0.begin(), sv0.end());
    REQUIRE(v0.size() == 0);

    std::vector<int> sv5 = { 1,2,3,4,5 };
    vector<int> v5(sv5.begin(), sv5.end());
    REQUIRE(v5.size() == 5);
    
  }

  
  TEST_CASE(VT_PRE "front and back")
  {
    vector<int> v1 = {1, 2, 3, 4, 5 };
    
    REQUIRE(v1.front() == 1);
    REQUIRE(v1.back() == 5);
    
    const auto& v2 = v1;
    REQUIRE(v2.front() == 1);
    REQUIRE(v2.back() == 5);
  }
  
  TEST_CASE(VT_PRE "data")
  {
    vector<int> v1 = {1, 2, 3, 4, 5 };
    
    REQUIRE(v1.data() != nullptr);
    REQUIRE(*v1.data() == 1);
    REQUIRE(*(v1.data()+4) == 5);
    REQUIRE(&v1.front() == v1.data());

    const auto& v2 = v1;
    REQUIRE(v2.data() != nullptr);
    REQUIRE(*v2.data() == 1);
    REQUIRE(*(v2.data()+4) == 5);
    REQUIRE(&v2.front() == v2.data());
  }
  
  TEST_CASE(VT_PRE "begin / end")
  {
    vector<int> v = {1, 2, 3, 4, 5 };
    
    int i=0;
    size_t count = 0;
    for(vector<int>::iterator it = v.begin(); it != v.end(); ++it) {
      REQUIRE(++i ==  *it);
      count++;
    }
    REQUIRE(count == v.size());    
    
    i=0;
    count = 0;
    for(auto  it = v.begin(); it != v.end(); ++it) {
      REQUIRE(++i ==  *it);
      count++;
    }
    REQUIRE(count == v.size());
    
    i=0;
    count = 0;
    for(auto  it = v.cbegin(); it != v.cend(); ++it) {
      REQUIRE(++i ==  *it);
      count++;
    }
    REQUIRE(count == v.size());
    
    i=0;
    count = 0;
    for(const auto  n : v) {
      REQUIRE(++i ==  n);
      count++;
    }
    REQUIRE(count == v.size());
  }
  
  TEST_CASE(VT_PRE "rbegin / rend")
  {
    vector<int> v = { 1, 2, 3, 4, 5 };

    int i = 6;
    size_t count = 0;
    for (vector<int>::reverse_iterator it = v.rbegin(); it != v.rend(); ++it) {
      REQUIRE(--i == *it);
      count++;
    }
    REQUIRE(count == v.size());

    i = 6;
    count = 0;
    for (auto it = v.rbegin(); it != v.rend(); ++it) {
      REQUIRE(--i == *it);
      count++;
    }
    REQUIRE(count == v.size());

/*
    i = 6;
    count = 0;
    for (auto it = v.crbegin(); it != v.crend(); ++it) {
      REQUIRE(--i == *it);
      count++;
    }
    REQUIRE(count == v.size());*/
  }


  TEST_CASE(VT_PRE "range base write")
  {
    vector<int> v(11, -1);

    int i=0;
    size_t count = 0;
    for(auto&  n : v) {
      n = ++i;  // change v!!!!
      count++;
    }
    REQUIRE(count == v.size());

    i=0;
    count = 0;
    for(auto  it = v.begin(); it != v.end(); ++it) {
      REQUIRE(++i ==  *it);
      count++;
    }
    REQUIRE(count == v.size());

#ifdef __GNUG__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif
    count = 0;
    for(auto  n : v) { // no ref!!!
      n = -299;  // don't change v!!!!
      count++;
    }
    REQUIRE(count == v.size());
#ifdef __GNUG__
#pragma GCC diagnostic pop
#endif


    i=0;
    count = 0;
    for(auto  it = v.begin(); it != v.end(); ++it) {
      REQUIRE(++i ==  *it);
      count++;
    }
    REQUIRE(count == v.size());
  }
  
  TEST_CASE(VT_PRE "pop back")
  {
    vector<int> v = {1, 2, 3, 4, 5 };
    
    const auto it_begin = v.begin();
    const auto it_end = v.end();
    const auto it_2 = it_begin + 1;
    
    v.pop_back();
    
    REQUIRE(v.empty() == false);
    REQUIRE(v.size() == 4);
    REQUIRE(v.capacity() == 5);

    REQUIRE(v[0] == 1);
    REQUIRE(v[1] == 2);
    REQUIRE(v[2] == 3);
    REQUIRE(v[3] == 4);
    
    REQUIRE(it_begin == v.begin());
    REQUIRE(it_2 == v.begin() + 1);
    
    value_sink(it_end);
#if !defined(_MSC_VER)
    REQUIRE(it_end != v.end());
#endif
  }
  
  TEST_CASE(VT_PRE "push back")
  {
    vector<int> v = {1, 2, 3, 4, 5 };
    
    v.push_back(11);

    REQUIRE(v.empty() == false);
    REQUIRE(v.size() == 6);
    REQUIRE(v.capacity() >= 6);

    REQUIRE(v[0] == 1);
    REQUIRE(v[1] == 2);
    REQUIRE(v[2] == 3);
    REQUIRE(v[3] == 4);
    REQUIRE(v[4] == 5);
    REQUIRE(v[5] == 11);
    
    v.reserve(1000);
    v.push_back(22);
    
        REQUIRE(v.empty() == false);
    REQUIRE(v.size() == 7);
    REQUIRE(v.capacity() >= 1000);

    REQUIRE(v[0] == 1);
    REQUIRE(v[1] == 2);
    REQUIRE(v[2] == 3);
    REQUIRE(v[3] == 4);
    REQUIRE(v[4] == 5);
    REQUIRE(v[5] == 11);
    REQUIRE(v[6] == 22);
  }
  
  TEST_CASE(VT_PRE "push back (empty vector)")
  {
    vector<int> v;
    
    v.push_back(-11);

    REQUIRE(v.empty() == false);
    REQUIRE(v.size() == 1);
    REQUIRE(v.capacity() >= 1);

    REQUIRE(v[0] == -11);
  }
  
  TEST_CASE(VT_PRE "push back (geometric(?) resize)")
  {
    vector<int> v;
    
    int realloc_count = 0;
    auto old_capacity = v.capacity(); // == 0
    for(int i=0; i<100; ++i) {
      v.push_back(i);
      if (old_capacity != v.capacity()) {
        old_capacity = v.capacity();
        realloc_count++;
      }
    }
    // GCC == 8
    // std2 == 11
    // MSVC++ 2015 == 13
    REQUIRE(realloc_count < 14);
  }
  
  TEST_CASE(VT_PRE "push back (move)")
  {
    vector<int> v = {1, 2, 3, 4, 5 };
    
    int n = 11;
    v.push_back(std::move(n));

    REQUIRE(v.empty() == false);
    REQUIRE(v.size() == 6);
    REQUIRE(v.capacity() >= 6);

    REQUIRE(v[0] == 1);
    REQUIRE(v[1] == 2);
    REQUIRE(v[2] == 3);
    REQUIRE(v[3] == 4);
    REQUIRE(v[4] == 5);
    REQUIRE(v[5] == 11);
    
    v.reserve(1000);
    int m=22;
    v.push_back(std::move(m));
    
    REQUIRE(v.empty() == false);
    REQUIRE(v.size() == 7);
    REQUIRE(v.capacity() >= 1000);

    REQUIRE(v[0] == 1);
    REQUIRE(v[1] == 2);
    REQUIRE(v[2] == 3);
    REQUIRE(v[3] == 4);
    REQUIRE(v[4] == 5);
    REQUIRE(v[5] == 11);
    REQUIRE(v[6] == 22);
  }

  TEST_CASE(VT_PRE "void assign(size_type n, const T& u)")
  {
    vector<int> v = { 1,2 };
    v.assign(10, -1);

    REQUIRE(v.size() == 10);
    REQUIRE(v[0] == -1);
    REQUIRE(v[9] == -1);
  }

  TEST_CASE(VT_PRE "operator=")
  {
    vector<int> v;
    vector<int> v1 = { 1 };
    vector<int> v2 = { 1,2 };
    v = v; // self assign
    v = v1;
    v = std::move(v2);
    v = { 1 , 2 };
  }

  TEST_CASE(VT_PRE "assign")
  {
    vector<int> v;
    vector<int> v1 = { 1 };
    std::string s = "Hallo";
    
    v.assign(v.begin(), v.end()); // self assign
    v.assign(s.begin(), s.end());
    v.assign({ 1 , 2 });
    v.assign(3, -1);
  }


  TEST_CASE(VT_PRE "vector<std::string>")
  {
    vector<std::string> v1;
    vector<std::string> v10(10);
    vector<std::string> v11(11, "Hallo");
  }

  template<typename V, typename T = typename V::value_type>
  void insert_test(const V& v, const V& expected, const typename V::iterator& it, const T& expected_val)
  {
    REQUIRE(v == expected);
    REQUIRE(*it == expected_val);
    REQUIRE(it < v.end());
  }

  TEST_CASE(VT_PRE "insert")
  {
    for (int i = 0; i < 2; ++i) {
      vector<int> v = { 1, 2, 3 };
      if (i == 1) 
        v.resize(1000);

      SECTION("insert begin")
      {
        auto it = v.insert(v.begin(), 10);
        vector<int> w = { 10, 1, 2, 3 };
        insert_test(v, w, it, 10);
      }
      SECTION("insert end")
      {
        auto it = v.insert(v.end(), 99);
        vector<int> w = { 1, 2, 3, 99 };
        insert_test(v, w, it, 99);
      }
      SECTION("insert multipe elements begin")
      {
#if defined(VT_STD2) || !defined(__GNUG__)
        auto it = v.insert(v.begin(), 3, 10);
        vector<int> w = { 10, 10, 10, 1, 2, 3 };
        insert_test(v, w, it, 10);
#endif
      }
      SECTION("insert multipe elements end")
      {
#if defined(VT_STD2) || !defined(__GNUG__)
        auto it = v.insert(v.end(), 3, 99);
        vector<int> w = { 1, 2, 3, 99, 99, 99 };
        insert_test(v, w, it, 99);
#endif
      }
      SECTION("insert init-list begin")
      {
#if defined(VT_STD2) || !defined(__GNUG__)
        auto it = v.insert(v.begin(), { 10, 11});
        vector<int> w = { 10, 11, 1, 2, 3 };
        insert_test(v, w, it, 10);
#endif
      }
      SECTION("insert init-list end")
      {
#if defined(VT_STD2) || !defined(__GNUG__)
        auto it = v.insert(v.end(), { 99 , 100, 101 });
        vector<int> w = { 1, 2, 3, 99, 100, 101 };
        insert_test(v, w, it, 99);
#endif
      }
      SECTION("insert other vec begin")
      {
#if defined(VT_STD2) || !defined(__GNUG__)
        vector<char> other = { 'a', 'b', 'c' };
        auto it = v.insert(v.begin(), other.begin(), other.end());
        vector<int> w = { 97, 98, 99, 1, 2, 3 };
        insert_test(v, w, it, 97);
#endif
      }
      SECTION("insert other vec end")
      {
#if defined(VT_STD2) || !defined(__GNUG__)
        vector<char> other = { 'a', 'b', 'c' };
        auto it = v.insert(v.end(), other.begin(), other.end());
        vector<int> w = { 1, 2, 3, 97, 98, 99};
        insert_test(v, w, it, 97);
#endif
      }
      SECTION("emplace begin")
      {
        auto it = v.emplace(v.begin(), 10);
        vector<int> w = { 10, 1, 2, 3 };
        insert_test(v, w, it, 10);
      }
      SECTION("emplace end")
      {
        auto it = v.emplace(v.end(), 99);
        vector<int> w = { 1, 2, 3, 99 };
        insert_test(v, w, it, 99);
      }
    }
  }

  TEST_CASE(VT_PRE "Insert with move")
  {
    const std::string s = "longer_then_short_string_optimization";

    for (int i = 0; i < 2; ++i) {
      vector<std::string> v = { "ABC", "DEF", "GHI" };
      if (i == 1)
        v.reserve(1000);
      SECTION("move begin")
      {
        string lv = s;
        v.insert(v.begin(), std::move(lv));
        REQUIRE(lv.size() == 0);
      }
      SECTION("move end")
      {
        string lv = s;
        v.insert(v.end(), std::move(lv));
        REQUIRE(lv.size() == 0);
      }
    }
  }

  TEST_CASE(VT_PRE "erase begin")
  {
    vector<int> v = { 1, 2, 3 };
    auto it = v.erase(v.begin());
    vector<int> w = { 2, 3 };
    REQUIRE(v == w);
    REQUIRE(*it == 2);
  }

  TEST_CASE(VT_PRE "erase 2")
  {
    vector<int> v = { 1, 2, 3 };

    auto it2 = v.begin(); ++it2;

    auto it = v.erase(it2);
    vector<int> w = { 1, 3 };
    REQUIRE(v == w);
    REQUIRE(*it == 3);
  }

  TEST_CASE(VT_PRE "erase string")
  {
    vector<std::string> v = { "A", "B", "C" };
    auto it = v.erase(v.begin());
    vector<std::string> w = { "B", "C" };
    REQUIRE(v == w);
    REQUIRE(*it == "B");
  }

  TEST_CASE(VT_PRE "multi erase")
  {
    vector<int> v = { 1, 2, 3 };
    v.erase(v.begin(), v.end());
    REQUIRE(v.empty());
  }

  TEST_CASE(VT_PRE "multi erase string")
  {
    vector<std::string> v = { "A", "B", "C" };
    v.erase(v.begin(), v.end());
    REQUIRE(v.empty());
  }

  TEST_CASE(VT_PRE "re")
  {
    vector<std::string> v = { "A________A", "X", "B________B", "X", "C________C" };
    auto it = std::remove_if(v.begin(), v.end(), [](const std::string& x) { return x == "X"; });
    v.erase(it, v.end());

    REQUIRE(v.size() == 3);
    REQUIRE(v[0] == "A________A");
    REQUIRE(v[1] == "B________B");
    REQUIRE(v[2] == "C________C");
  }

  TEST_CASE(VT_PRE "push back for string")
  {
    vector<std::string> v = { "abc", "def", "ghi" };
    const std::string bla = "bla";
    std::string blub = "longer_then_short_string_optimization";

    v.push_back(bla);
    vector<std::string> w = vector<std::string>{ "abc", "def", "ghi", "bla" };

    REQUIRE(v == w);
    REQUIRE(bla == "bla");
    v.push_back(std::move(blub));
    w = vector<std::string>{ "abc", "def", "ghi", "bla", "longer_then_short_string_optimization" };
    REQUIRE(v == w);
    REQUIRE(blub.empty() == true);
    v.push_back("hallo");
    w = vector<std::string>{ "abc", "def", "ghi", "bla", "longer_then_short_string_optimization", "hallo" };
    REQUIRE(v == w);
  }

  TEST_CASE(VT_PRE "equal")
  {
    vector<int> v1 = { 1, 2, 3 };
    vector<int> v11;
    v11.reserve(1000);
    v11.push_back(1);
    v11.push_back(2);
    v11.push_back(3);

    vector<int> v2 = { 1, 2, 3, 4 };
    vector<int> v3 = { 1, 2, 4 };

    // test ==
    REQUIRE(v1 == v1);
    REQUIRE(v1 == v11);

    REQUIRE(!(v1 == v2));
    REQUIRE(!(v1 == v3));

    // test !=
    REQUIRE(!(v1 != v1));
    REQUIRE(!(v1 != v11));

    REQUIRE(v1 != v2);
    REQUIRE(v1 != v3);
  }

  TEST_CASE(VT_PRE "empty vector special tests")
  {
    const std::string s = "longer_then_short_string_optimization";
    vector<std::string> v;

    SECTION("basics")
    {
      REQUIRE(v.empty());
      REQUIRE(v.size() == 0);
      REQUIRE(v.capacity() == 0);
      REQUIRE(v.begin() == v.end());
      REQUIRE(v.rbegin() == v.rend());
      REQUIRE(v.cbegin() == v.cend());
      REQUIRE(v.crbegin() == v.crend());
    }
    SECTION("push_back")
    {
      v.push_back(s);
      REQUIRE(v.size() == 1);
      REQUIRE(v.front() == s);
      REQUIRE(v.back() == s);
    }
    SECTION("push_back (move)")
    {
      auto lv = s;
      v.push_back(std::move(lv));
      REQUIRE(v.size() == 1);
      REQUIRE(v.front() == s);
      REQUIRE(v.back() == s);
      REQUIRE(lv.empty());
    }
    SECTION("emplace_back")
    {
      v.emplace_back(s);
      REQUIRE(v.size() == 1);
      REQUIRE(v.front() == s);
      REQUIRE(v.back() == s);
    }
    SECTION("clear()")
    {
      v.clear();
      REQUIRE(v.size() == 0);
      REQUIRE(v.empty());
    }
    SECTION("resize()")
    {
      v.resize(1);
      REQUIRE(v.size() == 1);
    }
    SECTION("reserve()")
    {
      v.reserve(1);
      REQUIRE(v.empty());
      REQUIRE(v.size() == 0);
      REQUIRE(v.capacity() >= 1);
    }
    SECTION("reserve()")
    {
      v.shrink_to_fit();
      REQUIRE(v.empty());
      REQUIRE(v.size() == 0);
      REQUIRE(v.capacity() == 0);
    }
    SECTION("at()")
    {
      bool exception_fired = false;
      try { v.at(1); }
      catch (const out_of_range&) {
        exception_fired = true;
      }      
      REQUIRE(exception_fired);
    }
    SECTION("swap")
    {
      vector<std::string> t(10, s);
      swap(t, v);

      REQUIRE(v.size() == 10);
      REQUIRE(t.size() == 0);
    }
    SECTION("v = t")
    {   
      vector<std::string> t(10, s);
      v = t;
      REQUIRE(v.size() == 10);
      REQUIRE(t.size() == 10);
    }
    SECTION("t = v")
    {
      vector<std::string> t(10, s);
      t = v;
      REQUIRE(v.size() == 0);
      REQUIRE(t.size() == 0);
      REQUIRE(v.empty());
      REQUIRE(t.empty());
    }
    SECTION("v = t empty")
    {
      vector<std::string> t;
      v = t;
      REQUIRE(v.size() == 0);
      REQUIRE(t.size() == 0);
      REQUIRE(v.empty());
      REQUIRE(t.empty());
    }
    SECTION("assign 0")
    {      
      v.assign(0, s);
      REQUIRE(v.size() == 0);
      REQUIRE(v.empty());
      auto* p = v.data();
      REQUIRE(p == nullptr);
    }
    SECTION("assign 2")
    {
      v.assign(2, s);
      REQUIRE(v.size() == 2);
      REQUIRE(!v.empty());
      REQUIRE(v[1] == s);
    }
  }
  
}
