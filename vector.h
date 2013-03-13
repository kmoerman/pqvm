#ifndef pqvm_vector_h
#define pqvm_vector_h

#include <memory>
#include <stdexcept>

/**
 * Fixed-length vector of objects of type T.
 * @WARNING Contrary to the std::vector, this vector does not
 * construct or destruct its members. This container is desinged
 * primarily with numeric or simple compound types in mind,
 * and to be as STL compatible as possible without compromising
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
    allocator_type __alloc;
    const pointer __begin;
    const pointer __end;

public:
    /*
     * Constructor.
     * Allocate: allocate space for n elements.
     * //Copy: allocate space for n elements and copy them from the original.
     */
    explicit vector (const size_type n, const allocator_type& a = allocator_type ()) :
    __alloc (a),
    __begin (__alloc.allocate(n)),
    __end   (__begin + n) {}
    
/* 
    vector (const self_type& v) :
    __alloc (v.__alloc),
    __begin (__alloc.allocate(v.size())),
    __end   (__begin + v.size()) {
        std::copy(v.begin(), v.end(), __begin);
    }
 */
    
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
     * @TODO To be STL complaint, add reverse iterators.
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
     * The subscript operator provides access without bound checking.
     * The at function checks the index first.
     * The function front() and back() result in undefined behaviour
     * in empty vectors.
     */
    
    inline reference operator [] (size_type i) {
        return __begin[i];
    }
    
    inline const_reference operator [] (size_type i) const {
        return __begin[i];
    }
    
    inline reference front () {
        return __begin;
    }
    
    inline const_reference front () const {
        return __begin;
    }
    
    inline reference back () {
        return __end - 1;
    }
    
    inline const_reference back () const {
        return __end - 1;
    }
    
    reference at (size_type i) {
        if (0 < i && i < size()) return __begin[i];
        else throw std::out_of_range ("Index " + i + "out of range for vector of size " + size() + ".");
    }
    
    const_reference at (size_type i) const {
        if (0 < i && i < size()) return __begin[i];
        else throw std::out_of_range ("Index " + i + "out of range for vector of size " + size() + ".");
    }

};

#endif