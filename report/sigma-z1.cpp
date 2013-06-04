struct z {
    const size_t mask;
    const iterator input, output;
    
    sigma_z (size_t t, quregister& in, quregister& out) :
    mask (1 << t), input (in.begin()), output (out.begin()) {}
    
    void operator() (const range& rng) const {
        for (size_t i = r.begin(), n = r.end(); i < n; ++i)
            output[i] = (i & mask) ? -input[i] : input[i];
    }
};