#ifndef pqvm_foreach_h
#define pqvm_foreach_h

#include <type_traits>

namespace macros {
    
    namespace foreach {
        
        #define foreach(range, iterator) \
            for (auto iterator (macros::foreach::begin(range)); iterator != macros::foreach::end(range); ++iterator)
        
        template <typename T>
        inline typename T::iterator begin (const T& range) {
            return range.begin();
        }
        
        template <typename T>
        inline typename T::iterator begin (const T* range) {
            return range->begin();
        }
        
        template <typename T>
        inline typename std::enable_if<std::is_integral<T>::value, T>::type begin(T range) {
            return 0;
        }
        
        template <typename T>
        inline typename T::iterator end (const T& range) {
            return range.end();
        }
        
        template <typename T>
        inline typename T::iterator end (const T* range) {
            return range->end();
        }
        
        template <typename T>
        inline typename std::enable_if<std::is_integral<T>::value, T>::type end (T range) {
            return range;
        }
    }
    
}

#endif
