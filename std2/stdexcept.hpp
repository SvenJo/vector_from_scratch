#pragma once

#include <stdexcept>
#include <string>

namespace std2
{
  using std::logic_error;
  using std::string;
  
  class out_of_range : public logic_error
  {
  public:
    explicit out_of_range(const string& what_arg) : logic_error(what_arg) {}
    explicit out_of_range(const char* what_arg) : logic_error(what_arg) {}
  };

  class length_error : public logic_error
  {
  public:
    explicit length_error(const string& what_arg) : logic_error(what_arg) {}
    explicit length_error(const char* what_arg) : logic_error(what_arg) {}
  };

}