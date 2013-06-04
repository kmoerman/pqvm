struct x_even {
    const size_t target;
    const iterator input, output;
    
    x_even (size_t t_, quregister& i_, quregister& o_) :
    target (t_), input (i_.begin()), output (o_.begin()) {}
    
    void operator () (range& rng) const {
        size_t stride = 1 << target,
               period = 2 * stride,
               i      = rng.begin(),
               block  = stride * sizeof(complex),
               blocks = rng.size() / period;
        
        iterator  ipt = input  + i,
                  opt = output + i + stride;
        
        while (blocks > 0) {
            memcpy(opt, ipt, block);
            opt += period;
            ipt += period;
            --blocks;
        }
    }
};