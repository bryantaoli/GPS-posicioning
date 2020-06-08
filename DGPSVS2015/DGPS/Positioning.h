#pragma once
#include"Datadecoding.h"
#include "ReadFile.h"

bool PositioningAndSpeeingFirst(RawData*rawdata,int GPSEPHEMSVCanBeUsednum, XYZ* xyz, XYZdot* xyzdot,double* Dop,double* sigma0);
void CalculateSVTimeOfTransimit(RawData*rawdata, unsigned long PRN,int i,XYZt* xyzt,GPSTIME *SVTimeOfTransimit);
bool CalculateSVPositionOfTransimit(RawData*rawdata, unsigned long PRN,int i, XYZt* xyz0,SVPositinAndSpeed* PS, double* delta_tsv_L1,double *delta_tsv_L1pie);
bool PVWithRTD(RawData*rawdata,RTCM1* R ,int GPSEPHEMSVCanBeUsednum, XYZ* xyz, XYZdot* xyzdot, double* Dop, double* sigma0);