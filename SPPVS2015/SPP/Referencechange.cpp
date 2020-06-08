#include "Referencechange.h"
#include "matrix.h"
#include<math.h>


bool BLHToXYZ(BLH * A, XYZ * B)
{
	if (A != NULL&& B!= NULL)
	{
		double N = a / (sqrt(1 - e*e*sin(A->B)*sin(A->B)));
		B->X = (N + A->H)*cos(A->B)*cos(A->L);
		B->Y = (N + A->H)*cos(A->B)*sin(A->L);
		B->Z = (N*(1 - e*e) + A->H)*sin(A->B);
		return true;
	}
	else
	{
		return false;
	}
}

bool XYZToENU(XYZ * A1, XYZ * A2, ENU * E)
{
	XYZ Geocentric;
	if (fabs(Distance(A2, &Geocentric)==0)) return false;
	else
	{
		BLH A2_blh;
		XYZToBLH(A2, &A2_blh);
		XYZ AA;
		BLHToXYZ(&A2_blh, &AA);
		matrix M;
		M.col = 3;
		M.row = 3;
		M.num = 9;
		M.element = (double*)malloc(sizeof(double)*M.num);
		M.element[0] = -sin(A2_blh.L);
		M.element[1] = cos(A2_blh.L);
		M.element[2] = 0;
		M.element[3] = -sin(A2_blh.B)*cos(A2_blh.L);
		//M.element[4] = -sin(A2_blh.B)*cos(A2_blh.L);
		M.element[4] = -sin(A2_blh.B)*sin(A2_blh.L);
		M.element[5] = cos(A2_blh.B);
		M.element[6] = cos(A2_blh.B)*cos(A2_blh.L);
		M.element[7] = cos(A2_blh.B)*sin(A2_blh.L);
		M.element[8] = sin(A2_blh.B);
		matrix delta_A1_A2;
		delta_A1_A2.row = 3;
		delta_A1_A2.col = 1;
		delta_A1_A2.num = 3;
		delta_A1_A2.element = (double*)malloc(sizeof(double)*delta_A1_A2.num);
		delta_A1_A2.element[0] = A1->X - A2->X;
		delta_A1_A2.element[1] = A1->Y - A2->Y;
		delta_A1_A2.element[2] = A1->Z - A2->Z;
		//matrix T = matrixmultiply(M, delta_A1_A2);
		matrix T;
		T.row = M.row; T.col = delta_A1_A2.col; T.num = T.col*T.row;
		T.element = (double*)malloc(sizeof(double)*T.num);
		MatrixMultiply(&M, &delta_A1_A2, &T);
		free(delta_A1_A2.element);
		free(M.element);
		E->E = T.element[0];
		E->N = T.element[1];
		E->U = T.element[2];
		free(T.element);
		return true;
	}
}

bool XYZToNEU(XYZ * A1, XYZ * A2, NEU * N)
{
	XYZ Geocentric;
	if (fabs(Distance(A2, &Geocentric)-a) > 10000) return false;
	else
	{
		BLH A2_blh;
		XYZToBLH(A2, &A2_blh);
		matrix M;
		M.col = 3;
		M.row = 3;
		M.num = 9;
		M.element = (double*)malloc(sizeof(double)*M.num);
		M.element[0] = -sin(A2_blh.B)*cos(A2_blh.L);
		M.element[1] = -sin(A2_blh.B)*sin(A2_blh.L);
		M.element[2] = cos(A2_blh.B);
		M.element[3] = -sin(A2_blh.L);
		M.element[4] = cos(A2_blh.L);
		M.element[5] = 0.0;
		M.element[6] = cos(A2_blh.B)*cos(A2_blh.L);
		M.element[7] = cos(A2_blh.B)*sin(A2_blh.L);
		M.element[8] = sin(A2_blh.B);
		matrix delta_A1_A2;
		delta_A1_A2.row = 3;
		delta_A1_A2.col = 1;
		delta_A1_A2.num = 3;
		delta_A1_A2.element = (double*)malloc(sizeof(double)*delta_A1_A2.num);
		delta_A1_A2.element[0] = A1->X - A2->X;
		delta_A1_A2.element[1] = A1->Y - A2->Y;
		delta_A1_A2.element[2] = A1->Z - A2->Z;
		//matrix T = matrixmultiply(M, delta_A1_A2);

		matrix T;
		T.row = M.row; T.col = delta_A1_A2.col; T.num = T.col*T.row;
		T.element= (double*)malloc(sizeof(double)*T.num);
		MatrixMultiply(&M, &delta_A1_A2, &T);
		
		N->N = T.element[0];
		N->E = T.element[1];
		N->U = T.element[2];
		free(delta_A1_A2.element);
		free(M.element);
		free(T.element);
		return true;
	}
}

double Distance(XYZ * A1, XYZ * A2)
{
	return(sqrt((A1->X - A2->X)*(A1->X - A2->X) + (A1->Y - A2->Y)*(A1->Y - A2->Y) + (A1->Z - A2->Z)*(A1->Z - A2->Z)));
}



bool XYZToBLH(XYZ * xyz, BLH * blh)
{
	/*if (blh != NULL&& xyz != NULL)
	{
		blh->L = atan2(xyz->Y, xyz->X);
		
		double W = sqrt(1 - e*e*sin(blh->B)*sin(blh->B));
		double N = a / W;
		double R = sqrt(xyz->X*xyz->X + xyz->Y*xyz->Y + xyz->Z*xyz->Z);
		double phi = atan2(xyz->Z, sqrt(xyz->X*xyz->X + xyz->Y*xyz->Y));
		
		double d0,d1;
		d0 = xyz->Z / sqrt(xyz->X*xyz->X + xyz->Y*xyz->Y);
		for (int i = 0; i <= 100; i++)
		{
			W = sqrt(1 - e*e*sin(blh->B)*sin(blh->B));
			N = a / W;
			//d1 = atan2((xyz->Z/sqrt(xyz->X*xyz->X + xyz->Y*xyz->Y) + a*e*e*tan(phi) / sqrt(1 + (1 - e*e)*tan(d0)*tan(d0))) / sqrt(xyz->X*xyz->X + xyz->Y*xyz->Y),1);
			d1 = atan2(xyz->Z / sqrt(xyz->X*xyz->X + xyz->Y*xyz->Y) + tan(phi)*a*e*e*sin(blh->B) / (xyz->Z*W), 1);
			blh->B = d1;
			if (fabs(d1 - d0 )< 0.00000000001)
			{
				break;
			}
			d0 = d1;
		}
		blh->H = (R*cos(phi) / cos(blh->B)) - N;
		return true;
	}
	else
	{
		return false;
	}*/
	double DeltaZ, N;
	DeltaZ = e*e*xyz->Z;
	double d0, d1;
	d0 = 1.0;
	d1 = 0.0;
	for (int i = 0; i <= 200; i++)
	{
		if (fabs(d1 - d0) < 0.000000000001)
		{
			break;
		}
		else
		{
			d1 = d0;
			d0 = asin((xyz->Z + DeltaZ) / sqrt(pow(xyz->X, 2) + pow(xyz->Y, 2) + pow(xyz->Z + DeltaZ, 2)));
			blh->B = d0;
			N = a / sqrt(1 - e*e*sin(blh->B)*sin(blh->B));
			DeltaZ = N * sin(blh->B) * e*e;
		}
	}
	blh->B = atan2((xyz->Z + DeltaZ), sqrt(pow(xyz->X, 2) + pow(xyz->Y, 2)));
	blh->L = atan2(xyz->Y, xyz->X);
	blh->H = sqrt(xyz->X * xyz->X + xyz->Y * xyz->Y + (xyz->Z + DeltaZ)*(xyz->Z + DeltaZ)) - N;
	return true;
}

