void transpose_naive (double** A, double** B, size_t m, size_t n) {
    for (size_t i = 0; i < m; ++i)
        for (size_t j = 0; j < n; ++j)
            B[j][i] = A[i][j];
}