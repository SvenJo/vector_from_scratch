#pragma once

#include <iterator>

namespace std2
{
  using std::random_access_iterator_tag;

  namespace detail {

    template<typename CONT_TYPE>
    class vector_iterator
    {
    public:
      typedef typename CONT_TYPE::difference_type difference_type; //almost always ptrdif_t
      typedef typename CONT_TYPE::value_type value_type; //almost always T
      typedef typename CONT_TYPE::reference reference; //almost always T& or const T&
      typedef typename CONT_TYPE::value_type* pointer; //almost always T* or const T*
      typedef random_access_iterator_tag iterator_category;  //usually std::forward_iterator_tag or similar

      using const_pointer_type = const pointer; // GCC Warning: type qualifiers ignored on function return type [-Werror=ignored-qualifiers]
      using const_reference_type = typename CONT_TYPE::const_reference;
      using size_type = typename CONT_TYPE::size_type;

      vector_iterator() {}
      explicit vector_iterator(pointer p) : p_(p) {}

      pointer operator->() { return p_; }

#ifdef __GNUG__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#endif
      const_pointer_type operator->() const { return p_; }
#ifdef __GNUG__
#pragma GCC diagnostic pop
#endif

      reference operator*() { return *p_; }
      const_reference_type operator*() const { return *p_; }

      vector_iterator& operator++() //prefix increment: ++it
      {
        ++p_;
        return *this;
      }
      vector_iterator operator++(int) //postfix increment: it++
      {
        vector_iterator i(*this);
        ++p_;
        return i;
      }
      vector_iterator& operator--() //prefix increment
      {
        --p_;
        return *this;
      }
      vector_iterator operator--(int) //postfix increment
      {
        vector_iterator i(*this);
        --p_;
        return i;
      }

      vector_iterator& operator+=(size_type n)
      {
        p_ += n;
        return *this;
      }

      vector_iterator& operator-=(size_type n)
      {
        p_ -= n;
        return *this;
      }

      reference operator[](size_type n)
      {
        return *(p_ + n);
      }
      const_reference_type operator[](size_type n) const
      {
        return *(p_ + n);
      }

      friend vector_iterator operator+(const vector_iterator& it, size_type n)
      {
        return vector_iterator(it) += n;
      }
      friend vector_iterator operator+(size_type n, const vector_iterator& it)
      {
        return vector_iterator(it) += n;
      }
      friend vector_iterator operator-(const vector_iterator& it, size_type n)
      {
        return vector_iterator(it) -= n;
      }
      friend difference_type operator-(const vector_iterator& l, const vector_iterator& r)
      {
        return l.diff(r);
      }

      void swap(vector_iterator& other)
      {
        std::swap(p_, other.p_);
      }

    private:
      bool not_equal(const vector_iterator& other) const 
      { 
        return p_ != other.p_; 
      }
      bool less(const vector_iterator& other) const 
      { 
        return p_ < other.p_; 
      }

      difference_type diff(const vector_iterator& other) const
      {
        return p_ - other.p_;
      }

    private:
      pointer p_ = nullptr;

      friend bool operator<(const vector_iterator& l, const vector_iterator& r) { return l.less(r); }
      friend bool operator>(const vector_iterator& l, const vector_iterator& r) { return r.less(l); }
      friend bool operator<=(const vector_iterator& l, const vector_iterator& r) { return l.less(r) || !l.not_equal(r); }
      friend bool operator>=(const vector_iterator& l, const vector_iterator& r) { return r.less(l) || !r.not_equal(l); }
      friend bool operator==(const vector_iterator& l, const vector_iterator& r) { return !l.not_equal(r); }
      friend bool operator!=(const vector_iterator& l, const vector_iterator& r) { return l.not_equal(r); }

      friend void swap(vector_iterator& l, vector_iterator& r) { l.swap(r); }
    };
  } // detail;
  
}
