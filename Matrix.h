#define MATRIX_H

#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int m;
    int n;
    double** data;
} Matrix;

// 创建一个新的矩阵
extern Matrix createMatrix(int m, int n);

// 释放矩阵内存
extern void freeMatrix(Matrix mat);

// 创建单位矩阵
extern Matrix identityMatrix(int size);

// 创建零矩阵
extern Matrix zeroMatrix(int m, int n);

// 矩阵加法
extern Matrix matrixAdd(Matrix a, Matrix b);

// 矩阵减法
extern Matrix matrixSubtract(Matrix a, Matrix b);

// 矩阵相乘
extern Matrix matrixMultiply(Matrix a, Matrix b);

// 矩阵转置
extern Matrix matrixTranspose(Matrix a);

// 矩阵求逆（适用于所有 n*n 满秩方阵）
extern Matrix matrixInverse(Matrix a);

// 打印矩阵
extern void printMatrix(Matrix mat);
