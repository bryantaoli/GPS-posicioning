#pragma once
#include<stdio.h>
#include<stdlib.h>
#ifndef _MATRIX_H
#define _MATRIX_H

struct matrix
{
	 int num;
	 int row;//����;
	 int col;//����;
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
//ǰ���Ǳ������󣬺����Ǿ���
matrix matrixminus(matrix A, matrix B);
//ǰһ�������ľ������Ҫ���ں�һ�������������
matrix matrixmultiply(matrix A, matrix B);
double matrixdet(matrix A);
//��Ҫ������ʱ������������ȥ��Ҫ�����ĳ����ĵ���
matrix matrixscalarmultiply(matrix A,double ra);
//�����ת�þ���
matrix matrixtranspose(matrix A);
//����Ĵ�������ʽ����
matrix matrixcof(matrix A);
//�ô���ַ�ķ�ʽ�������Ĵ�������ʽ����
bool matrixcofbyaddress(matrix* A,matrix* cof);
//����İ������
matrix matrixadj(matrix A);
//����������
matrix matrixinv(matrix A);
matrix matrixmultiply3(matrix A, matrix B, matrix C);
matrix matrixmultiply4(matrix A, matrix B, matrix C,matrix D);
int MatrixInv(int n, double ra[], double rb[]);

bool MatrixTranspose(matrix* A, matrix* Transpose);
bool MatrixMultiply(matrix *A, matrix *B, matrix*C);
#endif // !_MATRIX_H





