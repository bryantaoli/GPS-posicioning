#pragma once
#include<stdio.h>
#include<stdlib.h>
#ifndef _MATRIX_H
#define _MATRIX_H

struct matrix
{
	 int num;
	 int row;//行数;
	 int col;//列数;
	 double *element;
	 matrix()
	 {
		num = 1;
		row = 1;
		col = 1;
		element = NULL;
	 }
};

typedef matrix* matrixpointer;

matrix matrixadd(matrix A, matrix B);
//前面是被减矩阵，后面是矩阵
matrix matrixminus(matrix A, matrix B);
//前一个参数的矩阵的列要等于后一个参数矩阵的行
matrix matrixmultiply(matrix A, matrix B);
double matrixdet(matrix A);
//当要数除的时候就用这个函数去成要数除的除数的倒数
matrix matrixscalarmultiply(matrix A,double ra);
//矩阵的转置矩阵
matrix matrixtranspose(matrix A);
//矩阵的代数余子式矩阵
matrix matrixcof(matrix A);
//用传地址的方式计算矩阵的代数余子式矩阵
bool matrixcofbyaddress(matrix* A,matrix* cof);
//矩阵的伴随矩阵
matrix matrixadj(matrix A);
//矩阵的逆矩阵
matrix matrixinv(matrix A);
matrix matrixmultiply3(matrix A, matrix B, matrix C);
matrix matrixmultiply4(matrix A, matrix B, matrix C,matrix D);
int MatrixInv(int n, double ra[], double rb[]);

bool MatrixTranspose(matrix* A, matrix* Transpose);
bool MatrixMultiply(matrix *A, matrix *B, matrix*C);
#endif // !_MATRIX_H





