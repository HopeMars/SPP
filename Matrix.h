#define MATRIX_H

#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int m;
    int n;
    double** data;
} Matrix;

// ����һ���µľ���
extern Matrix createMatrix(int m, int n);

// �ͷž����ڴ�
extern void freeMatrix(Matrix mat);

// ������λ����
extern Matrix identityMatrix(int size);

// ���������
extern Matrix zeroMatrix(int m, int n);

// ����ӷ�
extern Matrix matrixAdd(Matrix a, Matrix b);

// �������
extern Matrix matrixSubtract(Matrix a, Matrix b);

// �������
extern Matrix matrixMultiply(Matrix a, Matrix b);

// ����ת��
extern Matrix matrixTranspose(Matrix a);

// �������棨���������� n*n ���ȷ���
extern Matrix matrixInverse(Matrix a);

// ��ӡ����
extern void printMatrix(Matrix mat);
