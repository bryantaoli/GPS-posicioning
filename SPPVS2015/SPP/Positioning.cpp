#include"Positioning.h"
#include"Datadecoding.h"
#include "matrix.h"
#include <math.h>
#pragma warning(disable:4996)
#define minE15 0.2617993877991494
#define minE10 0.1745329251994330
#define minE5 0.0872664625997164788
#define lambda1 0.1902936728
//#define lambda1 0.19020
//#define lambda1 0.19

bool PositioningAndSpeeingFirst(RawData*rawdata,int GPSEPHEMSVCanBeUsednum,XYZ* xyz,XYZdot* xyzdot,double* Dop,double* sigma0)
{ 
	double d_ion;
	double d_trop;
	matrix x;
	matrix xpie;
	XYZt xyzt0;
	XYZ xyz0;
	XYZ GeoCenter;
	bool whileflag = true;
	do
	{
		GPSTIME rectime;
		rectime = rawdata->Obs->TIME;
		SVPositinAndSpeed SVXYZandXYZpie;

		double delta_tsv_L1, delta_tsv_L1pie;
		//matrix v;
		matrix B, w,P;
		
		//v是改正数，w才是残差阵;
		//v.col = 1;
		//v.num = IsGPSEPHEMSVnum;
		//v.row = IsGPSEPHEMSVnum;
		//v.element = (double*)malloc(sizeof(double)*v.num);
		B.col = 4;
		B.row = GPSEPHEMSVCanBeUsednum;
		B.num = 4 * GPSEPHEMSVCanBeUsednum;
		B.element = (double*)malloc(sizeof(double)*B.num);
		w.col = 1;
		w.num = GPSEPHEMSVCanBeUsednum;
		w.row = GPSEPHEMSVCanBeUsednum;
		w.element = (double*)malloc(sizeof(double)*w.num);

		P.col = GPSEPHEMSVCanBeUsednum;
		P.row = GPSEPHEMSVCanBeUsednum;
		P.num = GPSEPHEMSVCanBeUsednum * GPSEPHEMSVCanBeUsednum;
		P.element = (double*)malloc(sizeof(double)*P.num);

		for (int i = 0; i < P.num; i++)
		{
			P.element[i] = 0.0;
		}
		for (int i = 0; i < P.col; i++)
		{
			P.element[i+i*P.col] = 1;
		}
		
		int j = 0;
		for (int i = 0; i < rawdata->Obsnum; i++)
		{
			CalculateSVPositionOfTransimit(rawdata, rawdata->GPSEPH[rawdata->Obs[i].PRN - 1].PRN, i,&xyzt0, &SVXYZandXYZpie, &delta_tsv_L1,&delta_tsv_L1pie);
			//i是卫星观测值的索引号;
			//这里其实该去判断星历在不在两个小时内，不在不能同，但是这里剔除的话整个观测数会改变...;//解决方法，用一些类似链表的数据结构，或者之前不判断，现在来总的判断一下;
			//目前先不考虑这种情况，直接都拿来算;
			//if (IsGPSEPHEM(rawdata->GPSEPH[rawdata->Obs[i].PRN - 1].PRN))
			if(GPSEPHEMCanBeUsed(rawdata, rawdata->Obs[i].PRN, i))
			{
				//这里每次都要算卫星位置，出来结果还是一样的，浪费了计算时间;
				XYZ SVXYZ;
				//这里开始一直用.SVPositin.X;相当于没改地球自转改正！;
				//SVXYZ.X = SVXYZandXYZpie.SVPositin.X;
				SVXYZ.X = SVXYZandXYZpie.SVPositon_after_wecorret.X;
				//SVXYZ.Y = SVXYZandXYZpie.SVPositin.Y;
				SVXYZ.Y = SVXYZandXYZpie.SVPositon_after_wecorret.Y;
				SVXYZ.Z = SVXYZandXYZpie.SVPositin.Z;
				//printf("%lf  %lf  %lf  \n", SVXYZ.X , SVXYZ.Y, SVXYZ.Z);
				//钟差为什么不收敛，因为w.element[j] =  rawdata->Obs[i].psr - Distance(&xyz0, &SVXYZ)- xyzt0.t这一项开始忘记减钟差了！;
				//高程为什么很大，因为w.element[j] =  rawdata->Obs[i].psr - Distance(&xyz0, &SVXYZ)- xyzt0.t++c*delta_tsv_L1这一项开始忘记加卫星钟差了！;
				//刚开始想着后面来改卫星钟差和接收机钟差，这两项不能后面来改，必须马上改！;
				double d = Distance(&GeoCenter, &xyz0);
				if ((Distance(&GeoCenter, &xyz0)>a - 50000)&& (Distance(&GeoCenter, &xyz0)<a + 50000))
				{
					BLH blh;
					XYZToBLH(&xyz0, &blh);

					double E = calculateE(&SVXYZ, &xyz0);
					double A = calculateA(&SVXYZ, &xyz0);
					
					d_ion = Klobutchar(rectime, blh, E, A, rawdata->ION);
					//double d_ion = 0.0;
					d_trop = Hopfield(blh.H, E*180/PI);
					//printf("%lf\n", d_trop);
					//地球自转改正要先改?;
					//地球自转改正,电离层对流层都放在收敛以后，也就是接收机坐标比较靠谱的时候再改;      错误的想法
					//printf("%lf  %lf  %lf  %lf\n", E * 180 / PI, A * 180 / PI,d_ion,d_trop);
					double aljkdsalskjdla= Distance(&xyz0, &SVXYZ);
					w.element[j] = rawdata->Obs[i].psr - Distance(&xyz0, &SVXYZ) - xyzt0.t + c*delta_tsv_L1 - d_ion - d_trop;
					B.element[4 * j] = (xyz0.X - SVXYZ.X) / Distance(&xyz0, &SVXYZ);
					B.element[4 * j + 1] = (xyz0.Y - SVXYZ.Y) / Distance(&xyz0, &SVXYZ);
					B.element[4 * j + 2] = (xyz0.Z - SVXYZ.Z) / Distance(&xyz0, &SVXYZ);
					B.element[4 * j + 3] = 1.0;


					if (E < 0)
					{
						P.element[(i + 1)*(i + 1) - 1] = 0.0;
					}
					j++;
				}
				else 
				{
					w.element[j] = rawdata->Obs[i].psr - Distance(&xyz0, &SVXYZ) - xyzt0.t + c*delta_tsv_L1;
					B.element[4 * j] = (xyz0.X - SVXYZ.X) / Distance(&xyz0, &SVXYZ);
					B.element[4 * j + 1] = (xyz0.Y - SVXYZ.Y) / Distance(&xyz0, &SVXYZ);
					B.element[4 * j + 2] = (xyz0.Z - SVXYZ.Z) / Distance(&xyz0, &SVXYZ);
					B.element[4 * j + 3] = 1.0;
					j++;
				}
			}
		}
		/***矩阵算结果部分;*/
		matrix BT;
		BT.col = B.row;BT.row = B.col;BT.num = B.num;
		BT.element = (double*)malloc(sizeof(double)*BT.num);
		MatrixTranspose(&B, &BT);

		matrix Mul1;
		Mul1.row = BT.row;Mul1.col = P.col;  Mul1.num = Mul1.col*Mul1.row;
		Mul1.element = (double*)malloc(sizeof(double)*Mul1.num);
		MatrixMultiply(&BT, &P,&Mul1);
		matrix Mul2;
	    Mul2.row = Mul1.row; Mul2.col = B.col; Mul2.num = Mul2.col*Mul2.row;
		Mul2.element = (double*)malloc(sizeof(double)*Mul2.num);
		MatrixMultiply(&Mul1, &B,&Mul2);
		
		matrix y;
		y.col = Mul2.col; y.row = Mul2.col; y.num = Mul2.col*Mul2.col;
		y.element = (double*)malloc(sizeof(double)*y.num);

		MatrixInv(B.col, Mul2.element, y.element);
		if (MatrixInv(B.col, Mul2.element, y.element) == 0)
		{
			free(y.element);
			free(Mul1.element);
			free(Mul2.element);
			free(P.element);
			free(BT.element);
			free(B.element);
			free(w.element);
			return false;
		}

		matrix yBT;
		yBT.row = y.row; yBT.col = BT.col; yBT.num = yBT.col*yBT.row;
		yBT.element = (double*)malloc(sizeof(double)*yBT.num);
		MatrixMultiply(&y, &BT, &yBT);
		
		matrix D;
		D.row = yBT.row;D.col = P.col; D.num = D.col*D.row;
		D.element = (double*)malloc(sizeof(double)*D.num);
		MatrixMultiply(&yBT, &P, &D);

		
		x.row = D.row; x.col = w.col; x.num = x.col*x.row;
		x.element = (double*)malloc(sizeof(double)*x.num);
		MatrixMultiply(&D, &w, &x);

		xyzt0.X = x.element[0] + xyzt0.X;
		xyzt0.Y = x.element[1] + xyzt0.Y;
		xyzt0.Z = x.element[2] + xyzt0.Z;
		xyzt0.t = x.element[3] + xyzt0.t;
		xyz0.X = x.element[0] + xyz0.X;
		xyz0.Y = x.element[1] + xyz0.Y;
		xyz0.Z = x.element[2] + xyz0.Z;
		if (GPSEPHEMSVCanBeUsednum > 4)
		{
			matrix wT;
			wT.col = w.row; wT.row = w.col; wT.num = w.num;
			wT.element= (double*)malloc(sizeof(double)*wT.num);
			MatrixTranspose(&w, &wT);
			matrix VTPV;
			VTPV.col = w.col; VTPV.row = wT.row; VTPV.num = VTPV.col*VTPV.row;
			VTPV.element=(double*)malloc(sizeof(double)*VTPV.num);
			MatrixMultiply(&wT, &w, &VTPV);
			*sigma0 = sqrt(VTPV.element[0]/(GPSEPHEMSVCanBeUsednum-4));
			*Dop = sqrt(y.element[0] + y.element[5] + y.element[10]);
			free(wT.element);
			free(VTPV.element);
		}

		free(y.element);
		free(Mul1.element);
		free(Mul2.element);
		free(P.element);
		free(BT.element);
		free(B.element);
		free(w.element);

		free(D.element);
		free(yBT.element);

		xyz->X = xyzt0.X;
		xyz->Y = xyzt0.Y;
		xyz->Z = xyzt0.Z;
		if ((fabs(x.element[0])>0.0000001) || (fabs(x.element[1])>0.0000001) || (fabs(x.element[2])>0.0000001) || (fabs(x.element[3])>0.0000001)) {
			whileflag = true;
		}
		else {
			whileflag = false;
		}
		free(x.element);
	} while (whileflag);

	

	/************************************************************************/
	/*            测速模块;                                
	作者：李涛;
	注意事项,速度不需要迭代;每次速度算完必须要在外面置零;*/
	/************************************************************************/
	//free(x.element);
	GPSTIME rectime;
	rectime = rawdata->Obs->TIME;
	SVPositinAndSpeed SVXYZandXYZpie;
	double delta_tsv_L1, delta_tsv_L1pie;
	matrix B, w, P;
	B.col = 4;
	B.row = GPSEPHEMSVCanBeUsednum;
	B.num = 4 * GPSEPHEMSVCanBeUsednum;
	B.element = (double*)malloc(sizeof(double)*B.num);
	w.col = 1;
	w.num = GPSEPHEMSVCanBeUsednum;
	w.row = GPSEPHEMSVCanBeUsednum;
	w.element = (double*)malloc(sizeof(double)*w.num);
	P.col = GPSEPHEMSVCanBeUsednum;
	P.row = GPSEPHEMSVCanBeUsednum;
	P.num = GPSEPHEMSVCanBeUsednum * GPSEPHEMSVCanBeUsednum;
	P.element = (double*)malloc(sizeof(double)*P.num);
	for (int i = 0; i < P.num; i++)
	{
		P.element[i] = 0.0;
	}
	for (int i = 0; i < P.col; i++)
	{
		P.element[i + i*P.col] = 1;
	}
	int j = 0;
	for (int i = 0; i < rawdata->Obsnum; i++)
	{
		CalculateSVPositionOfTransimit(rawdata, rawdata->GPSEPH[rawdata->Obs[i].PRN - 1].PRN, i, &xyzt0, &SVXYZandXYZpie, &delta_tsv_L1, &delta_tsv_L1pie);
		if (GPSEPHEMCanBeUsed(rawdata, rawdata->Obs[i].PRN, i))
		{
			XYZ SVXYZ;
			SVXYZ.X = SVXYZandXYZpie.SVPositon_after_wecorret.X;
			SVXYZ.Y = SVXYZandXYZpie.SVPositon_after_wecorret.Y;
			SVXYZ.Z = SVXYZandXYZpie.SVPositin.Z;
			BLH blh;
			XYZToBLH(&xyz0, &blh);
			double Rij = sqrt(pow(xyz0.X - SVXYZ.X, 2) + pow(xyz0.Y - SVXYZ.Y, 2) + pow(xyz0.Z - SVXYZ.Z, 2));
			if (blh.H > 70)
			{
				double rho = rawdata->Obs[i].psr - xyzt0.t + c*delta_tsv_L1 - d_ion - d_trop;
				FILE *fwrong;
				fwrong = fopen("WrongData.txt", "a+");
				fprintf(fwrong,"%8.4f  %8.4f  %8.4f  %8.4f  %8.4f \n", rawdata->Obs->TIME.SecOfWeek,SVXYZ.X,SVXYZ.Y, SVXYZ.Z, rho);
				fclose(fwrong);
			}
			double rhopie = (SVXYZ.X-xyz0.X)*SVXYZandXYZpie.SVSpeed.Xdot / Rij+ (SVXYZ.Y - xyz0.Y)*SVXYZandXYZpie.SVSpeed.Ydot / Rij+ (SVXYZ.Z - xyz0.Z)*SVXYZandXYZpie.SVSpeed.Zdot / Rij;
			w.element[j]= -(rawdata->Obs[i].dopp*lambda1) - rhopie + c*delta_tsv_L1pie;
			B.element[4 * j] = (xyz0.X - SVXYZ.X) / Distance(&xyz0, &SVXYZ);
			B.element[4 * j + 1] = (xyz0.Y - SVXYZ.Y) / Distance(&xyz0, &SVXYZ);
			B.element[4 * j + 2] = (xyz0.Z - SVXYZ.Z) / Distance(&xyz0, &SVXYZ);
			B.element[4 * j + 3] = 1.0;
			j++;
		}
	}
	matrix BT;
	BT.col = B.row; BT.row = B.col; BT.num = B.num;
	BT.element = (double*)malloc(sizeof(double)*BT.num);
	MatrixTranspose(&B, &BT);
	matrix Mul1;
	Mul1.row = BT.row; Mul1.col = P.col;  Mul1.num = Mul1.col*Mul1.row;
	Mul1.element = (double*)malloc(sizeof(double)*Mul1.num);
	MatrixMultiply(&BT, &P, &Mul1);
	matrix Mul2;
	Mul2.row = Mul1.row; Mul2.col = B.col; Mul2.num = Mul2.col*Mul2.row;
	Mul2.element = (double*)malloc(sizeof(double)*Mul2.num);
	MatrixMultiply(&Mul1, &B, &Mul2);
	matrix y;
	y.col = B.col; y.row = B.col; y.num = B.col*B.col;
	y.element = (double*)malloc(sizeof(double)*y.num);
	MatrixInv(B.col, Mul2.element, y.element);
	if (MatrixInv(B.col, Mul2.element, y.element) == 0)
	{
		free(B.element);
		free(y.element);
		free(Mul1.element);
		free(Mul2.element);
		free(P.element);
		free(BT.element);
		free(w.element);
		return false;
	}
	
	matrix yBT;
	yBT.row = y.row; yBT.col = BT.col; yBT.num = yBT.col*yBT.row;
	yBT.element = (double*)malloc(sizeof(double)*yBT.num);
	MatrixMultiply(&y, &BT, &yBT);
	
	matrix D;
	D.row = yBT.row; D.col = P.col; D.num = D.col*D.row;
	D.element = (double*)malloc(sizeof(double)*D.num);
	MatrixMultiply(&yBT, &P, &D);
	xpie.row = D.row; xpie.col = w.col; xpie.num = xpie.col*xpie.row;
	xpie.element = (double*)malloc(sizeof(double)*xpie.num);
	MatrixMultiply(&D, &w, &xpie);
	xyzdot->Xdot = xyzdot->Xdot + xpie.element[0];
	xyzdot->Ydot = xyzdot->Ydot + xpie.element[1];
	xyzdot->Zdot = xyzdot->Zdot + xpie.element[2];

	free(B.element);
	free(y.element);
	free(Mul1.element);
	free(Mul2.element);
	free(P.element);
	free(BT.element);
	free(w.element);

	free(yBT.element);
	free(D.element);
	free(xpie.element);
	return true;
}





bool CalculateSVPositionOfTransimit(RawData* rawdata, unsigned long PRN, int i,XYZt* xyzt0, SVPositinAndSpeed* PS, double* delta_tsv_L1, double* delta_tsv_L1pie)
{
	GPSTIME SVTimeOfTransimit;
	
	CalculateSVTimeOfTransimit(rawdata, PRN, i, xyzt0,&SVTimeOfTransimit);
	if (CalulateSVPositionAndSVclock(SVTimeOfTransimit, PRN, rawdata, PS, delta_tsv_L1, delta_tsv_L1pie) == true)
	{
		double t = 608400 * (rawdata->Obs[i].TIME.Week - SVTimeOfTransimit.Week)+ rawdata->Obs[i].TIME.SecOfWeek - xyzt0->t/c - SVTimeOfTransimit.SecOfWeek;
		do
		{
			if (t > 608400)
			{
				t = t - 608400;
			}
			else if (t < 0)
			{
				t = t + 608400;
			}
		} while ((t < 0) || (t > 608400));
		//PS->SVPositon_after_wecorret.X = wedot*t*PS->SVPositin.Y + PS->SVPositin.X;
		//PS->SVPositon_after_wecorret.Y = -wedot*t*PS->SVPositin.X + PS->SVPositin.Y;

		double rad = wedot*t;
		PS->SVPositon_after_wecorret.X = sin(wedot*t)*PS->SVPositin.Y + cos(wedot*t)* PS->SVPositin.X;
		PS->SVPositon_after_wecorret.Y = -sin(wedot*t)*PS->SVPositin.X + cos(wedot*t)*PS->SVPositin.Y;
		//地球自转改正项;
		return true;
	}
	else
	{
		return false;
	}
}




void CalculateSVTimeOfTransimit(RawData * rawdata, unsigned long PRN,int i, XYZt* xyzt0,GPSTIME *SVTimeOfTransimit)
{
	SVPositinAndSpeed pvsod;
	SVTimeOfTransimit->SecOfWeek = rawdata->Obs[i].TIME.SecOfWeek+ xyzt0->t/c - rawdata->Obs[i].psr / c;
	if (SVTimeOfTransimit->SecOfWeek > 0)
	{
		SVTimeOfTransimit->Week = rawdata->Obs[i].TIME.Week;
	}
	else
	{
		SVTimeOfTransimit->Week = rawdata->Obs[i].TIME.Week - 1;
		SVTimeOfTransimit->SecOfWeek = SVTimeOfTransimit->SecOfWeek + 604800;
	}

	double SVClockDiff, SVClockDiffpie;
	CalulateSVPositionAndSVclock(*SVTimeOfTransimit, PRN, rawdata, &pvsod, &SVClockDiff, &SVClockDiffpie);

	SVTimeOfTransimit->SecOfWeek = rawdata->Obs[i].TIME.SecOfWeek+ xyzt0->t / c - rawdata->Obs[i].psr /c- SVClockDiff;
	if (SVTimeOfTransimit->SecOfWeek > 0)
	{
		SVTimeOfTransimit->Week = rawdata->Obs[i].TIME.Week;
	}
	else
	{
		SVTimeOfTransimit->Week = rawdata->Obs[i].TIME.Week - 1;
		SVTimeOfTransimit->SecOfWeek = SVTimeOfTransimit->SecOfWeek + 604800;
	}

}


