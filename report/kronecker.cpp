void kronecker (quregister& left, quregister& right, quregister& result) {
    size_t n = left.size(),
           m = right.size();
    
    result.reserve(n * m);
    
    parallel for (size_t i = 0, k = 0; i != n; ++i)
        for (size_t j = 0; j != m; ++j; ++k)
            result[k] = left[i] * right[j];
}