typedef tbb::blocked_range<size_t> range;
typedef double real;
typedef std::complex<double> complex;

inline real inner_product_term (const real a, const real b) {
    return a * b;
}

inline complex inner_product_term(const complex a, const complex b) {
    return a * std::conj(b);
}

template <typename number>
class inner_product {
    vector<number>& vec;
    number part;
public:
    inner_product (std::vector<number>& v): vec(v), part(0) {}
    inner_product (inner_product& ip, split): vec(ip.vec), part(0) {}
    
    void operator() (const range& rng) {
        for (size_t i = rng.begin(), n = rng.end(); i != n; ++i)
            part += inner_product_term(vec[i], vec[i]);
    }
    
    void join (const inner_product& ipr) {
        part += ipr.part;
    }
};

template <typename number>
normalize_parallel (std::vector<number>& vec) {
    tbb::parallel_reduce(range (0, vec.size()), inner_product (vec));
    number norm = std::sqrt(ipr.part);
    // ... divide vec by norm
}

template <typename number>
normalize_sequential (std::vector<number>& vec) {
    number norm;
    for (size_t i = vec.begin(), n = vec.end(); i != n; ++i)
        norm += inner_product_term(vec[i], vec[i]);
    norm = std::sqrt(norm);
    // ... divide vec by norm
}