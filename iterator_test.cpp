#include <vector.hpp>

#include <catch.hpp>
#include <vector>

namespace iterator_test
{
  template<typename T>
  inline void value_sink(const T&) {}

  using namespace std2;
  using namespace std2::detail;

  TEST_CASE("compare")
  {
    int a[] = { 1, 2, 3 };
    vector_iterator<vector<int>> b1(a);
    vector_iterator<vector<int>> e1(a+3);
    vector_iterator<vector<int>> b2(a);
    vector_iterator<vector<int>> e2(a + 3);

    vector_iterator<vector<int>> n1;
    vector_iterator<vector<int>> n2;

    REQUIRE(b1 < e1);
    REQUIRE(b1 <= e1);
    REQUIRE(e1 > b1);
    REQUIRE(e1 >= b1);
    REQUIRE(b1 == b2);
    REQUIRE(b1 <= b2);
    REQUIRE(b1 >= b2);
    REQUIRE(b1 == b1);
    REQUIRE(b1 != e1);

    vector_iterator<vector<int>> b3(b1);
    vector_iterator<vector<int>> b4; b4 = b1;

    REQUIRE(b1 == b3);
    REQUIRE(b1 == b4);

    REQUIRE(!(b1 != b3));
    REQUIRE(!(b1 != b4));

    // default iterator
    REQUIRE(n1 == n2);
    REQUIRE(b1 != n1);
  }

  namespace {
    struct Foo { int i; };
  }

  TEST_CASE("deref")
  {
    int a[] = { 1, 2, 3 };
    Foo foo[] = { {-1}, {-2} };
    vector_iterator<vector<int>> ita(a);
    vector_iterator<vector<Foo>> itf(foo);

    REQUIRE(1 == *ita);
    REQUIRE(-1 == itf->i);

    *ita = 11;
    itf->i = -11;

    REQUIRE(11 == a[0]);
    REQUIRE(-11 == foo[0].i);

  }

  TEST_CASE("increment")
  {
    int a[] = { 1, 2, 3 };
    std::vector<int> v = { 1, 2 , 3 };

    vector_iterator<vector<int>> ita(a);
    std::vector<int>::iterator itv(v.begin());

    auto ta1 = ++ita;
    auto tv1 = ++itv;
    REQUIRE(*ta1 == *tv1);

    ta1 = ita++;
    tv1 = itv++;
    REQUIRE(*ta1 == *tv1);

    REQUIRE(*ta1++ == *tv1++);
  }

  TEST_CASE("decrement")
  {
    int a[] = { 1, 2, 3 };
    std::vector<int> v = { 1, 2 , 3 };

    vector_iterator<vector<int>> ita(a+2);
    std::vector<int>::iterator itv(v.begin()+2);

    auto ta1 = --ita;
    auto tv1 = --itv;
    REQUIRE(*ta1 == *tv1);

    ta1 = ita--;
    tv1 = itv--;
    REQUIRE(*ta1 == *tv1);

    REQUIRE(*ta1-- == *tv1--);
  }

  TEST_CASE("+,-,+=,-=")
  {
    int a[] = { 1, 2, 3, 4, 5, 6 };

    vector_iterator<vector<int>> it(a);

    auto t = it + 2;
    REQUIRE(3 == *t);
    t -= 2;
    REQUIRE(1 == *t);
    t += 2;
    REQUIRE(3 == *t);

    t = t - 2;
    REQUIRE(1 == *t);

    t = t + 2;
    REQUIRE(3 == *t);

    t = 2 + t;
    REQUIRE(5 == *t);

    REQUIRE(4 == t - it);
    REQUIRE(-4 == it - t);

    REQUIRE(5 == it[4]);
  }

  TEST_CASE("std2 postfix increment")
  {
    std2::vector<int> v = { 1, 3, 5, 7, 11 };
    auto it = v.begin();
    ++it++ ++;
    it++ ++ ++;
  }

  TEST_CASE("array postfix increment")
  {
    int v[] = { 1, 3, 5, 7, 11 };
    int* it = v;
    ++it;
    it++;
  }


}


