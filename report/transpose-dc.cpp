inline void deinterleave (uint64_t z, uint64_t* row, uint64_t* col) {
    
    z = (z & 0x9999999999999999) | ((z >>  1) & 0x2222222222222222)
                                 | ((z <<  1) & 0x4444444444444444);
    z = (z & 0xC3C3C3C3C3C3C3C3) | ((z >>  2) & 0x0C0C0C0C0C0C0C0C)
                                 | ((z <<  2) & 0x3030303030303030);
    z = (z & 0xF00FF00FF00FF00F) | ((z >>  4) & 0x00F000F000F000F0)
                                 | ((z <<  4) & 0x0F000F000F000F00);
    z = (z & 0xFF0000FFFF0000FF) | ((z >>  8) & 0x0000FF000000FF00)
                                 | ((z <<  8) & 0x00FF000000FF0000);
    z = (z & 0xFFFF00000000FFFF) | ((z >> 16) & 0x00000000FFFF0000)
                                 | ((z << 16) & 0x0000FFFF00000000);
    
    *col = z & 0x00000000FFFFFFFF;
    *row = z >> 32;
}

void transpose_dc (double** A, double** B, size_t n) {
    size_t i, j;
    for (size_t z = 0, z_max = n * n; z < z_max; ++z) {
        deinterleave(z, &i, &j);
        B[j][i] = A[i][j];
    }
}