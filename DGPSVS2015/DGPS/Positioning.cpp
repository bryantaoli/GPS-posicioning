#include"Positioning.h"
#include"Datadecoding.h"
#include "matrix.h"
#include <math.h>
#include <float.h>
#define minE15 0.2617993877991494
#define minE10 0.1745329251994330
#define minE5 0.0872664625997164788
#define lambda1 0.1902936728

#define deltaRTCMandOBSMAX 20
//#define lambda1 0.19020
//#define lambda1 0.19

bool PositioningAndSpeeingFirst(RawData*rawdata,int GPSEPHEMSVCanBeUsednum,XYZ* xyz,XYZdot* xyzdot,double* Dop,double* sigma0)
{ 	
	XYZt xyzt0;
	XYZ xyz0;
	XYZ GeoCenter;
	double deltax = 10.0; double deltay = 10.0; double deltaz = 10.0; double deltacdt = 10.0;
	do
	{
		matrix x;
		GPSTIME rectime;
		rectime = rawdata->Obs->TIME;
		SVPositinAndSpeed SVXYZandXYZpie;
		double delta_tsv_L1, delta_tsv_L1pie;
		matrix B, w,P;
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
			if ((rawdata->Obs[i].PRN>0) && (rawdata->Obs[i].PRN < 33))
			{
				CalculateSVPositionOfTransimit(rawdata, rawdata->GPSEPH[rawdata->Obs[i].PRN - 1].PRN, i, &xyzt0, &SVXYZandXYZpie, &delta_tsv_L1, &delta_tsv_L1pie);
				if (GPSEPHEMCanBeUsed(rawdata, rawdata->Obs[i].PRN, i))
				{
					XYZ SVXYZ;
					SVXYZ.X = SVXYZandXYZpie.SVPositon_after_wecorret.X;
					SVXYZ.Y = SVXYZandXYZpie.SVPositon_after_wecorret.Y;
					SVXYZ.Z = SVXYZandXYZpie.SVPositin.Z;
					double d = Distance(&GeoCenter, &xyz0);
					if ((Distance(&GeoCenter, &xyz0) > a - 50000) && (Distance(&GeoCenter, &xyz0) < a + 50000))
					{
						BLH blh;
						XYZToBLH(&xyz0, &blh);
						double E = calculateE(&SVXYZ, &xyz0);
						double A = calculateA(&SVXYZ, &xyz0);
						double d_ion = Klobutchar(rectime, blh, E, A, rawdata->ION);
						double d_trop = Hopfield(blh.H, E * 180 / PI);
						w.element[j] = rawdata->Obs[i].psr - Distance(&xyz0, &SVXYZ) - xyzt0.t + c*delta_tsv_L1 - d_ion - d_trop;
						B.element[4 * j] = (xyz0.X - SVXYZ.X) / Distance(&xyz0, &SVXYZ);
						B.element[4 * j + 1] = (xyz0.Y - SVXYZ.Y) / Distance(&xyz0, &SVXYZ);
						B.element[4 * j + 2] = (xyz0.Z - SVXYZ.Z) / Distance(&xyz0, &SVXYZ);
						B.element[4 * j + 3] = 1.0;
						/*if (E < minE10)
						{
							P.element[i *GPSEPHEMSVCanBeUsednum +i] = 0.0;
						}*/
						if (E < 0)
						{
							P.element[i *GPSEPHEMSVCanBeUsednum + i] = 0.0;
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
		}
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
			free(w.element);
			return false;
		}
		free(Mul1.element);
		free(Mul2.element);

		matrix yBT;
		yBT.row = y.row; yBT.col = BT.col; yBT.num = yBT.col*yBT.row;
		yBT.element = (double*)malloc(sizeof(double)*yBT.num);
		MatrixMultiply(&y, &BT, &yBT);
		
		matrix D;
		D.row = yBT.row;D.col = P.col; D.num = D.col*D.row;
		D.element = (double*)malloc(sizeof(double)*D.num);
		MatrixMultiply(&yBT,&P,&D);

		free(P.element);
		free(BT.element);
		x.row = D.row; x.col = w.col; x.num = x.col*x.row;
		x.element = (double*)malloc(sizeof(double)*x.num);
		MatrixMultiply(&D,&w,&x);
		free(D.element);
		free(yBT.element);
		free(B.element);

		xyzt0.X = x.element[0] + xyzt0.X;
		xyzt0.Y = x.element[1] + xyzt0.Y;
		xyzt0.Z = x.element[2] + xyzt0.Z;
		xyzt0.t = x.element[3] + xyzt0.t;
		xyz0.X = x.element[0] + xyz0.X;
		xyz0.Y = x.element[1] + xyz0.Y;
		xyz0.Z = x.element[2] + xyz0.Z;

		deltax = x.element[0]; deltay = x.element[1]; deltaz = x.element[2]; deltacdt = x.element[3];
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
		free(w.element);
		xyz->X = xyzt0.X;
		xyz->Y = xyzt0.Y;
		xyz->Z = xyzt0.Z;
		free(x.element);
} while ((fabs(deltax)>0.0000001) || (fabs(deltay)>0.0000001) || (fabs(deltaz)>0.0000001) || (fabs(deltacdt)>0.0000001));

	matrix xpie;
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
		if ((rawdata->Obs[i].PRN>0) && (rawdata->Obs[i].PRN < 33))
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
				double rhopie = (SVXYZ.X - xyz0.X)*SVXYZandXYZpie.SVSpeed.Xdot / Rij + (SVXYZ.Y - xyz0.Y)*SVXYZandXYZpie.SVSpeed.Ydot / Rij + (SVXYZ.Z - xyz0.Z)*SVXYZandXYZpie.SVSpeed.Zdot / Rij;//把卫星速度投影到卫星接收机连线这一径向上;
				w.element[j] = -(rawdata->Obs[i].dopp*lambda1) - rhopie + c*delta_tsv_L1pie;
				B.element[4 * j] = (xyz0.X - SVXYZ.X) / Distance(&xyz0, &SVXYZ);
				B.element[4 * j + 1] = (xyz0.Y - SVXYZ.Y) / Distance(&xyz0, &SVXYZ);
				B.element[4 * j + 2] = (xyz0.Z - SVXYZ.Z) / Distance(&xyz0, &SVXYZ);
				B.element[4 * j + 3] = 1.0;
				j++;
			}
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
		free(y.element);
		free(Mul1.element);
		free(Mul2.element);
		free(P.element);
		free(BT.element);
		free(w.element);
		return false;
	}
	free(Mul1.element);
	free(Mul2.element);
	matrix yBT;
	yBT.row = y.row; yBT.col = BT.col; yBT.num = yBT.col*yBT.row;
	yBT.element = (double*)malloc(sizeof(double)*yBT.num);
	MatrixMultiply(&y, &BT, &yBT);
	free(y.element);
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
	free(yBT.element);
	free(D.element);
	free(w.element);
	free(P.element);
	free(BT.element);
	free(xpie.element);
	return true;
}





//要把电离层对流层延迟改掉，外部接口处也要把可以用的卫星改成有差分信息的卫星
//这里只把基准站看到的卫星中有星历的用于定位（这是建立在基准站与流动站较近的情况下可以简化处理的策略）;
//后期若碰到流动站与基准站很远，或者基准站有好几个的时候，便不能采用这样的策略，要进一步修改;
bool PVWithRTD(RawData * rawdata, RTCM1 * R, int SVEPHEMAndRTCMCanBeUsednum, XYZ * xyz, XYZdot * xyzdot, double * Dop, double * sigma0)
{
	XYZt xyzt0;XYZ xyz0;XYZ GeoCenter;
	double deltax = 10.0; double deltay = 10.0; double deltaz = 10.0; double deltacdt = 10.0;
	do
	{
		matrix x;GPSTIME rectime;
		rectime = rawdata->Obs->TIME;
		SVPositinAndSpeed SVXYZandXYZpie;
		double delta_tsv_L1, delta_tsv_L1pie;
		matrix B, w, P;
		B.col = 4;
		B.row = SVEPHEMAndRTCMCanBeUsednum;
		B.num = 4 * SVEPHEMAndRTCMCanBeUsednum;
		B.element = (double*)malloc(sizeof(double)*B.num);
		w.col = 1;
		w.num = SVEPHEMAndRTCMCanBeUsednum;
		w.row = SVEPHEMAndRTCMCanBeUsednum;
		w.element = (double*)malloc(sizeof(double)*w.num);

		P.col = SVEPHEMAndRTCMCanBeUsednum;
		P.row = SVEPHEMAndRTCMCanBeUsednum;
		P.num = SVEPHEMAndRTCMCanBeUsednum * SVEPHEMAndRTCMCanBeUsednum;
		P.element = (double*)malloc(sizeof(double)*P.num);

		for (int i = 0; i < P.num; i++)
		{
			P.element[i] = 0.0;
		}
		for (int i = 0; i < P.col; i++)
		{
			P.element[i + i * P.col] = 1;
		}
		int j = 0;
		for (int i = 0; i < rawdata->Obsnum; i++)
		{
			if ((rawdata->Obs[i].PRN>0) && (rawdata->Obs[i].PRN < 33))
			{
				CalculateSVPositionOfTransimit(rawdata, rawdata->GPSEPH[rawdata->Obs[i].PRN - 1].PRN, i, &xyzt0, &SVXYZandXYZpie, &delta_tsv_L1, &delta_tsv_L1pie);
				//if (GPSEPHEMCanBeUsed(rawdata, rawdata->Obs[i].PRN, i))
				
				/********************************
				这里判断大于10秒后的处理需要考虑;虽然这里没有这个情况，为了增加鲁棒性需要考虑;
				**********************************/

				
				if(RTCMCanBeUsed(R, rawdata->Obs[i].PRN,rawdata))
				{
					double PRC;
					double OBSTimeOfHour = rawdata->Obs[i].TIME.SecOfWeek-int(rawdata->Obs[i].TIME.SecOfWeek / 3600)*3600;
					if ((fabs(OBSTimeOfHour - R->rtcm1body[rawdata->Obs[i].PRN - 1].TimeOfHour) > deltaRTCMandOBSMAX) && \
						(fabs(OBSTimeOfHour - R->rtcm1body[rawdata->Obs[i].PRN - 1].TimeOfHour + 3600) > deltaRTCMandOBSMAX) && \
						(fabs(OBSTimeOfHour - R->rtcm1body[rawdata->Obs[i].PRN - 1].TimeOfHour - 3600) > deltaRTCMandOBSMAX))
					{
						return false;
					}
					if (fabs(OBSTimeOfHour - R->rtcm1body[rawdata->Obs[i].PRN - 1].TimeOfHour) < deltaRTCMandOBSMAX+ 0.5)
					{
						PRC = R->rtcm1body[rawdata->Obs[i].PRN - 1].PRC + R->rtcm1body[rawdata->Obs[i].PRN - 1].RRC*\
							(OBSTimeOfHour - R->rtcm1body[rawdata->Obs[i].PRN - 1].TimeOfHour);
					}
					else if(fabs(OBSTimeOfHour - R->rtcm1body[rawdata->Obs[i].PRN - 1].TimeOfHour + 3600) < deltaRTCMandOBSMAX)
					{
						PRC = R->rtcm1body[rawdata->Obs[i].PRN - 1].PRC + R->rtcm1body[rawdata->Obs[i].PRN - 1].RRC*\
							(OBSTimeOfHour - R->rtcm1body[rawdata->Obs[i].PRN - 1].TimeOfHour + 3600);
					}
					else if (fabs(OBSTimeOfHour - R->rtcm1body[rawdata->Obs[i].PRN - 1].TimeOfHour - 3600) < deltaRTCMandOBSMAX)
					{
						PRC = R->rtcm1body[rawdata->Obs[i].PRN - 1].PRC + R->rtcm1body[rawdata->Obs[i].PRN - 1].RRC*\
							(OBSTimeOfHour - R->rtcm1body[rawdata->Obs[i].PRN - 1].TimeOfHour - 3600);
					}

					XYZ SVXYZ;
					SVXYZ.X = SVXYZandXYZpie.SVPositon_after_wecorret.X;
					SVXYZ.Y = SVXYZandXYZpie.SVPositon_after_wecorret.Y;
					SVXYZ.Z = SVXYZandXYZpie.SVPositin.Z;
					double d = Distance(&GeoCenter, &xyz0);
					if ((Distance(&GeoCenter, &xyz0) > a - 50000) && (Distance(&GeoCenter, &xyz0) < a + 50000)){
						BLH blh;
						XYZToBLH(&xyz0, &blh);
						double E = calculateE(&SVXYZ, &xyz0);
						double A = calculateA(&SVXYZ, &xyz0);
						double d_ion = Klobutchar(rectime, blh, E, A, rawdata->ION);
						double d_trop = Hopfield(blh.H, E * 180 / PI);
						/*w.element[j] = rawdata->Obs[i].psr - Distance(&xyz0, &SVXYZ) - xyzt0.t + c*delta_tsv_L1 - d_ion - d_trop;
						*/
						//计算差分改正数;
						if (fabs(d_ion + d_trop + PRC) > 10){
							w.element[j] = rawdata->Obs[i].psr - Distance(&xyz0, &SVXYZ) - xyzt0.t + c*delta_tsv_L1 - d_ion - d_trop;
							//P.element[i *SVEPHEMAndRTCMCanBeUsednum + i] = 0.0;
						}
						else{
							w.element[j] = rawdata->Obs[i].psr + PRC - Distance(&xyz0, &SVXYZ) - xyzt0.t + c*delta_tsv_L1;
						}

						B.element[4 * j] = (xyz0.X - SVXYZ.X) / Distance(&xyz0, &SVXYZ);
						B.element[4 * j + 1] = (xyz0.Y - SVXYZ.Y) / Distance(&xyz0, &SVXYZ);
						B.element[4 * j + 2] = (xyz0.Z - SVXYZ.Z) / Distance(&xyz0, &SVXYZ);
						B.element[4 * j + 3] = 1.0;
						/*if (E < minE10)
						{
						P.element[i *GPSEPHEMSVCanBeUsednum +i] = 0.0;
						}*/
						/*if (E < 0)
						{
							P.element[i *SVEPHEMAndRTCMCanBeUsednum + i] = 0.0;
						}*/
						j++;
					}
					else{
						w.element[j] = rawdata->Obs[i].psr - Distance(&xyz0, &SVXYZ) - xyzt0.t + c*delta_tsv_L1;
						B.element[4 * j] = (xyz0.X - SVXYZ.X) / Distance(&xyz0, &SVXYZ);
						B.element[4 * j + 1] = (xyz0.Y - SVXYZ.Y) / Distance(&xyz0, &SVXYZ);
						B.element[4 * j + 2] = (xyz0.Z - SVXYZ.Z) / Distance(&xyz0, &SVXYZ);
						B.element[4 * j + 3] = 1.0;
						j++;
					}
				}
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
		D.row = yBT.row; D.col = P.col; D.num = D.col*D.row;
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

		deltax = x.element[0]; deltay = x.element[1]; deltaz = x.element[2]; deltacdt = x.element[3];
		if (SVEPHEMAndRTCMCanBeUsednum > 4)
		{
			matrix wT;
			wT.col = w.row; wT.row = w.col; wT.num = w.num;
			wT.element = (double*)malloc(sizeof(double)*wT.num);
			MatrixTranspose(&w, &wT);
			matrix VTPV;
			VTPV.col = w.col; VTPV.row = wT.row; VTPV.num = VTPV.col*VTPV.row;
			VTPV.element = (double*)malloc(sizeof(double)*VTPV.num);
			MatrixMultiply(&wT, &w, &VTPV);
			*sigma0 = sqrt(VTPV.element[0] / (SVEPHEMAndRTCMCanBeUsednum - 4));
			*Dop = sqrt(y.element[0] + y.element[5] + y.element[10]);
			free(wT.element);
			free(VTPV.element);
		}

		free(Mul1.element);
		free(Mul2.element);
		free(P.element);
		free(BT.element);
		free(D.element);
		free(yBT.element);
		free(B.element);
		free(y.element);
		free(w.element);
		xyz->X = xyzt0.X;
		xyz->Y = xyzt0.Y;
		xyz->Z = xyzt0.Z;
		free(x.element);
	} while ((fabs(deltax)>0.00001) || (fabs(deltay)>0.0001) || (fabs(deltaz)>0.00001) || (fabs(deltacdt)>0.0001));

	matrix xpie;
	GPSTIME rectime;
	rectime = rawdata->Obs->TIME;
	SVPositinAndSpeed SVXYZandXYZpie;
	double delta_tsv_L1, delta_tsv_L1pie;
	matrix B, w, P;
	B.col = 4;
	B.row = SVEPHEMAndRTCMCanBeUsednum;
	B.num = 4 * SVEPHEMAndRTCMCanBeUsednum;
	B.element = (double*)malloc(sizeof(double)*B.num);
	w.col = 1;
	w.num = SVEPHEMAndRTCMCanBeUsednum;
	w.row = SVEPHEMAndRTCMCanBeUsednum;
	w.element = (double*)malloc(sizeof(double)*w.num);
	P.col = SVEPHEMAndRTCMCanBeUsednum;
	P.row = SVEPHEMAndRTCMCanBeUsednum;
	P.num = SVEPHEMAndRTCMCanBeUsednum * SVEPHEMAndRTCMCanBeUsednum;
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
		if ((rawdata->Obs[i].PRN>0) && (rawdata->Obs[i].PRN < 33))
		{
			CalculateSVPositionOfTransimit(rawdata, rawdata->GPSEPH[rawdata->Obs[i].PRN - 1].PRN, i, &xyzt0, &SVXYZandXYZpie, &delta_tsv_L1, &delta_tsv_L1pie);
			if (RTCMCanBeUsed(R, rawdata->Obs[i].PRN, rawdata))
			{
				XYZ SVXYZ;
				SVXYZ.X = SVXYZandXYZpie.SVPositon_after_wecorret.X;
				SVXYZ.Y = SVXYZandXYZpie.SVPositon_after_wecorret.Y;
				SVXYZ.Z = SVXYZandXYZpie.SVPositin.Z;
				BLH blh;
				XYZToBLH(&xyz0, &blh);
				double Rij = sqrt(pow(xyz0.X - SVXYZ.X, 2) + pow(xyz0.Y - SVXYZ.Y, 2) + pow(xyz0.Z - SVXYZ.Z, 2));
				double rhopie = (SVXYZ.X - xyz0.X)*SVXYZandXYZpie.SVSpeed.Xdot / Rij + (SVXYZ.Y - xyz0.Y)*SVXYZandXYZpie.SVSpeed.Ydot / Rij + (SVXYZ.Z - xyz0.Z)*SVXYZandXYZpie.SVSpeed.Zdot / Rij;//把卫星速度投影到卫星接收机连线这一径向上;
				w.element[j] = -(rawdata->Obs[i].dopp*lambda1) - rhopie + c*delta_tsv_L1pie;
				B.element[4 * j] = (xyz0.X - SVXYZ.X) / Distance(&xyz0, &SVXYZ);
				B.element[4 * j + 1] = (xyz0.Y - SVXYZ.Y) / Distance(&xyz0, &SVXYZ);
				B.element[4 * j + 2] = (xyz0.Z - SVXYZ.Z) / Distance(&xyz0, &SVXYZ);
				B.element[4 * j + 3] = 1.0;
				j++;
			}
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
		free(y.element);
		free(Mul1.element);
		free(Mul2.element);
		free(P.element);
		free(BT.element);
		free(w.element);
		return false;
	}
	free(Mul1.element);
	free(Mul2.element);
	matrix yBT;
	yBT.row = y.row; yBT.col = BT.col; yBT.num = yBT.col*yBT.row;
	yBT.element = (double*)malloc(sizeof(double)*yBT.num);
	MatrixMultiply(&y, &BT, &yBT);
	free(y.element);
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
	free(yBT.element);
	free(D.element);
	free(w.element);
	free(P.element);
	free(BT.element);
	free(xpie.element);
	return true;
}

bool CalculateSVPositionOfTransimit(RawData* rawdata, unsigned long PRN, int i, XYZt* xyzt0, SVPositinAndSpeed* PS, double* delta_tsv_L1, double* delta_tsv_L1pie)
{
	GPSTIME SVTimeOfTransimit;

	CalculateSVTimeOfTransimit(rawdata, PRN, i, xyzt0, &SVTimeOfTransimit);
	if (CalulateSVPositionAndSVclock(SVTimeOfTransimit, PRN, rawdata, PS, delta_tsv_L1, delta_tsv_L1pie) == true)
	{
		double t = 608400 * (rawdata->Obs[i].TIME.Week - SVTimeOfTransimit.Week) + rawdata->Obs[i].TIME.SecOfWeek - SVTimeOfTransimit.SecOfWeek + xyzt0->t / c;//用+还是-？？;
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


