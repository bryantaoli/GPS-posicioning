#pragma once
#ifndef _DATADECODING_H
#define _DATADECODING_H
#define POLYCRC32 0xEDB88320u
#define MAXCHANNUM 15//ͨ����ȷ����12��;
#define MAXSATNUM 32
//ע������һ��Ҫ��������ֺ������.0,��Ȼ����ʱ�ֲ�������double�ͻ���int��;
#define c 299792458.0
#define u 398600441500000.0
#define wedot 0.000072921151467
#include"Timechange.h"
#include"Referencechange.h"
#include "ReadFile.h"

struct SVPositinAndSpeed
{
	unsigned long PRN;
	XYZ SVPositin;
	XYZ SVPositon_after_wecorret;
	XYZdot SVSpeed;
};

struct POS_calculate_by_me
{
	GPSTIME POStime;
	BLH POSblh;
};
struct IONUTC
{
	double alpha[4];
	double belta[4];
	IONUTC()
	{
		alpha[0] = 0; alpha[1] = 0; alpha[2] = 0; alpha[3] = 0;
		belta[0] = 0; belta[1] = 0; belta[2] = 0; belta[3] = 0;
	}//�����ʼ����Ŀ����Ϊ����Klobucharģ�͵�ʱ���ж��Ƿ�û�е�������ݣ������û�з���һ��0;
};

struct  GPSEPHEM
{
	GPSTIME EPHEMtime;
	unsigned long PRN;double tow;//time of week
	unsigned long health;unsigned long IODE1;unsigned long IODE2;unsigned long week;
	unsigned long z_week;double toe;//reference time for ephemeris
	double A;//Semi-major axis  metres;
	double delta_N;//mean motion difference   rad/s
	double M0;//mean anomaly of reference time   rad
	double ecc;//e=0����Բ,e>1��˫���ߣ�0<e<1����Բ
	double w;//������ྭ����;
	double cuc;double cus;double crc;double crs;double cic;double cis;double I0;double IDOT;//I0�ı仯��
	double w0;double rate_w0;unsigned long iodc;//issue of data clock
	double toc;double tgd;double a10;double a11;double a12;double N;//corrected mean motion
	double URA;
};
struct PSRPOS
{
	GPSTIME POStime;
	BLH POSblh;
};

struct GPSObs
{
	GPSTIME TIME;
	long PRN; double psr; double adr; float dopp; float CN0;
	float locktime;//�������������;
};

struct RawData
{
	IONUTC ION;
	PSRPOS OMESTARPOS;
	GPSEPHEM GPSEPH[MAXSATNUM];
	GPSObs Obs[MAXCHANNUM];
	short Obsnum;
};

//void BinRead(char *FileName,IONUTC *ION, PSRPOS *blh);


//�۲�ֵ43������7����λ���47�������8;
unsigned short ReadOmestarBinFile(FILE *Fin, RawData* rawdata);


unsigned int crc32(const unsigned char *buff, int len);


void IONrenew(unsigned char *buff, int HeaderLgth,IONUTC* ION);
void OmestarPOSrenew(unsigned char *buff,int HeaderLgth,PSRPOS*blh);
void GPSObsGet(unsigned char *buff, int HeaderLgth, RawData*raw);
void GPSEPHEMrenew(unsigned char *buff, int HeaderLgth, RawData*rawdata);

bool CalulateSVPositionAndSVclock(GPSTIME Time, unsigned long PRN, RawData*rawdata, SVPositinAndSpeed* pvofsv, double* delta_tsv_L1, double* delta_tsv_L1pie);
double Klobutchar(GPSTIME time, BLH blh,double E,double A,IONUTC ION);
double Hopfield(double H, double A);
bool IsGPSEPHEM(unsigned long PRN);
bool GPSEPHEMCanBeUsed(RawData* rawdata, unsigned long PRN,int i);
bool RTCMCanBeUsed(RTCM1 * R, unsigned long PRN, RawData * rawdata);

double calculateE(XYZ* SV, XYZ* Re);
double calculateA(XYZ* SV, XYZ* Re);

double R8(unsigned char *p);
float R4(unsigned char *p);
unsigned int Uint4(unsigned char *p);
int Int4(unsigned char *p);
unsigned short Ushort2(unsigned char * p);
unsigned long Ulong4(unsigned char * p);
long Long4(unsigned char * p);
short Short2(unsigned char * p);

#endif // !_DATADECODING_H
