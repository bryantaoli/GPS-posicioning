#pragma once
#include<stdio.h>
#include<stdlib.h>
#ifndef _REFERENCECHANGE_H
#define a 6378137.0000
#define b 6356752.3142
#define PI 3.1415926535898
#define e 0.081819190928906327
#define epie 0.082094438036854261
#define _REFERENCECHANGE_H
struct XYZ
{
	double X;
	double Y;
	double Z;
	XYZ()
	{
		X = 0.0;
		Y = 0.0;
		Z = 0.0;
	}
};

struct XYZt
{
	double X;
	double Y;
	double Z;
	double t;
	XYZt()
	{
		X = 0.0;
		Y = 0.0;
		Z = 0.0;
		t = 0.0;
	}
};

struct XYZdot
{
	double Xdot;
	double Ydot;
	double Zdot;
	XYZdot()
	{
		Xdot = 0.0;
		Ydot = 0.0;
		Zdot = 0.0;
	}
};

//BLH必须是弧度制

struct BLH
{
	double B;
	double L;
	double H;
	BLH()
	{
		B = 0.0;
		L = 0.0;
		H = 0.0;
	}
};


struct NEU
{
	double N;
	double E;
	double U;
	NEU()
	{
		N = 0.0;
		E = 0.0;
		U = 0.0;
	}
};

struct ENU
{
	double E;
	double N;
	double U;
	ENU()
	{
		E = 0.0;
		N = 0.0;
		U = 0.0;
	}
};

bool XYZToBLH(XYZ* A, BLH* B);
bool BLHToXYZ(BLH* B, XYZ* A);


bool XYZToENU(XYZ* A1, XYZ* A2, ENU* E);
//输出的N是A1在以A2为站心的站心坐标系下的坐标
bool XYZToNEU(XYZ* A1, XYZ* A2, NEU* N);

double Distance(XYZ* A1, XYZ* A2);
#endif // !_REFERENCECHANGE_H
