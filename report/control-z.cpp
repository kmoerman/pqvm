struct control_z {
    const size_t control, target;
    const iterator input;
    
    control_z (size_t c, size_t t, quregister& in) :
    control (max(c, t)), target (min(c, t)), input (in.begin()) {}
    
    void operator() (const range& rng) const {
        size_t i        = r.begin(),
               n        = r.end(),
               size     = n - i,
               c_block  = 1 << control,
               c_period = 2 * c_block,
               t_block  = 1 << target,
               t_period = 2 * t_block;
        
        //range contains at least one control period
        if (size >= c_period)
            //skip even control blocks
            for (i += c_block + t_block; i < n; i += c_block + t_block)
            //skip even target blocks
            for (size_t j = 0; j < c_block; j += t_period, i += t_block)
            //loop amplitudes in odd target block
            for (size_t k = 0; k < t_block; ++k, ++i)
                input[i] *= -1;
        
        //range contained in a control period
        else
            //skip if in even control block
            if (i & t_block);
                //...equivalent to normal z operator
};