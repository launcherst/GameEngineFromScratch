#pragma once
#include "geommath.hpp"

namespace My {
    // infact our matrix is column-major, so it is RQ decompose ...
    template <typename T, int ROWS, int COLS>
    inline void MatrixQRDecompose(const Matrix<T, ROWS, COLS>& in_matrix,
        Matrix<T, COLS, COLS>& Q,
        Matrix<T, ROWS, COLS>& R)
    {
        static_assert(ROWS >= COLS, "We can only QR Decompose a Matrix which ROWS >= COLS. (note our matrix is column major)");
        Matrix<T, ROWS, COLS> U;
        Vector<T, COLS>* pUi;
        std::memset(U, 0x00, sizeof(U));
        std::memset(R, 0x00, sizeof(R));
        for (int i = 0; i < ROWS; i++)
        {
            std::memcpy(U[i], in_matrix[i], sizeof(T) * COLS);
            pUi = reinterpret_cast<Vector<T, COLS>*>(U[i]);
            for (int j = 0; j < i; j++)
            {
                Vector<T, COLS>* pUj;
                pUj = reinterpret_cast<Vector<T, COLS>*>(U[j]);
                T numerator, denominator;
                DotProduct(numerator, *pUi, *pUj);
                denominator = Length(*pUj);
                auto coefficient = numerator / denominator;
                (*pUi) = (*pUi) - coefficient * (*pUj);
                R[i][j] = coefficient;
            }

            R[i][i] = Length(*pUi);
            *pUi = *pUi / R[i][i];

            if (i < COLS)
            {
                std::memcpy(Q[i], pUi, sizeof(T) * COLS);
            }
        }
    }

    inline void Matrix4X4fCompose(Matrix4X4f& matrix, const Vector3f& rotation, const Vector3f& scalar, const Vector3f& translation)
    {
        Matrix4X4f matrix_rotate;
        MatrixRotationYawPitchRoll(matrix_rotate, rotation[0], rotation[1], rotation[2]);
        Matrix4X4f matrix_scale;
        MatrixScale(matrix_scale, scalar);
        Matrix4X4f matrix_translation;
        MatrixTranslation(matrix_translation, translation);
        matrix = matrix_rotate * matrix_scale * matrix_translation;
    }

    inline void Matrix4X4fDecompose(const Matrix4X4f& matrix, Vector3f& rotation, Vector3f& scalar, Vector3f& translation)
    {
        translation.Set({matrix[3][0], matrix[3][1], matrix[3][2]});

        // QR decompose the top-left 3x3 matrix
        Matrix3X3f bases = {{{
            {matrix[0][0], matrix[0][1], matrix[0][2]},
            {matrix[1][0], matrix[1][1], matrix[1][2]},
            {matrix[2][0], matrix[2][1], matrix[2][2]}
        }}};
        Matrix3X3f Q, R;
        MatrixQRDecompose(bases, Q, R);

        // decompose the scale
        scalar.Set({R[0][0], R[1][1], R[2][2]});

        // decompose the rotation matrix
        float theta_x = atan2f(Q[0][2], Q[1][2]);
        float theta_y = acosf(Q[2][2]);
        float theta_z = -atan2f(Q[2][0], Q[2][1]);
        
        rotation.Set({theta_x, theta_y, theta_z});
    }
}