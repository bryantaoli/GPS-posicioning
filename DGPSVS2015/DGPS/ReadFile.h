#pragma once
#include"Datadecoding.h"
#include"Referencechange.h"
#include"Timechange.h"


//ÿ�����Ƕ�Ӧ��ʱ�䲻һ��������Ҫ��ʱ���RTCMBody��;

struct RTCMHeader
{
	unsigned short RTCMType;
	unsigned int BaseStationID;
	unsigned int Zcount;
	unsigned short LenOfMessage;
	unsigned char StateOfWork;
	//��ʼ���ṹ������壬�������ʼ�����´��ٶ����ʱ������ֵ�ǲ����ģ�
	//��Ȼ��Ҳ�ɷ���˼ά�����Ҫ�õ�ÿ�ζ�����ֵ�����ʱ�򣬾Ͳ���ʼ����;
	//���ԣ�Ҫֵ����ֱ�Ӱ����������������治���ˣ�;�����һ�ٲ���ʼ������;
	RTCMHeader()
	{
		RTCMType = 52428;
		BaseStationID = 3435973836;
		Zcount = 3435973836;
		LenOfMessage = 52428;
		StateOfWork = 204;
	}
};

struct RTCM1body
{
	unsigned short s;
	double ScaleFactorForPRC;
	double ScaleFactorForRRC;
	unsigned short UDRE;
	double PRC;
	short PRCwithoutScale;
	double RRC;
	short RRCwithoutScale;
	unsigned short AOD;
	double TimeOfHour;
	RTCM1body()
	{
		s = 52428;
		ScaleFactorForPRC = -9.2559631349317831e+61;
		ScaleFactorForRRC = -9.2559631349317831e+61;
		UDRE = 52428;
		PRC = -9.2559631349317831e+61;
		PRCwithoutScale = -13108;
		RRC= -9.2559631349317831e+61;
		RRCwithoutScale = -13108;
		AOD = 52428;
		TimeOfHour= -9.2559631349317831e+61;
	}
};

struct RTCM1
{
	//ע�⣬�ṹ���׽ṹ���ʱ�򣬳�ʼ���ṹ���������ڲ�Ĺ��캯��;
	RTCMHeader header;
	RTCM1body rtcm1body[32];
	RTCM1()
	{
		RTCMHeader();
		RTCM1body();
	}
};








bool ReadRTCMFile(FILE *RTCMFile, RTCM1 *R, unsigned char *d29last, unsigned char *d30last);
//bool ReadRTCMFile(FILE *RTCMFile,FILE*RTCMFilein, RTCM1 *R, unsigned char *d29last, unsigned char *d30last);
bool ReadFiveByte(FILE *RTCMFile, RTCM1 *R, unsigned char *d29last, unsigned char *d30last,unsigned char* Combined);
bool OEC(unsigned char* OECRec,unsigned char* ByteAfterRoll, unsigned char *d29last, unsigned char *d30last);
bool ReadOnePage(FILE *RTCMFile, RTCM1 *R, unsigned char *d29last, unsigned char *d30last);

bool RefreshRTCM1(RTCM1 *R);

bool DecodeRTCMFirstSV(unsigned char* Combined1, unsigned char* Combined2, RTCM1 *R_temp);

bool DecodeRTCMSecondSV(unsigned char* Combined2, unsigned char* Combined3, unsigned char* Combined4, RTCM1 *R_temp);
bool DecodeRTCMThirdSV(unsigned char* Combined4, unsigned char* Combined5, RTCM1 *R_temp);
//bool MakeRTCMAndOBSsametime(RTCM1 *R1, RawData *rawdata);
