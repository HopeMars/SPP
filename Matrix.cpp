#include "Matrix.h"

// 创建一个新的矩阵
Matrix createMatrix(int m, int n) {
    Matrix mat;
    mat.m = m;
    mat.n = n;
    mat.data = (double**)malloc(m * sizeof(double*));
    for (int i = 0; i < m; i++) {
        mat.data[i] = (double*)malloc(n * sizeof(double));
    }
    return mat;
}

// 释放矩阵内存
void freeMatrix(Matrix mat) {
    for (int i = 0; i < mat.m; i++) {
        free(mat.data[i]);
    }
    free(mat.data);
}

// 创建单位矩阵
Matrix identityMatrix(int size) {
    Matrix mat = createMatrix(size, size);
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            mat.data[i][j] = (i == j) ? 1.0 : 0.0;
        }
    }
    return mat;
}

// 创建零矩阵
Matrix zeroMatrix(int m, int n) {
    Matrix mat = createMatrix(m, n);
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            mat.data[i][j] = 0.0;
        }
    }
    return mat;
}

// 矩阵加法
Matrix matrixAdd(Matrix a, Matrix b) {
    if (a.m != b.m || a.n != b.n) {
        printf("Error: Matrices have different dimensions.\n");
        exit(1);
    }
    Matrix result = createMatrix(a.m, a.n);
    for (int i = 0; i < a.m; i++) {
        for (int j = 0; j < a.n; j++) {
            result.data[i][j] = a.data[i][j] + b.data[i][j];
        }
    }
    return result;
}

// 矩阵减法
Matrix matrixSubtract(Matrix a, Matrix b) {
    if (a.m != b.m || a.n != b.n) {
        printf("Error: Matrices have different dimensions.\n");
        exit(1);
    }
    Matrix result = createMatrix(a.m, a.n);
    for (int i = 0; i < a.m; i++) {
        for (int j = 0; j < a.n; j++) {
            result.data[i][j] = a.data[i][j] - b.data[i][j];
        }
    }
    return result;
}

// 矩阵相乘
Matrix matrixMultiply(Matrix a, Matrix b) {
    if (a.n != b.m) {
        printf("Error: Incompatible dimensions for matrix multiplication.\n");
        exit(1);
    }
    Matrix result = createMatrix(a.m, b.n);
    for (int i = 0; i < a.m; i++) {
        for (int j = 0; j < b.n; j++) {
            result.data[i][j] = 0.0;
            for (int k = 0; k < a.n; k++) {
                result.data[i][j] += a.data[i][k] * b.data[k][j];
            }
        }
    }
    return result;
}

// 矩阵转置
Matrix matrixTranspose(Matrix a) {
    Matrix result = createMatrix(a.n, a.m);
    for (int i = 0; i < a.m; i++) {
        for (int j = 0; j < a.n; j++) {
            result.data[j][i] = a.data[i][j];
        }
    }
    return result;
}

// 矩阵求逆（适用于所有 n*n 满秩方阵）
Matrix matrixInverse(Matrix a) {
    if (a.m != a.n) {
        printf("Error: Matrix is not square.\n");
        exit(1);
    }
    int n = a.m;
    Matrix result = createMatrix(n, n);
    Matrix temp = createMatrix(n, n * 2);

    // 初始化增广矩阵
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            temp.data[i][j] = a.data[i][j];
        }
        for (int j = n; j < 2 * n; j++) {
            temp.data[i][j] = (i == j - n) ? 1.0 : 0.0;
        }
    }

    // 高斯-约旦消元法
    for (int i = 0; i < n; i++) {
        if (temp.data[i][i] == 0.0) {
            printf("Error: Matrix is not invertible.\n");
            exit(1);
        }
        for (int j = 0; j < 2 * n; j++) {
            temp.data[i][j] /= temp.data[i][i];
        }
        for (int k = 0; k < n; k++) {
            if (k != i) {
                double factor = temp.data[k][i];
                for (int j = 0; j < 2 * n; j++) {
                    temp.data[k][j] -= factor * temp.data[i][j];
                }
            }
        }
    }

    // 提取逆矩阵
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            result.data[i][j] = temp.data[i][j + n];
        }
    }

    freeMatrix(temp);
    return result;
}

// 打印矩阵
void printMatrix(Matrix mat) {
    for (int i = 0; i < mat.m; i++) {
        for (int j = 0; j < mat.n; j++) {
            printf("%f ", mat.data[i][j]);
        }
        printf("\n");
    }
}
