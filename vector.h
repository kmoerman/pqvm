#ifndef pqvm_vector_h
#define pqvm_vector_h

#include <iostream>
#include <memory>
#include <stdexcept>

/**
 * Fixed-length vector of objects of type T.
 * @WARNING Contrary to the std::vector, this container does not
 * construct or destruct its members. This container is desinged
 * primarily with numeric or simple compound types in mind, and
 * to be as STL compatible as possible without compromising
 * performance.
 */

template <class T, class A = std::allocator<T> >
class vector {
private:
    typedef vector<T, A> self_type;
    //typedef typename std::allocator_traits<A> allocator_traits;
public:
    typedef T value_type;
    typedef A allocator_type;
    typedef value_type& reference;
    typedef const value_type& const_reference;
/*  
    typedef typename allocator_traits::size_type size_type;
    typedef typename allocator_traits::difference_type difference_type;
    typedef typename allocator_traits::pointer pointer;
    typedef typename allocator_traits::const_pointer const_pointer;
*/
    typedef typename allocator_type::size_type size_type;
    typedef typename allocator_type::difference_type difference_type;
    typedef typename allocator_type::pointer pointer;
    typedef typename allocator_type::const_pointer const_pointer;
    typedef pointer iterator;
    typedef const_pointer const_iterator;

private:
    pointer  _begin;
    pointer  _end;
    allocator_type _alloc;

public:
    /*
     * Constructor.
     * Allocate: allocate space for n elements, or wait for allocation to be called later.
     */
    explicit vector (const size_type n, const allocator_type& a = allocator_type ()) :
    _begin (_alloc.allocate(n)), _end (_begin + n), _alloc (a) {}
    
    explicit vector (const allocator_type& a = allocator_type ()) :
    _begin (NULL), _end (NULL), _alloc (a) {}
    
    /*
     * Destructor.
     * Deallocation only, no destuctors are called on the member objects.
     */
    ~vector () {
        if (size() > 0)
            _alloc.deallocate(_begin, size());
    }
    
    /*
     * Storage.
     */
    inline void reserve (const size_type n) {
        if (size() > 0) return;
        _begin = _alloc.allocate(n);
        _end = _begin + n;
    }
    
    inline size_type size () const {
        return _end - _begin;
    }
    
    inline allocator_type get_allocator () const {
        return _alloc;
    }
    
    inline void empty () {
        if (size() == 0) return;
        _alloc.deallocate(_begin, size());
        _begin = NULL;
        _end = NULL;
    }
    
    inline void reset () {
        _begin = NULL;
        _end = NULL;
    }
    
    /*
     * Iterators.
     * We provide (constant and non-constant) random access iterators,
     * as transparent wrappers around array pointers.
     * @TODO To be STL complaint, add reverse iterators.
     */
    
public:
    inline iterator begin () {
        return _begin;
    }
    
    inline const_iterator begin () const {
        return _begin;
    }
    
    inline iterator end () {
        return _end;
    }
    
    inline const_iterator end () const {
        return _end;
    }
    
    /*
     * Element access.
     * The subscript operator provides access without bound checking.
     * The at function checks the index first.
     * The function front() and back() result in undefined behaviour
     * in empty vectors.
     */
    
    inline reference operator [] (size_type i) {
        return _begin[i];
    }
    
    inline const_reference operator [] (size_type i) const {
        return _begin[i];
    }
    
    inline reference front () {
        return _begin;
    }
    
    inline const_reference front () const {
        return _begin;
    }
    
    inline reference back () {
        return _end - 1;
    }
    
    inline const_reference back () const {
        return _end - 1;
    }
    
    reference at (size_type i) {
        if (0 < i && i < size()) return _begin[i];
        else throw std::out_of_range ("Index " + i + "out of range for vector of size " + size() + ".");
    }
    
    const_reference at (size_type i) const {
        if (0 < i && i < size()) return _begin[i];
        else throw std::out_of_range ("Index " + i + "out of range for vector of size " + size() + ".");
    }

};

#endif