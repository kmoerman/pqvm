struct z {
    const size_t target;
    const iterator input;
    
    z (size_t t, quregister& in) :
    target (t), input (in.begin()) {}
    
    void operator() (const range& rng) const {
        size_t i      = r.begin(),
               n      = r.end(),
               size   = n - i,
               block  = 1 << target,
               period = 2 * block;
        
        //range contains at least one period
        if (size >= period)
            //skip even target blocks
            for (i += block; i < n; i += block)
            //loop amplitudes in odd target block
            for (size_t j = 0; j < block; ++j, ++i)
                input[i] *= -1;
        
        //range contained in a period
        else
            //skip if in even target block
            if (i & block)
            //loop ampliutdes in add target block
            for (; i < n; ++i)
                input[i] *= -1;
    }
};