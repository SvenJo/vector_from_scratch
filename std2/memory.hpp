#pragma once

#include <cstddef> // ptrdiff_t
#include <new>     // bad_alloc
#include <limits>  // numeric_limits
#include <utility> // forward

// C++11: noexcept, C++03 throw()
#define CPP11_ALLOCATOR

#ifdef CPP11_ALLOCATOR
#define NOEXCEPT noexcept
#define NOEXCEPT11 noexcept
#define THROW98

#else // C++98/03 style
#define NOEXCEPT throw()
#define NOEXCEPT11
#define THROW98 throw() // for d-tors
#endif

namespace std2
{
  // 20.6.3, pointer traits
  // template <class Ptr> struct pointer_traits;
  // template <class T> struct pointer_traits<T*>;

  // 20.6.4, pointer safety
  // enum class pointer_safety { relaxed, preferred, strict };
  // void declare_reachable(void *p);
  // template <class T> T *undeclare_reachable(T *p);
  // void declare_no_pointers(char *p, size_t n);
  // void undeclare_no_pointers(char *p, size_t n);
  // pointer_safety get_pointer_safety() noexcept;

  // 20.6.5, pointer alignment function
  //void *align(std::size_t alignment, std::size_t size, void *&ptr, std::size_t& space);
  
  // 20.6.8, allocator traits
  template <class Alloc> struct allocator_traits {
    typedef Alloc allocator_type;

    typedef typename Alloc::value_type value_type;

    typedef typename allocator_type::pointer pointer;
    typedef typename allocator_type::const_pointer const_pointer;
    typedef void* void_pointer;
    typedef const void* const_void_pointer;

    typedef typename allocator_type::difference_type difference_type;
    typedef typename allocator_type::size_type size_type;

    // typedef see below propagate_on_container_copy_assignment;
    // typedef see below propagate_on_container_move_assignment;
    // typedef see below propagate_on_container_swap;

    /* 23.2.1 General container requirements
(7)
Unless otherwise specified, all containers defined in this clause obtain memory using an allocator (see 17.6.3.5).
Copy constructors for these container types obtain an allocator by calling allocator_traits<allocator_type>::select_on_container_copy_construction on their first parameters. Move constructors obtain
an allocator by move construction from the allocator belonging to the container being moved. Such move
construction of the allocator shall not exit via an exception. All other constructors for these container types
take an Allocator& argument (17.6.3.5), an allocator whose value type is the same as the container�s value
type. [ Note: If an invocation of a constructor uses the default value of an optional allocator argument, then
the Allocator type must support value initialization. � end note ] A copy of this allocator is used for any
memory allocation performed, by these constructors and by all member functions, during the lifetime of each
container object or until the allocator is replaced. The allocator may be replaced only via assignment or
swap(). Allocator replacement is performed by copy assignment, move assignment, or swapping of the allocator only if allocator_traits<allocator_type>::propagate_on_container_copy_assignment::value,
allocator_traits<allocator_type>::propagate_on_container_move_assignment::value, or allocator_traits<allocator_type>::propagate_on_container_swap::value is true within the implementation of the corresponding container operation. The behavior of a call to a container�s swap function is undefined unless the objects being swapped have allocators that compare equal or allocator_traits<allocator_-
type>::propagate_on_container_swap::value is true. In all container types defined in this Clause, the
member get_allocator() returns a copy of the allocator used to construct the container or, if that allocator
has been replaced, a copy of the most recent replacement.
    */


    //template <class T> using rebind_alloc = see below;
    //template <class T> using rebind_traits = allocator_traits<rebind_alloc<T> >;

    static pointer allocate(Alloc& alloc, size_type n) { return alloc.allocate(n); }
    static pointer allocate(Alloc& alloc, size_type n, const_void_pointer hint) { return alloc.allocate(n, hint); }

    static void deallocate(Alloc& alloc, pointer p, size_type n) { alloc.deallocate(p, n); }

    template <class T, class... Args>
    static void construct(Alloc& alloc, T* p, Args&&... args) { alloc.construct(p, std::forward<Args>(args)...); }

    template <class T>
    static void destroy(Alloc& alloc, T* p) { alloc.destroy(p); }

    static size_type max_size(const Alloc& alloc) { return alloc.max_size(); }
    //static Alloc select_on_container_copy_construction(const Alloc& rhs);
  };

  // 20.6.9, the default allocator: (C++98: 20.4.1)
  template <class T> class allocator;

  // specialize for void:
  template <> class allocator<void> {
  public:
    typedef void* pointer;
    typedef const void* const_pointer;
    // reference-to-void members are impossible.
    typedef void value_type;
    template <class U> struct rebind { typedef allocator<U> other; };
  };

  template <class T> class allocator {
  public:
    typedef size_t size_type;
    typedef std::ptrdiff_t difference_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef T value_type;
    template <class U> struct rebind { typedef allocator<U> other; };

    allocator() NOEXCEPT {}
    allocator(const allocator&) NOEXCEPT {}
    template <class U> allocator(const allocator<U>&) NOEXCEPT {}
    ~allocator() THROW98 {}

    pointer address(reference x) const NOEXCEPT11 { return &x; }
    const_pointer address(const_reference x) const NOEXCEPT11 { return &x; }

    pointer allocate(size_type n, allocator<void>::const_pointer /*hint*/ = 0)
    {
      if (n > max_size()) throw std::bad_alloc(); // TODO maybe length_error
      return (pointer)::operator new[](n * sizeof(T));
    }
    void deallocate(pointer p, size_type /*n*/)
    {
      ::operator delete[](p);
    }

    size_type max_size() const NOEXCEPT
    {
      return std::numeric_limits<size_type>::max() / sizeof(T);
    }

#ifdef CPP11_ALLOCATOR
    template<class U, class... Args>
    void construct(U* p, Args&&... args)
    {
      new(p)U(std::forward<Args>(args)...);
    }

#ifdef  _MSC_VER 
#pragma warning(push)
#pragma warning(disable: 4100) // false positive: 'p': unreferenced formal parameter 
#endif //  _MSC_VER 

    template <class U>
    void destroy(U* p)
    {
      p->~U();
    }
#ifdef  _MSC_VER 
#pragma warning(pop)
#endif //  _MSC_VER 

#else
    void construct(pointer p, const T& val)
    {
      new((void*)p)T(val);
    }

    void destroy(pointer p)
    {
      p->~T();
    }
#endif
  };

  // template <class T, class U>
  // bool operator==(const allocator<T>&, const allocator<U>&) noexcept;
  // template <class T, class U>
  // bool operator!=(const allocator<T>&, const allocator<U>&) noexcept;

  // 20.6.10, raw storage iterator:
  // template <class OutputIterator, class T> class raw_storage_iterator;

  // 20.6.11, temporary buffers:
  // template <class T>
  // pair<T*, ptrdiff_t> get_temporary_buffer(ptrdiff_t n) noexcept;
  // template <class T>
  // void return_temporary_buffer(T* p);

  // 20.6.12, specialized algorithms:
  // template <class T> T* addressof(T& r) noexcept;
  // template <class InputIterator, class ForwardIterator>
  // ForwardIterator uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result);
  // template <class InputIterator, class Size, class ForwardIterator>
  // ForwardIterator uninitialized_copy_n(InputIterator first, Size n, ForwardIterator result);
  // template <class ForwardIterator, class T>
  // void uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& x);
  // template <class ForwardIterator, class Size, class T>
  // ForwardIterator uninitialized_fill_n(ForwardIterator first, Size n, const T& x);

  // 20.7.1 class template unique_ptr:
  // template <class T> class default_delete;
  // template <class T> class default_delete<T[]>;
  // template <class T, class D = default_delete<T>> class unique_ptr;
  // template <class T, class D> class unique_ptr<T[], D>;

  // template <class T1, class D1, class T2, class D2>
  // bool operator==(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y);
  // template <class T1, class D1, class T2, class D2>
  // bool operator!=(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y);
  // template <class T1, class D1, class T2, class D2>
  // bool operator<(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y);
  // template <class T1, class D1, class T2, class D2>
  // bool operator<=(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y);
  // template <class T1, class D1, class T2, class D2>
  // bool operator>(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y);
  // template <class T1, class D1, class T2, class D2>
  // bool operator>=(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y);

  // template <class T, class D>
  // bool operator==(const unique_ptr<T, D>& x, nullptr_t) noexcept;
  // template <class T, class D>
  // bool operator==(nullptr_t, const unique_ptr<T, D>& y) noexcept;
  // template <class T, class D>
  // bool operator!=(const unique_ptr<T, D>& x, nullptr_t) noexcept;
  // template <class T, class D>
  // bool operator!=(nullptr_t, const unique_ptr<T, D>& y) noexcept;

  // template <class T, class D>
  // bool operator<(const unique_ptr<T, D>& x, nullptr_t);
  // template <class T, class D>
  // bool operator<(nullptr_t, const unique_ptr<T, D>& y);
  // template <class T, class D>
  // bool operator<=(const unique_ptr<T, D>& x, nullptr_t);
  // template <class T, class D>
  // bool operator<=(nullptr_t, const unique_ptr<T, D>& y);
  // template <class T, class D>
  // bool operator>(const unique_ptr<T, D>& x, nullptr_t);
  // template <class T, class D>
  // bool operator>(nullptr_t, const unique_ptr<T, D>& y);
  // template <class T, class D>
  // bool operator>=(const unique_ptr<T, D>& x, nullptr_t);
  // template <class T, class D>
  // bool operator>=(nullptr_t, const unique_ptr<T, D>& y);

  // 20.7.2.1, class bad_weak_ptr:
  // class bad_weak_ptr;

  // 20.7.2.2, class template shared_ptr:
  // template<class T> class shared_ptr;

  // 20.7.2.2.7, shared_ptr comparisons:
  // template<class T, class U>
  // bool operator==(shared_ptr<T> const& a, shared_ptr<U> const& b) noexcept;
  // template<class T, class U>
  // bool operator!=(shared_ptr<T> const& a, shared_ptr<U> const& b) noexcept;
  // template<class T, class U>
  // bool operator<(shared_ptr<T> const& a, shared_ptr<U> const& b) noexcept;
  // template<class T, class U>
  // bool operator>(shared_ptr<T> const& a, shared_ptr<U> const& b) noexcept;
  // template<class T, class U>
  // bool operator<=(shared_ptr<T> const& a, shared_ptr<U> const& b) noexcept;
  // template<class T, class U>
  // bool operator>=(shared_ptr<T> const& a, shared_ptr<U> const& b) noexcept;

  // template <class T>
  // bool operator==(const shared_ptr<T>& x, nullptr_t) noexcept;
  // template <class T>
  // bool operator==(nullptr_t, const shared_ptr<T>& y) noexcept;
  // template <class T>
  // bool operator!=(const shared_ptr<T>& x, nullptr_t) noexcept;
  // template <class T>
  // bool operator!=(nullptr_t, const shared_ptr<T>& y) noexcept;
  // template <class T>
  // bool operator<(const shared_ptr<T>& x, nullptr_t) noexcept;
  // template <class T>
  // bool operator<(nullptr_t, const shared_ptr<T>& y) noexcept;
  // template <class T>
  // bool operator<=(const shared_ptr<T>& x, nullptr_t) noexcept;
  // template <class T>
  // bool operator<=(nullptr_t, const shared_ptr<T>& y) noexcept;
  // template <class T>
  // bool operator>(const shared_ptr<T>& x, nullptr_t) noexcept;
  // template <class T>
  // bool operator>(nullptr_t, const shared_ptr<T>& y) noexcept;
  // template <class T>
  // bool operator>=(const shared_ptr<T>& x, nullptr_t) noexcept;
  // template <class T>
  // bool operator>=(nullptr_t, const shared_ptr<T>& y) noexcept;

  // 20.7.2.2.8, shared_ptr specialized algorithms:
  // template<class T> void swap(shared_ptr<T>& a, shared_ptr<T>& b) noexcept;

  // 20.7.2.2.9, shared_ptr casts:
  // template<class T, class U>
  // shared_ptr<T> static_pointer_cast(shared_ptr<U> const& r) noexcept;
  // template<class T, class U>
  // shared_ptr<T> dynamic_pointer_cast(shared_ptr<U> const& r) noexcept;
  // template<class T, class U>
  // shared_ptr<T> const_pointer_cast(shared_ptr<U> const& r) noexcept;

  // 20.7.2.2.10, shared_ptr get_deleter:
  // template<class D, class T> D* get_deleter(shared_ptr<T> const& p) noexcept;

  // 20.7.2.2.11, shared_ptr I/O:
  // template<class E, class T, class Y>
  // basic_ostream<E, T>& operator<< (basic_ostream<E, T>& os, shared_ptr<Y> const& p);

  // 20.7.2.3, class template weak_ptr:
  // template<class T> class weak_ptr;

  // 20.7.2.3.6, weak_ptr specialized algorithms:
  // template<class T> void swap(weak_ptr<T>& a, weak_ptr<T>& b) noexcept;

  // 20.7.2.3.7, class template owner_less:
  // template<class T> class owner_less;

  // 20.7.2.4, class template enable_shared_from_this:
  // template<class T> class enable_shared_from_this;

  // 20.7.2.5, shared_ptr atomic access:
  // template<class T>
  // bool atomic_is_lock_free(const shared_ptr<T>* p);

  // template<class T>
  // shared_ptr<T> atomic_load(const shared_ptr<T>* p);
  // template<class T>
  // shared_ptr<T> atomic_load_explicit(const shared_ptr<T>* p, memory_order mo);

  // template<class T>
  // void atomic_store(shared_ptr<T>* p, shared_ptr<T> r);
  // template<class T>
  // void atomic_store_explicit(shared_ptr<T>* p, shared_ptr<T> r, memory_order mo);

  // template<class T>
  // shared_ptr<T> atomic_exchange(shared_ptr<T>* p, shared_ptr<T> r);
  // template<class T>
  // shared_ptr<T> atomic_exchange_explicit(shared_ptr<T>* p, shared_ptr<T> r, memory_order mo);

  // template<class T>
  // bool atomic_compare_exchange_weak(shared_ptr<T>* p, shared_ptr<T>* v, shared_ptr<T> w);
  // template<class T>
  // bool atomic_compare_exchange_strong(shared_ptr<T>* p, shared_ptr<T>* v, shared_ptr<T> w);
  // template<class T>
  // bool atomic_compare_exchange_weak_explicit(shared_ptr<T>* p, shared_ptr<T>* v, shared_ptr<T> w, memory_order success, memory_order failure);
  // template<class T>
  // bool atomic_compare_exchange_strong_explicit(shared_ptr<T>* p, shared_ptr<T>* v, shared_ptr<T> w, memory_order success, memory_order failure);

  // 20.7.2.6 hash support
  // template <class T> struct hash;
  // template <class T, class D> struct hash<unique_ptr<T, D> >;
  // template <class T> struct hash<shared_ptr<T> >;

  // D.10, auto_ptr (deprecated)
  // template <class X> class auto_ptr;





}
