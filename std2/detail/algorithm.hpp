#pragma once

namespace std2
{
  namespace detail
  {
    // std::equal with 3 iterator parameters
    // unsafe without external size check
    template<class InputIt1, class InputIt2>
    bool _equal(InputIt1 xstart, InputIt1 xend, InputIt2 ystart)
    {
      InputIt1 xit = xstart; InputIt2 yit = ystart;
      for (; xit != xend; ++xit, ++yit) {
        if (*xit != *yit) return false;
      }
      return true;
    }
    
    // std::equal with 4 iterator parameters are new in C++14
    template<class InputIt1, class InputIt2>
    bool _equal(InputIt1 xstart, InputIt1 xend, InputIt2 ystart, InputIt2 yend)
    {
      InputIt1 xit = xstart; InputIt2 yit = ystart;
      for (; xit != xend && yit != yend; ++xit, ++yit) {
        if (*xit != *yit) return false;
      }
      //return true;
      return xit == xend && yit == yend;
    }
  }
}
