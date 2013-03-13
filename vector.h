#ifndef pqvm_vector_h
#define pqvm_vector_h

#include <memory>
#include <cstddef>
#include <algorithm>
#include <stdexcept>

/**
 * Fixed-length vector of objects of type T.
 * @WARNING Contrary to the std::vector, this vector does not
 * construct or destruct its members. This container is desinged
 * to be as STL compatible as possible without compromising performance,
 * primarily with numeric or simple compound types in mind.
 */

template <class T, class A = std::allocator<T> >
class vector {
private:
    typedef vector<T, A> self_type;
    typedef typename std::allocator_traits<A> allocator_traits;
public:
    typedef T value_type;
    typedef A allocator_type;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef typename allocator_traits::size_type size_type;
    typedef typename allocator_traits::difference_type difference_type;
    typedef typename allocator_traits::pointer pointer;
    typedef typename allocator_traits::const_pointer const_pointer;
    typedef pointer iterator;
    typedef const_pointer const_iterator;

public:
    allocator_type __alloc;
    const pointer __begin;
    const pointer __end;

public:
    /*
     * Constructor.
     * Allocate: allocate space for n elements.
     * Copy: allocate space for n elements and copy them form the original.
     */
    explicit vector (const size_type n, const allocator_type& a = allocator_type ()):
    __alloc (a),
    __begin (__alloc.allocate(n)),
    __end (__begin + n) {}
    
    vector (const self_type& v):
    __alloc (v.__alloc),
    __begin (__alloc.allocate(v.size())),
    __end (__begin + v.size()) {
        std::copy<const_iterator, iterator> (v.begin(), v.end(), __begin);
    }
    
    /*
     * Destructor.
     * Basic destruction only. No destuctors are called on the member objects.
     */
    ~vector () {
        __alloc.deallocate(__begin, size());
    }
    
    /*
     * Storage.
     */
    inline size_type size () const {
        return static_cast<size_type>(__end - __begin);
    }
    
    inline allocator_type get_allocator () const {
        return __alloc;
    }
    
    /*
     * Iterators.
     * We provide (constant and non-constant) random access iterators,
     * as transparent wrappers around array pointers.
     * @TODO to be STL copmlaint, add reverse iterators.
     */
    
public:
    inline iterator begin () {
        return __begin;
    }
    
    inline const_iterator begin () const {
        return __begin;
    }
    
    inline iterator end () {
        return __end;
    }
    
    inline const_iterator end () const {
        return __end;
    }
    
    /*
     * Element access.
     */
    
    inline reference operator [] (size_type n) {
        return __begin[n];
    }
    
    inline const_reference operator [] (size_type n) const {
        return __begin[n];
    }
    
    reference at (size_type n) {
        if (0 < n && n < size()) return __begin[n];
        else throw std::out_of_range ("Index " + n + "out of range for vector of size " + size() + ".");
    }
    
    const_reference at (size_type n) const {
        if (0 < n && n < size()) return __begin[n];
        else throw std::out_of_range ("Index " + n + "out of range for vector of size " + size() + ".");
    }
    
    inline reference front () {
        return at(0);
    }
    
    inline const_reference front () const {
        return at(0);
    }
    
    inline reference back () {
        return at(size() - 1);
    }

    inline const_reference back () const {
        return at(size() - 1);
    }

};

#endif
