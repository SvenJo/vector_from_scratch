#ifdef MSVC_CHECK_MEMORY_LEAKS
# include  "crtdbg.h"
#endif

#include <iostream>
#include <vector>
#include <array>
#include <string>
#include <tuple>
#include <cassert>
#include <limits>

#include "cstyle.h"
#include "step00.h"
#include "step01.h"
#include "step02.h"
#include "step03.h"
#include "step04.h"
#include "step05.h"
#include "step06.h"
#include "step07.h"
#include "step07a.h"
#include "step08.h"
#include "step09.h"
#include "step10.h"
#include "step11.h"

#include "stepN.h"

//#include "vector.hpp"


int main()
{
#ifdef MSVC_CHECK_MEMORY_LEAKS
  // check for memory leak and report the leaks
  _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG | _CRTDBG_LEAK_CHECK_DF);
  //_crtBreakAlloc = 37963;
#endif

  using std::cout;
  using std::endl;
  using std::boolalpha;

  cout << boolalpha;


  //*
  cstyle::f1(); // new[] / delete[]
  cstyle::f2(); // C style array
  cstyle::f3(); // std::vector<>
  cstyle::f4(); // std::array<>

  step0::f1(); // memory layout
  step1::f1(); // size / capacity
  step2::f1(); // vector<>(10)
  step3::f1(); // operator[], at(n)

  step4::f1(); // data(), front(), back()
  step5::f1(); // allocator

  step6::f1(); // resize()  
  step6::f2(); // reserve()
  step6::f3(); // push_back()

  step7::f1(); // constructors
  step7::f2(); // assignment operators
  step7::f3(); // assign()
  
  // alternative implementation
  step7a::f1(); // constructors
  step7a::f2(); // assignment operators
  step7a::f3(); // assign()
  
  step8::f1();
  step9::f1(); // compare operators
  
  step10::f1(); // pop_back()
  step10::f2(); // erase() single element
  step10::f3(); // erase() multiple elements
  step10::f4(); // erase-remove idiom
  // */
  step11::f1(); // emplace_back()
  step11::f2(); // simple_insert()
  step11::f3(); // insert()

  step11::f4(); // insert with move
  step11::f5(); // emplace

  //*
  // fragment empty base optimization
  stepN_1::f1(); 
  stepN_2::f1();
  stepN_3::f1();
  // */

  cout << "fin..." << endl;
}
