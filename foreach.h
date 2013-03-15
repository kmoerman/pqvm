#ifndef pqvm_foreach_h
#define pqvm_foreach_h

namespace macros {
    
    namespace foreach {
        
        #define foreach(range, iterator) \
        for (auto iterator (macros::foreach::begin(range)); iterator != macros::foreach::end(range); ++i)
        
        template <typename T>
        inline typename T::iterator begin (const T& range) {
            return range.begin();
        }
        
        template <typename T>
        inline typename T::iterator begin (const T* range) {
            return range->begin();
        }
    
        inline char begin (const char range) {
            return 0;
        }
    
        inline short begin (const short range) {
            return 0;
        }
    
        inline int begin (const int range) {
            return 0;
        }
    
        inline long begin (const long range) {
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
        
        inline char end (const char range) {
            return range;
        }
        
        inline short end (const short range) {
            return range;
        }
        
        inline int end (const int range) {
            return range;
        }
        
        inline long end (const long range) {
            return range;
        }

    }
    
}

#endif
