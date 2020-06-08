#pragma once
#include"Datadecoding.h"

bool PositioningAndSpeeingFirst(RawData*rawdata,int IsGPSEPHEMSVnum, XYZ* xyz, XYZdot* xyzdot,double* Dop,double* sigma0);
void CalculateSVTimeOfTransimit(RawData*rawdata, unsigned long PRN,int i,XYZt* xyzt,GPSTIME *SVTimeOfTransimit);
bool CalculateSVPositionOfTransimit(RawData*rawdata, unsigned long PRN,int i, XYZt* xyz0,SVPositinAndSpeed* PS, double* delta_tsv_L1,double *delta_tsv_L1pie);
