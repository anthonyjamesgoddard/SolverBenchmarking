#pragma once
#include "core/matrix.hpp"
#include "core/vector.hpp"

#include <memory>

namespace solvant {

template <typename T, size_t N>
matrix<T, N, N> identity() {
    matrix<T, N, N> I;
    for (int i = 0; i < N; ++i) {
        I[i][i] = 1.0;
    }
    return I;
}

template <typename T, std::size_t N>
void ge(const solvant::vector<T, N>& rhs, const solvant::matrix<T, N, N>& m,
        solvant::vector<T, N>& x) {
    auto U = m;
    auto L = identity<T, N>();
    auto P = identity<T, N>();

    for (int k = 0; k < N - 1; k++) {
        // select i to max u{ik}
        int i = 0;
        T max = 0;
        for (int l = 0; l < N; l++) {
            if (U(l, k) > max) {
                i = l;
                max = U(l, k);
            }
        }
        U.interchange_rows(i, k);
        L.interchange_rows(i, k);
        P.interchange_rows(i, k);
        for (int j = k + 1; j < N; j++) {
            L(j, k) = U(j, k) / U(k, k);
            for (int s = k; s < N; s++) {
                U(j, s) = U(j, s) - L(j, k) * U(k, s);
            }
        }
    }

    solvant::print(U);
    solvant::print(L);
    solvant::print(P);
    auto bb = P * rhs;
    vector<T, N> y;

    // Call forward substitution (returns vector) to solve "Ly=bb"
    forward_substitute(bb, L, y);
    // Call backward substitution Ux = y
    backward_substitute(y, U, x);
}

template <typename T, std::size_t N>
void forward_substitute(const vector<T, N>& bb, const matrix<T, N, N>& L,
                        vector<T, N>& y) {
    y[0] = bb[0] / L(0, 0);
    for (int j = 1; j < N; j++) {
        double SUM = 0;
        for (int k = 0; k < j; k++) {
            SUM += y[k] * L(j, k);
        }
        y[j] = (bb[j] - SUM) / L(j, j);
    }
}

template <typename T, std::size_t N>
void backward_substitute(const vector<T, N>& y, const matrix<T, N, N>& U,
                         vector<T, N>& x) {
    x[N - 1] = y[N - 1] / U(N - 1, N - 1);
    for (int j = N - 1; j > -1; j--) {
        double SUM = 0;
        for (int k = j + 1; k < N; k++) {
            SUM += x[k] * U(j, k);
        }
        x[j] = (y[j] - SUM) / U(j, j);
    }
}

}  // namespace solvant
