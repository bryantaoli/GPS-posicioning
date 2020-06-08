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
			C.element[i] = A.element[i] + B.element[i];//这里变量传不传的出去是个问题，指针变量是一定传的出去的，但是C.num这样的变量可以出去么
		}
		return C;
	}
	else
	{
		printf("矩阵维数不一样，请回车键入即退出程序\n");
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
			C.element[i] = A.element[i] - B.element[i];//这里变量传不传的出去是个问题，居然传的出去？？
		}
		return C;
	}
	else
	{
		printf("矩阵维数不一样，请回车键入即退出程序\n");
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
			//int rowinA_1 = int(i/A.col)%A.row;//这里取行(即rowinA)困难,就用rowinA_1// 这里取东西有问题
			//int colinB_1 = int(i/A.row);  // 这里取东西有问题
			double sum = 0;
			for (int j = 0; j <A.col ; j++ )
			{
				//sum = sum + A.element[(rowinA_1)*A.col+j]*B.element[colinB_1+j*B.col];
				sum = sum + 
			}
			C.element[i] = sum;
		}*/
		/*********************************************

		这里开始用双重循环去做很困难，因为知道某个数的坐标去索引它的行数（or列数）很困难，
		所以用三重循环去解决这个问题比较好

		*********************************************/
		for(int i = 0; i < B.col; i++)
		{
			for (int j = 0; j < A.row; j++)
			{
				double sum = 0;
				for (int k = 0; k < A.col; k++)
				{
					sum = sum + A.element[j*A.col+k] * B.element[i+k*B.col];//索引的时候牢记矩阵是0开始的规律，所以这里都不需要-1这样
				}
				C.element[j*C.col+i] = sum;
			}
		}
		return C;
	}
	else 
	{
		printf("前一个矩阵的列数不等于后一个矩阵的行数，请回车键入即退出程序\n");
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
					sum = sum + A->element[j*A->col + k] * B->element[i + k*B->col];//索引的时候牢记矩阵是0开始的规律，所以这里都不需要-1这样
				}
				C->element[j*C->col + i] = sum;
			}
		}
		return true;
	}
	else
	{
		printf("前一个矩阵的列数不等于后一个矩阵的行数，请回车键入即退出程序\n");
		getchar();
		exit(1);
	}
}
double matrixdet(matrix A)
{
	
	/*这里有问题，递归的时候每次进来就把det给变0了*/
	/*其实没有问题，每次进来确实都是重新计算一个矩阵，必须要把这个置零，而且外面的det会被保留*/
     double det = 0.0;
	if (A.col != A.row)
	{
		printf("矩阵的列数不等于矩阵的行数，无法计算行列式，请回车键入即退出程序\n");
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
		for (int i = 0; i < A.col; i++)//i变量是控制列数的变量;
		{
			//这是静态变量在栈上面,所以不需要释放，退出函数他会自动释放;
			matrix M;
			matrix* Mpointer;
			Mpointer = &M;
			M.col = A.col - 1;
			M.row = A.row - 1;
			M.num = M.row * M.col;
			//这是申请的变量，在堆上面，必须手动释放;
			M.element = (double*)malloc(sizeof(double)*M.num);
			int j = 0;
			int k = 0;
			while (j < M.num)//j变量在索引整个矩阵;
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
			//free(Mpointer);   为什么这么写是不对的啊
			//Mpointer = NULL;
		}
	}
	return det;
}

/***************************************

求矩阵的数乘;

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

		求矩阵的转置;

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
	//划去第i行第j列;
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
			while (l < left.num)//l变量在索引整个left矩阵,k变量在索引整个A矩阵
			{
				//既不在i行，也在j列
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
	//划去第i行第j列
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
			while (l < left->num)//l变量在索引整个left矩阵,k变量在索引整个A矩阵
			{
				//既不在i行，也在j列
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

	王甫红老师给的矩阵主对角消元法计算矩阵的逆

	n是矩阵的维数

	a是一个double数据的头地址，所以可以用一个指向double的指针代入，比如A.element

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

	/* 将输入矩阵赋值给输出矩阵b，下面对b矩阵求逆，a矩阵不变 */
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
		for (i = k; i<n; i++)   /* 查找右下角方阵中主元素的位置 */
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

		if (d<DBL_EPSILON)   /* 主元素接近于0，矩阵不可逆 */
		{
			printf("Divided by 0 in MatrixInv!\n");
			return 0;
		}

		if (is[k] != k)  /* 对主元素所在的行与右下角方阵的首行进行调换 */
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

		if (js[k] != k)  /* 对主元素所在的列与右下角方阵的首列进行调换 */
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
		b[l] = 1.0 / b[l];  /* 初等行变换 */
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

	for (k = n - 1; k >= 0; k--)  /* 将上面的行列调换重新恢复 */
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
