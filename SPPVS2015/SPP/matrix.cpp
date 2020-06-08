#include"matrix.h"
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<float.h>

matrix matrixadd(matrix A, matrix B)
{
	if (A.col == B.col && A.row == B.row && A.num == B.num)
	{
		matrix C;
		C.num = A.num;
		C.row = A.row;
		C.col = A.col;
		C.element = (double*)malloc(sizeof(double)*C.num);
		for (int i = 0; i < A.num; i++)
		{
			C.element[i] = A.element[i] + B.element[i];//��������������ĳ�ȥ�Ǹ����⣬ָ�������һ�����ĳ�ȥ�ģ�����C.num�����ı������Գ�ȥô
		}
		return C;
	}
	else
	{
		printf("����ά����һ������س����뼴�˳�����\n");
		getchar();
		exit(1);
	}
}

matrix matrixminus(matrix A, matrix B)
{
	if (A.col == B.col && A.row == B.row && A.num == B.num)
	{
		matrix C;
		C.num = A.num;
		C.row = A.row;
		C.col = A.col;
		C.element = (double*)malloc(sizeof(double)*C.num);
		for (int i = 0; i < A.num; i++)
		{
			C.element[i] = A.element[i] - B.element[i];//��������������ĳ�ȥ�Ǹ����⣬��Ȼ���ĳ�ȥ����
		}
		return C;
	}
	else
	{
		printf("����ά����һ������س����뼴�˳�����\n");
		getchar();
		exit(1);
	}
}


matrix matrixmultiply3(matrix A, matrix B, matrix C)
{
	matrix AB = matrixmultiply(A, B);
	return (matrixmultiply(AB, C));
}

matrix matrixmultiply4(matrix A, matrix B, matrix C, matrix D)
{
	matrix ABC = matrixmultiply3(A, B, C);
	return(matrixmultiply(ABC, D));
}

matrix matrixmultiply(matrix A, matrix B)
{
	if (A.col == B.row)
	{
		matrix C;
		C.num = A.row*B.col;
		C.row = A.row;
		C.col = B.col;
		C.element = (double*)malloc(sizeof(double)*C.num);
		/*for (int i = 0; i < C.num; i++)
		{
			//int rowinA_1 = int(i/A.col)%A.row;//����ȡ��(��rowinA)����,����rowinA_1// ����ȡ����������
			//int colinB_1 = int(i/A.row);  // ����ȡ����������
			double sum = 0;
			for (int j = 0; j <A.col ; j++ )
			{
				//sum = sum + A.element[(rowinA_1)*A.col+j]*B.element[colinB_1+j*B.col];
				sum = sum + 
			}
			C.element[i] = sum;
		}*/
		/*********************************************

		���￪ʼ��˫��ѭ��ȥ�������ѣ���Ϊ֪��ĳ����������ȥ��������������or�����������ѣ�
		����������ѭ��ȥ����������ȽϺ�

		*********************************************/
		for(int i = 0; i < B.col; i++)
		{
			for (int j = 0; j < A.row; j++)
			{
				double sum = 0;
				for (int k = 0; k < A.col; k++)
				{
					sum = sum + A.element[j*A.col+k] * B.element[i+k*B.col];//������ʱ���μǾ�����0��ʼ�Ĺ��ɣ��������ﶼ����Ҫ-1����
				}
				C.element[j*C.col+i] = sum;
			}
		}
		return C;
	}
	else 
	{
		printf("ǰһ����������������ں�һ���������������س����뼴�˳�����\n");
		getchar();
		exit(1);
	}
}


bool MatrixMultiply(matrix *A, matrix *B,matrix*C)
{
	if (A->col == B->row)
	{
		for (int i = 0; i < B->col; i++)
		{
			for (int j = 0; j < A->row; j++)
			{
				double sum = 0;
				for (int k = 0; k < A->col; k++)
				{
					sum = sum + A->element[j*A->col + k] * B->element[i + k*B->col];//������ʱ���μǾ�����0��ʼ�Ĺ��ɣ��������ﶼ����Ҫ-1����
				}
				C->element[j*C->col + i] = sum;
			}
		}
		return true;
	}
	else
	{
		printf("ǰһ����������������ں�һ���������������س����뼴�˳�����\n");
		getchar();
		exit(1);
	}
}
double matrixdet(matrix A)
{
	
	/*���������⣬�ݹ��ʱ��ÿ�ν����Ͱ�det����0��*/
	/*��ʵû�����⣬ÿ�ν���ȷʵ�������¼���һ�����󣬱���Ҫ��������㣬���������det�ᱻ����*/
     double det = 0.0;
	if (A.col != A.row)
	{
		printf("��������������ھ�����������޷���������ʽ����س����뼴�˳�����\n");
		getchar();
		exit(1);
	}
	else if (A.num == 1)
	{
		return A.element[0];
	}
	else if(A.num == 4)
	{
		return A.element[0]* A.element[3]- A.element[1]* A.element[2];
	}
	else
	{
		for (int i = 0; i < A.col; i++)//i�����ǿ��������ı���;
		{
			//���Ǿ�̬������ջ����,���Բ���Ҫ�ͷţ��˳����������Զ��ͷ�;
			matrix M;
			matrix* Mpointer;
			Mpointer = &M;
			M.col = A.col - 1;
			M.row = A.row - 1;
			M.num = M.row * M.col;
			//��������ı������ڶ����棬�����ֶ��ͷ�;
			M.element = (double*)malloc(sizeof(double)*M.num);
			int j = 0;
			int k = 0;
			while (j < M.num)//j������������������;
			{
				if ((k>(A.col-1))&&(((k+1)%A.col)!=((i+1)%A.col)))
				{
					if(j<M.num)
					M.element[j] = A.element[k];
					j++;
				}
				k++;
			}
			det = det + A.element[i]*pow(-1,i)*matrixdet(M);
			free(M.element);
			//free(Mpointer);   Ϊʲô��ôд�ǲ��Եİ�
			//Mpointer = NULL;
		}
	}
	return det;
}

/***************************************

����������;

***************************************/
matrix matrixscalarmultiply(matrix A, double ra)
{
	matrix scalarmultiply;
	scalarmultiply.col = A.col;
	scalarmultiply.row = A.row;
	scalarmultiply.num = A.num;
	scalarmultiply.element = (double*)malloc(sizeof(double)*scalarmultiply.num);
	for (int i = 0; i < A.num; i++)
	{
		scalarmultiply.element[i] = A.element[i] * ra;
	}
	return scalarmultiply;
}


/***************************************

		������ת��;

***************************************/
matrix matrixtranspose(matrix A)
{
	matrix Transpose;
	Transpose.col = A.row;
	Transpose.row = A.col;
	Transpose.num = A.num;
	Transpose.element = (double*)malloc(sizeof(double)*Transpose.num);
	for (int i = 0; i < A.row; i++)
	{
		for (int j = 0; j < A.col; j++)
		{
			Transpose.element[j*Transpose.col+i] = A.element[i*A.col+j];
		}
	}
	return Transpose;
}

bool MatrixTranspose(matrix* A, matrix* BT)
{
	
	for (int i = 0; i < A->row; i++)
	{
		for (int j = 0; j < A->col; j++)
		{
			BT->element[j*BT->col + i] = A->element[i*A->col + j];
		}
	}
	return true;
}


matrix matrixcof(matrix A)
{
	matrix cof;
	cof.col = A.col;
	cof.row = A.row;
	cof.num = A.num;
	cof.element = (double*)malloc(sizeof(double)*cof.num);
	//��ȥ��i�е�j��;
	for (int i = 0; i < cof.row; i++)
	{
		for (int j = 0; j < cof.col;j++)
		{
			matrix left;
			left.col = A.col - 1;
			left.row = A.row - 1;
			left.num = left.col*left.row;
			left.element = (double*)malloc(sizeof(double)*left.num);
			int l = 0;
			int k = 0;
			while (l < left.num)//l��������������left����,k��������������A����
			{
				//�Ȳ���i�У�Ҳ��j��
				if ((int(k / A.col )!=i )&& ((k % A.row) != j))
				{
					left.element[l] = A.element[k];
					l++;
				}
				k++;
			}
			cof.element[i*cof.col + j] = pow(-1, i + j)* matrixdet(left);
			free(left.element);
		}
	}
	return cof;
}

bool matrixcofbyaddress(matrix * A, matrix * cof)
{
	cof->col = A->col;
	cof->row = A->row;
	cof->num = A->num;
	cof->element = (double*)malloc(sizeof(double)*cof->num);
	//��ȥ��i�е�j��
	for (int i = 0; i < cof->row; i++)
	{
		for (int j = 0; j < cof->col; j++)
		{
			matrix left1;
			matrix *left = &left1;
			left->col = A->col - 1;
			left->row = A->row - 1;
			left->num = left->col*left->row;
			left->element = (double*)malloc(sizeof(double)*left->num);
			int l = 0;
			int k = 0;
			while (l < left->num)//l��������������left����,k��������������A����
			{
				//�Ȳ���i�У�Ҳ��j��
				if ((int(k / A->col) != i) && ((k % A->row) != j))
				{
					left->element[l] = A->element[k];
					l++;
				}
				k++;
			}
			cof->element[i*cof->col + j] = pow(-1, i + j)* matrixdet(*left);
			free(left->element);
		}
	}
	return false;
}

matrix matrixadj(matrix A)
{
	matrix adj;
	/*adj.col = A.col;
	adj.num = A.num;
	adj.row = A.num;
	adj.element = (double*)malloc(sizeof(double)*adj.num);*/
	adj = matrixtranspose(matrixcof(A));
	return adj;
}

matrix matrixinv(matrix A)
{
	matrix inv;
	inv = matrixscalarmultiply(matrixadj(A), 1 / matrixdet(A));
	return inv;
}
/**********************************************************************

	��������ʦ���ľ������Խ���Ԫ������������

	n�Ǿ����ά��

	a��һ��double���ݵ�ͷ��ַ�����Կ�����һ��ָ��double��ָ����룬����A.element

**********************************************************************/
int MatrixInv(int n, double ra[], double b[])
{
	//int i, j, k, l, u, v, is[10], js[10];   /* matrix dimension <= 10 */
	int i, j, k, l, u, v;
	int *is = (int*)malloc(sizeof(int)*n);
	int *js = (int*)malloc(sizeof(int)*n);
	double d, p;

	if (n <= 0)
	{
		printf("Error dimension in MatrixInv!\n");
		exit(EXIT_FAILURE);
	}

	/* ���������ֵ���������b�������b�������棬a���󲻱� */
	for (i = 0; i<n; i++)
	{
		for (j = 0; j<n; j++)
		{
			b[i*n + j] = ra[i*n + j];
		}
	}

	for (k = 0; k<n; k++)
	{
		d = 0.0;
		for (i = k; i<n; i++)   /* �������½Ƿ�������Ԫ�ص�λ�� */
		{
			for (j = k; j<n; j++)
			{
				l = n*i + j;
				p = fabs(b[l]);
				if (p>d)
				{
					d = p;
					is[k] = i;
					js[k] = j;
				}
			}
		}

		if (d<DBL_EPSILON)   /* ��Ԫ�ؽӽ���0�����󲻿��� */
		{
			printf("Divided by 0 in MatrixInv!\n");
			return 0;
		}

		if (is[k] != k)  /* ����Ԫ�����ڵ��������½Ƿ�������н��е��� */
		{
			for (j = 0; j<n; j++)
			{
				u = k*n + j;
				v = is[k] * n + j;
				p = b[u];
				b[u] = b[v];
				b[v] = p;
			}
		}

		if (js[k] != k)  /* ����Ԫ�����ڵ��������½Ƿ�������н��е��� */
		{
			for (i = 0; i<n; i++)
			{
				u = i*n + k;
				v = i*n + js[k];
				p = b[u];
				b[u] = b[v];
				b[v] = p;
			}
		}

		l = k*n + k;
		b[l] = 1.0 / b[l];  /* �����б任 */
		for (j = 0; j<n; j++)
		{
			if (j != k)
			{
				u = k*n + j;
				b[u] = b[u] * b[l];
			}
		}
		for (i = 0; i<n; i++)
		{
			if (i != k)
			{
				for (j = 0; j<n; j++)
				{
					if (j != k)
					{
						u = i*n + j;
						b[u] = b[u] - b[i*n + k] * b[k*n + j];
					}
				}
			}
		}
		for (i = 0; i<n; i++)
		{
			if (i != k)
			{
				u = i*n + k;
				b[u] = -b[u] * b[l];
			}
		}
	}

	for (k = n - 1; k >= 0; k--)  /* ����������е������»ָ� */
	{
		if (js[k] != k)
		{
			for (j = 0; j<n; j++)
			{
				u = k*n + j;
				v = js[k] * n + j;
				p = b[u];
				b[u] = b[v];
				b[v] = p;
			}
		}
		if (is[k] != k)
		{
			for (i = 0; i<n; i++)
			{
				u = i*n + k;
				v = is[k] + i*n;
				p = b[u];
				b[u] = b[v];
				b[v] = p;
			}
		}
	}
	free(is);
	free(js);
	return (1);
}
