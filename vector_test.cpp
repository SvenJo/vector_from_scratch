#ifdef MSVC_CHECK_MEMORY_LEAKS
#define _CRTDBG_MAP_ALLOC 
#include <stdlib.h>
#include <crtdbg.h>
# define CATCH_CONFIG_RUNNER
#else
# define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this 
#endif

#include <catch.hpp>

// GCC reports warning for the catch unit tests (REQUIRE( == ))
#ifdef __GNUG__
#pragma GCC diagnostic ignored "-Wparentheses"
#endif

#define XTOKENMERGE(x, y) x ## y
#define TOKENMERGE(x, y) XTOKENMERGE(x, y)

#include <vector.hpp>
#include <vector>

// include vector_test.h twice to use the same tests for std::vector and std2::vector
// VT_ = Vector Test

#define VT_NAMPESPACE std
#define VT_PRE "std: "
#define VT_STD
#include "vector_test.h"
#undef VT_NAMPESPACE
#undef VT_PRE
#undef VT_STD

#define VT_NAMPESPACE std2
#define VT_PRE "std2: "
#define VT_STD2
#include "vector_test.h"
#undef VT_NAMPESPACE
#undef VT_PRE
#undef VT_STD2

#ifdef MSVC_CHECK_MEMORY_LEAKS

int main(int argc, char* const argv[])
{
  // check for memory leak and report the leaks
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
  //_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF);
  _crtBreakAlloc = 163;

  int result = Catch::Session().run(argc, argv);

  //_CrtDumpMemoryLeaks();
  return result;
}

#endif
