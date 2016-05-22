cmake_minimum_required (VERSION 2.8)

project (vector_from_scratch)

### Compiler flags
# don't work for cmake 2.8.x
set(CMAKE_CXX_STANDARD 11)

if (MSVC)
  set_property(GLOBAL PROPERTY USE_FOLDERS ON)
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
  
  if (CHECK_MEMORY_LEAKS)
    # at the moment only for the vector test enabled!
    message("run progam in the debugger and check the output window!")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DMSVC_CHECK_MEMORY_LEAKS=1")
  endif ()

#elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -ggdb")
    
    if (CHECK_MEMORY_LEAKS)
      message("set -fsanitize=address -fno-omit-frame-pointer for memory leak checks")
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
    endif ()   
    
    # check for undefined behavior
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined -fno-omit-frame-pointer")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wcast-qual -Wcast-align -Wlogical-op -Wdeprecated-declarations -Wunused-local-typedefs ")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wshadow -Wstrict-aliasing")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wmissing-braces -Wmissing-field-initializers -Wformat=2 ")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wpointer-arith -Wstrict-overflow=5  ")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wundef -Wunreachable-code -Wfloat-equal -Wredundant-decls") 
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Winit-self -Wmissing-include-dirs -Wno-return-local-addr -Wswitch -Wswitch-bool -Wuninitialized ")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunused -Winvalid-memory-model -Wconversion -Wuseless-cast -Wparentheses -Wconditionally-supported ")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wtrampolines -Wbool-compare -Wsuggest-override -Wsuggest-final-methods -Wsuggest-final-types")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunknown-pragmas -Wmaybe-uninitialized")
    
    # to be discuss
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Winline -Wswitch-default -Wswitch-enum")
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror ")
    
endif()

set (HEADER_FILES
   std2/vector.hpp
   std2/memory.hpp
   std2/stdexcept.hpp
   std2/detail/algorithm.hpp
   std2/detail/vector_iterator.hpp
)

include_directories(${CMAKE_SOURCE_DIR}/std2)

set (DEMO_HEADER_FILES
  cstyle.h
  step00.h
  step01.h
  step02.h
  step03.h
  step04.h
  step05.h
  step06.h
  step07.h
  step07a.h
  step08.h
  step09.h
  step10.h
  step11.h
  stepN.h
)

set (DEMO_SOURCE_FILES
   demo.cpp
)

set (TEST_HEADER_FILES
   vector_test.h   
   )

set (TEST_SOURCE_FILES
   vector_test.cpp
   iterator_test.cpp
   allocator_test.cpp
)

add_executable(demo
   ${HEADER_FILES}
   ${DEMO_HEADER_FILES}
   ${DEMO_SOURCE_FILES}
)

include_directories(${CMAKE_SOURCE_DIR}/extern/catch)

add_executable(vector_test
   ${HEADER_FILES}
   ${TEST_HEADER_FILES}
   ${TEST_SOURCE_FILES}
)
