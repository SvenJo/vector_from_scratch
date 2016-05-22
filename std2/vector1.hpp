#pragma once

#include <initializer_list>

#include <stdexcept>
#include <tuple>
#include <limits>
#include <string>
#include <iterator>

namespace std2
{
  using std::logic_error;
  using std::string;
  using std::distance;
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

      using const_pointer_type = const pointer;
      using const_reference_type = typename CONT_TYPE::const_reference;
      using size_type = typename CONT_TYPE::size_type;


      vector_iterator() {}
      vector_iterator(pointer p) : p_(p) {}

      pointer operator->() { return p_; }
      const_pointer_type operator->() const { return p_; }

      reference operator*() { return *p_; }
      const_reference_type operator*() const { return *p_; }

      vector_iterator& operator++() //prefix increment
      {
        ++p_;
        return *this;
      }
      vector_iterator operator++(int) //postfix increment
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

      friend bool operator<(const vector_iterator& l, const vector_iterator& r) { return l.less(r); }
      friend bool operator>(const vector_iterator& l, const vector_iterator& r) { return r.less(l); }
      friend bool operator<=(const vector_iterator& l, const vector_iterator& r) { return l.less(r) || !l.not_equal(r); }
      friend bool operator>=(const vector_iterator& l, const vector_iterator& r) { return r.less(l) || !r.not_equal(l); }
      friend bool operator==(const vector_iterator& l, const vector_iterator& r) { return !l.not_equal(r); }
      friend bool operator!=(const vector_iterator& l, const vector_iterator& r) { return l.not_equal(r); }

      void swap(vector_iterator& other)
      {
        std::swap(p_, other.p_);
      }

      friend void swap(vector_iterator& l, vector_iterator& r)
      {
        l.swap(r);
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
    };
  } // detail;
  
  class out_of_range : public logic_error
  {
  public:
    explicit out_of_range(const string& what_arg) : logic_error(what_arg) {}
    explicit out_of_range(const char* what_arg) : logic_error(what_arg) {}
  };

  template<typename T /*, class Allocator = allocator<T> */ >
  class vector
  {
  public:
    // types:
    typedef T value_type;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef size_t size_type; // see 23.2 // implementation-defined!
    
    /*
    typedef T* iterator; // see 23.2 // implementation-defined!
    typedef const T*  const_iterator; // see 23.2 // implementation-defined!
    /*/
    typedef detail::vector_iterator<vector> iterator; // see 23.2 // implementation-defined!
    typedef const detail::vector_iterator<vector>  const_iterator; // see 23.2 // implementation-defined!

    //*/
    
    
    typedef ptrdiff_t difference_type;// see 23.2 // implementation-defined

    //typedef Allocator allocator_type;
    //typedef typename allocator_traits<Allocator>::pointer pointer;
    //typedef typename allocator_traits<Allocator>::const_pointer const_pointer;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
    
    
    // 23.3.6.2, construct/copy/destroy:
    vector() /*noexcept(noexcept(Allocator())) : vector(Allocator())*/ { }
    //explicit vector(const Allocator&) noexcept;
    explicit vector(size_type n /*, const Allocator& = Allocator()*/)
    {
      _realloc(n, n, [] { return T(); });
    }
    vector(size_type n, const T& value /*, const Allocator& = Allocator()*/)
    {
      _realloc(n, n, [value] { return value; });
    }
    
    template <class InputIterator 
      //, class = typename std::enable_if<!std::is_same<typename std::iterator_traits<InputIterator>::value_type, void>::value>::type>
      , class = typename std::iterator_traits<InputIterator>::value_type>
      //>
    vector(InputIterator first, InputIterator last /*, const Allocator& = Allocator()*/)
    {
      reserve(std::distance(first, last));

      for (auto it = first; it != last; ++it) {
        push_back(*it);
      }

    }
    
    vector(const vector& other)
    {
      std::tie(begin_, capacity_) = _alloc(other.size());
      end_ = begin_ + other.size();
      for(size_type i = 0; i < size(); ++i) {
        begin_[i] = other.begin_[i];
      }
    }
    
    vector(vector&& other) noexcept
    {
      std::swap(begin_, other.begin_);
      std::swap(end_, other.end_);
      std::swap(capacity_, other.capacity_);
    }
    
    //vector(const vector&, const Allocator&);
    //vector(vector&&, const Allocator&);
    
    vector(std::initializer_list<T> init /*, const Allocator& = Allocator()*/)
    {
      std::tie(begin_, capacity_) = _alloc(init.size());
      end_ = begin_ + init.size();
      for(size_type i = 0; i < size(); ++i) {
        begin_[i] = *(init.begin() + i);
      }      
    }
    
    ~vector()
    {
      _dealloc(begin_, end_, capacity_);
    }
    
    vector& operator=(const vector& x)
    {
      // exception save, no self reference check, 
      // but slower / no reuse of memory
      vector v(x);
      swap(v);
      return *this;
    }

    vector& operator=(vector&& x) /*noexcept(allocator_traits<Allocator>::propagate_on_container_move_assignment::value || allocator_traits<Allocator>::is_always_equal::value);*/
    {
      vector v(std::move(x));
      swap(v);
      return *this;
    }

    vector& operator=(std::initializer_list<T> il)
    {
      vector v(il);
      swap(v);
      return *this;
    }

    template <class InputIterator
      //, class = typename std::enable_if<!std::is_same<typename std::iterator_traits<InputIterator>::value_type, void>::value>::type>
      , class = typename std::iterator_traits<InputIterator>::value_type>
      //>
    void assign(InputIterator first, InputIterator last)
    {
      vector v(first, last);
      swap(v);
    }

    void assign(size_type n, const T& u)
    {
      vector v(n, u);
      swap(v);
    }

    void assign(std::initializer_list<T> il)
    {
      vector v(il);
      swap(v);
    }
    
    //allocator_type get_allocator() const noexcept;
    
    // ...
    // iterators:
    iterator begin() noexcept
    {
      return begin_;
    }
    
    const_iterator begin() const noexcept
    {
      return begin_;
    }
    
    iterator end() noexcept
    {
      return end_; 
    }
    
    const_iterator end() const noexcept
    {
      return end_;
    }
    
    //reverse_iterator rbegin() noexcept
    //const_reverse_iterator rbegin() const noexcept;
    //reverse_iterator rend() noexcept;
    //const_reverse_iterator rend() const noexcept;
    const_iterator cbegin() const noexcept
    {
      return begin_;
    }
    
    const_iterator cend() const noexcept
    {
      return end_;
    }
    //const_reverse_iterator crbegin() const noexcept;
    //const_reverse_iterator crend() const noexcept;

    // 23.3.6.3, capacity:
    bool empty() const noexcept
    {
      return begin_ == end_;
    }

    size_type size() const noexcept
    {
      return end_ - begin_;
    }

    size_type max_size() const noexcept
    {
      // from http://en.cppreference.com/w/cpp/container/vector/max_size
      //return std::numeric_limits<size_type>::max();

      // same results as MSVC++
      return std::numeric_limits<size_type>::max() / sizeof(T);
    }

    size_type capacity() const noexcept
    {
      return capacity_ - begin_;
    }

    void resize(size_type n)
    {
      if (n == size()) return;

      // 0 <= n < size();
      if (_shrink(n)) return;

      // size() < n <= capacity();
      if (_expand(n, [] { return T(); })) return;

      // n > capacity => _alloc memory
      _realloc(n, n, [] { return T(); });
    }

    void resize(size_type n, const T& c)
    {
      // 0 <= n < size();
      if (_shrink(n)) return;

      // size() < n <= capacity();
      if (_expand(n, [] { return T(); })) return;

      // n > capacity => _alloc memory
      _realloc(n, n, [&c] { return c; });
    }

    void reserve(size_type n)
    {
      if (capacity() >= n) return;

      _realloc(size(), n, [] { assert(!"don't default initialization"); return T(); });
    }

    void shrink_to_fit()
    {
      if (capacity() == size()) return;

      _realloc(size(), size(), [] { assert(!"don't default initialization"); return T(); });
    }

    // element access:
    reference operator[](size_type n)
    {
      return *(begin_ + n);
    }
    const_reference operator[](size_type n) const
    {
      return *(begin_ + n);
    }
    const_reference at(size_type n) const
    {
      if (n >= size()) throw out_of_range("out_of_range: vector");
      return *(begin_ + n);
    }
    reference at(size_type n)
    {
      if (n >= size()) throw out_of_range("out_of_range: vector");
      return *(begin_ + n);
    }
    reference front()
    {
      return *begin_;
    }
    const_reference front() const
    {
      return *begin_;
    }
    
    reference back()
    {
      return *(end_ -1);
    }
    const_reference back() const
    {
      return *(end_ -1);
    }
    
    // 23.3.6.4, data access
    T* data() noexcept
    {
      return begin_;
    }
    const T* data() const noexcept
    {
      return begin_;
    }
    
    // 23.3.6.5, modifiers:
    //template <class... Args> void emplace_back(Args&&... args);
    void push_back(const T& c)
    {
      /*
      const auto n = size() + 1; 
      // size() < n <= capacity();
      if (_expand(n, [&c] { return c; })) return;

      // n > capacity => _alloc memory
      _realloc(n, n, [&c] { return c; });
      /*/
      const auto n = size() + 1; 
      // size() < n <= capacity();
      if (_expand(n, [&c] { return c; })) return;

      // n > capacity => _alloc memory
      const auto new_capacity = size() + size() / 2 + 1; 
      _realloc(n, new_capacity, [&c] { return c; });
      //*/
    }
    
    void push_back(T&& c)
    {
      /*
      const auto n = size() + 1; 
      // size() < n <= capacity();
      if (_expand(n, [&c] { return c; })) return;

      // n > capacity => _alloc memory
      _realloc(n, n, [&c] { return c; });
      /*/
      const auto n = size() + 1; 
      // size() < n <= capacity();
      if (_expand(n, [&c] { return std::move(c); })) return;

      // n > capacity => _alloc memory
      const auto new_capacity = size() + size() / 2 + 1; 
      _realloc(n, new_capacity, [&c] { return std::move(c); });
      //*/      
    }
    
    void pop_back()
    {
      resize(size()-1);
    }
    
    //template <class... Args> iterator emplace(const_iterator position, Args&&... args);
    
    //iterator insert(const_iterator position, const T& x);
    //iterator insert(const_iterator position, T&& x);
    //iterator insert(const_iterator position, size_type n, const T& x);
    //template <class InputIterator>
    //iterator insert(const_iterator position, InputIterator first, InputIterator last);
    //iterator insert(const_iterator position, initializer_list<T> il);
    //iterator erase(const_iterator position);
    //iterator erase(const_iterator first, const_iterator last);
    void swap(vector& other) /*//noexcept(allocator_traits<Allocator>::propagate_on_container_swap::value ||
    //allocator_traits<Allocator>::is_always_equal::value);*/
    {
      std::swap(begin_, other.begin_);
      std::swap(end_, other.end_);
      std::swap(capacity_, other.capacity_);
    }

    void clear() noexcept
    {
      // resize(0); // resize may throw. (not in this case but in the interface)
      //_dealloc(begin_, end_, capacity_);

      _shrink(0);
    }

  private:
    T* begin_ = nullptr;
    T* end_ = nullptr;
    T* capacity_ = nullptr;

    std::tuple<T*, T*> _alloc(size_type n)
    {
      // naive approach
      T* b = n ? new T[n] : nullptr;
      T* e = b + n;

      return std::make_tuple(b, e);
    }

    void _dealloc(T*& b, T*&e, T*& c)
    {
      delete[] b;
      c = e = b = nullptr;
    }
    
    // internal implementation:

    template<typename INIT>
    void _realloc(size_type new_size, size_type new_capacity, INIT init_elem)
    {
      T *b{ nullptr }, *e{ nullptr }, *c{ nullptr };

      std::tie(b, c) = _alloc(new_capacity);
      e = b + new_size;
      mini_guart guart = { b };

      // copy/move existing values
      for (size_type i = 0; i < size(); ++i) {
        // b[i] = std::move(begin_[i]); only if T::copy-ctor == notrow...
        b[i] = begin_[i];
      }

      // init new add values
      for (size_type i = size(); i < new_size; ++i) {
        b[i] = init_elem();
      }

      std::swap(b, begin_);
      std::swap(e, end_);
      std::swap(c, capacity_);
    }

    bool _shrink(size_type n) noexcept
    {
      if (n < size()) {
        // todo missing dtor call for element between n <-> size()
        end_ = begin_ + n;
        return true;
      }
      return false;
    }

    template<typename INIT>
    bool _expand(size_type n, INIT init_elem)
    {
      assert(n >= size());

      if (n <= capacity()) {
        for (size_type i = size(); i < n; i++) {
          *(begin_ + i) = init_elem();
        }
        end_ = begin_ + n;
        return true;
      }
      return false;
    }

    struct mini_guart
    {
      ~mini_guart() { delete[] p; }
      T*& p;
    };
  };


  template <class T/*, class Allocator*/>
  bool operator==(const vector<T/*, Allocator*/>& x,const vector<T /*,Allocator*/>& y);
  
  template <class T/*, class Allocator*/>
  bool operator< (const vector<T /*,Allocator*/>& x,const vector<T /*,Allocator*/>& y);
  
  template <class T/*, class Allocator*/>
  bool operator!=(const vector<T /*,Allocator*/>& x,const vector<T /*,Allocator*/>& y);
  
  template <class T/*, class Allocator*/>
  bool operator> (const vector<T /*,Allocator*/>& x,const vector<T /*,Allocator*/>& y);
  
  template <class T/*, class Allocator*/>
  bool operator>=(const vector<T /*,Allocator*/>& x,const vector<T /*,Allocator*/>& y);
  
  template <class T/*, class Allocator*/>
  bool operator<=(const vector<T /*,Allocator*/>& x,const vector<T /*,Allocator*/>& y);
  
  template <class T/*, class Allocator*/>
  void swap(vector<T /*,Allocator*/>& x, vector<T /*,Allocator*/>& y)
  {
    x.swap(y);
  }

  //template <class /*Allocator*/> class vector<bool,Allocator>;  

  // hash support
  //template <class T> struct hash;
  //template <class Allocator> struct hash<vector<bool, Allocator> >;

}
