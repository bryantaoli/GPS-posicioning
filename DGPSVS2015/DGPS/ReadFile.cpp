#include"Datadecoding.h"
#include"Referencechange.h"
#include"Timechange.h"
#include "ReadFile.h"
#include <math.h>


//观测值43，星历7，定位结果47，电离层8;
unsigned short ReadOmestarBinFile(FILE *Fin, RawData* rawdata)
{
	unsigned short Retdata;
	//unsigned char buffer[4096];
	unsigned char buffer[70000];
	//fread返回的不是字节数，当且仅当下面这么用的时候，返回值才是字节数（当然因为恰好一个数据块大小为1个字节，相当于realRead * 1）

	//realRead = fread(buff, 1, size, fp);

	//fread(buff, size, 1, fp)每次读size个字节，读一次;
	//如果是: fread(buff, size, 1, fp)   返回1表示读取了size字节，返回0表示读取数量不够size字节;
	if (fread(buffer, 3, 1, Fin) == 0) return -1;
	while (1)
	{
		if (buffer[0] == 0xAA && buffer[1] == 0x44 && buffer[2] == 0x12)
		{
			break;
		}
		else
		{
			buffer[0] = buffer[1];
			buffer[1] = buffer[2];
			if (fread(buffer + 2, 1, 1, Fin) == 0) return -1;//切忌不能这么写buffer[2]= fread(buffer, 1, 1, Fin);
		}
	}

	/*****************fread一下指针就会移位************************/

	if (fread(buffer + 3, 1, 1, Fin) == 0)
	{
		return -1;
	}
	//HeaderLgth头的长度，包括前三个字节aa4412
	int HeaderLgth = buffer[3];
	if (fread(buffer + 4, HeaderLgth - 4, 1, Fin) == 0)return -1;
	//MessageLength消息长度，不包括最后四个crc校验字节

	//这边有毒，刚开始以为消息的长度和头的长度都是一个字节，没想到消息长度是两个字节啊
	unsigned short MessageLength = Ushort2(buffer + 8);
	//MessageLength = unsigned short(buffer[8]*256+ buffer[9]);
	if (fread(buffer + HeaderLgth, MessageLength, 1, Fin) == 0)return-1;

	if (fread(buffer + HeaderLgth + MessageLength, 4, 1, Fin) == 0)return -1;

	//这里可得用unsigned char，用char的话就会有很多数据通不过检验，而且现象都是crc某位数上差1;
	unsigned char R[] = { buffer[HeaderLgth + MessageLength],buffer[HeaderLgth + MessageLength + 1],buffer[HeaderLgth + MessageLength + 2] ,buffer[HeaderLgth + MessageLength + 3] };


	if (crc32(buffer, HeaderLgth + MessageLength) == Uint4(R))
	{
		switch (buffer[4])
		{
		case 7:
			GPSEPHEMrenew(buffer, HeaderLgth, rawdata); Retdata = 7; break;
		case 8:
			IONrenew(buffer, HeaderLgth, &rawdata->ION); Retdata = 8; break;
		case 47:
			OmestarPOSrenew(buffer, HeaderLgth, &rawdata->OMESTARPOS); Retdata = 47; break;
		case 43:
			GPSObsGet(buffer, HeaderLgth, rawdata);
			//printf("%lf   %d%\n", rawdata->Obs->TIME.SecOfWeek, rawdata->Obsnum);
			Retdata = 43;
			break;
		default:
			break;
		}

	}
	return Retdata;
}

bool ReadRTCMFile(FILE * RTCMFile, RTCM1 * R, unsigned char *d29last, unsigned char *d30last)
{
	RTCM1 R_temp ;
StartOfReadRTCM:unsigned char Preamble1 = 0;unsigned char Preamble2 = 0;
	unsigned char Preamble3 = 0;unsigned char Preamble4 = 0;unsigned char Preamble5 = 0;

	while (1)
	{
		if (fread(&Preamble1, 1, 1, RTCMFile) == 0) return false;

		if ((Preamble1 == 0x59)||(Preamble1 == 0x66))
		{
			break;
		}
	}

	do
	{
		fread(&Preamble2, 1, 1, RTCMFile);
	} while ((Preamble2 < 64) && (Preamble2 > 127));

	do
	{
		fread(&Preamble3, 1, 1, RTCMFile);
	} while ((Preamble3 < 64) && (Preamble3 > 127));

	do
	{
		fread(&Preamble4, 1, 1, RTCMFile);
	} while ((Preamble4 < 64) && (Preamble3 > 127));

	do
	{
		fread(&Preamble5, 1, 1, RTCMFile);
	} while ((Preamble5 < 64) && (Preamble5 > 127));

	//把数据前的01都去掉;
	unsigned char ByteBeforeRoll1 = 63 & Preamble1;unsigned char ByteBeforeRoll2 = 63 & Preamble2;unsigned char ByteBeforeRoll3 = 63 & Preamble3;
	unsigned char ByteBeforeRoll4 = 63 & Preamble4;unsigned char ByteBeforeRoll5 = 63 & Preamble5;

	//滚动;
	unsigned char ByteAfterRoll[5];
	ByteAfterRoll[0] = 32 * (1 & ByteBeforeRoll1) + 16 * (1 & (ByteBeforeRoll1 >> 1)) + 8 * (1 & (ByteBeforeRoll1 >> 2)) + 4 * (1 & (ByteBeforeRoll1 >> 3)) + 2 * (1 & (ByteBeforeRoll1 >> 4)) + 1 * (1 & (ByteBeforeRoll1 >> 5));
	ByteAfterRoll[1] = 32 * (1 & ByteBeforeRoll2) + 16 * (1 & (ByteBeforeRoll2 >> 1)) + 8 * (1 & (ByteBeforeRoll2 >> 2)) + 4 * (1 & (ByteBeforeRoll2 >> 3)) + 2 * (1 & (ByteBeforeRoll2 >> 4)) + 1 * (1 & (ByteBeforeRoll2 >> 5));
	ByteAfterRoll[2] = 32 * (1 & ByteBeforeRoll3) + 16 * (1 & (ByteBeforeRoll3 >> 1)) + 8 * (1 & (ByteBeforeRoll3 >> 2)) + 4 * (1 & (ByteBeforeRoll3 >> 3)) + 2 * (1 & (ByteBeforeRoll3 >> 4)) + 1 * (1 & (ByteBeforeRoll3 >> 5));
	ByteAfterRoll[3] = 32 * (1 & ByteBeforeRoll4) + 16 * (1 & (ByteBeforeRoll4 >> 1)) + 8 * (1 & (ByteBeforeRoll4 >> 2)) + 4 * (1 & (ByteBeforeRoll4 >> 3)) + 2 * (1 & (ByteBeforeRoll4 >> 4)) + 1 * (1 & (ByteBeforeRoll4 >> 5));
	ByteAfterRoll[4] = 32 * (1 & ByteBeforeRoll5) + 16 * (1 & (ByteBeforeRoll5 >> 1)) + 8 * (1 & (ByteBeforeRoll5 >> 2)) + 4 * (1 & (ByteBeforeRoll5 >> 3)) + 2 * (1 & (ByteBeforeRoll5 >> 4)) + 1 * (1 & (ByteBeforeRoll5 >> 5));


	//这里找0x59 10那个有Bug,因为如果上一次的30位是1那要取补码;
	//如果上一次的30位是1那要取补码;
	if (*d30last == 1)
	{
		ByteAfterRoll[0] = ~(ByteAfterRoll[0] + 128 + 64);ByteAfterRoll[1] = ~(ByteAfterRoll[1] + 128 + 64);
		ByteAfterRoll[2] = ~(ByteAfterRoll[2] + 128 + 64);ByteAfterRoll[3] = ~(ByteAfterRoll[3] + 128 + 64);
	}
	//字符合并;
	unsigned char Combined[4];
	Combined[0] = (ByteAfterRoll[0] << 2) + (3 & (ByteAfterRoll[1] >> 4));Combined[1] = ((ByteAfterRoll[1] << 4) & 240) + ((ByteAfterRoll[2] >> 2) & 15);
	Combined[2] = ((ByteAfterRoll[2] << 6) & 192) + (ByteAfterRoll[3] & 63);Combined[3] = ByteAfterRoll[4];

	//取出接收到的校验码;
	unsigned char OECRec[6];
	OECRec[0] = 1 & (ByteAfterRoll[4] >> 5); OECRec[1] = 1 & (ByteAfterRoll[4] >> 4); OECRec[2] = 1 & (ByteAfterRoll[4] >> 3);
	OECRec[3] = 1 & (ByteAfterRoll[4] >> 2); OECRec[4] = 1 & (ByteAfterRoll[4] >> 1); OECRec[5] = 1 & (ByteAfterRoll[4] >> 0);

	//进行奇偶校验;不通过重新去找头;
	if (OEC(OECRec, ByteAfterRoll, d29last, d30last) == false)
	{
		//虽然奇偶校验不通过，但是29.30位还是要更新;???;
		//*d29last = OECRec[4]; *d30last = OECRec[5];
		RefreshRTCM1(&R_temp);
		goto StartOfReadRTCM;
	}

	//奇偶校验的第一位应该移五位得到！;
	//更新接下来要用的29,30位;
	else
	{
		*d29last = OECRec[4]; *d30last = OECRec[5];
	}

	//表示不是同步字;
	if ((((Preamble1 == 0x59) && (Preamble2 & 3) != 2)) || ((Preamble1 == 0x66) && (Preamble2 & 3) != 1))
	{
		RefreshRTCM1(&R_temp);
		fseek(RTCMFile, -4, SEEK_CUR);
		goto StartOfReadRTCM;
	}

	//把收到的消息放到消息头的WORD1里;
	R_temp.header.RTCMType = (Combined[1] >> 2) & 63;
	if (R_temp.header.RTCMType != 1)
	{
		RefreshRTCM1(&R_temp);
		fseek(RTCMFile, -4, SEEK_CUR);
		goto StartOfReadRTCM;
	}
	R_temp.header.BaseStationID = (Combined[1] & 3) * 256+ Combined[2];

	//读五个字进来;
	if(ReadFiveByte(RTCMFile, &R_temp, d29last, d30last,Combined)==false)//读头第二部分;
	{
		RefreshRTCM1(&R_temp);
		goto StartOfReadRTCM;
	}

	//把刚读进来的5个字合并的东西根据规则拆开放到消息头的WORD2里;
	R_temp.header.Zcount = Combined[0] * 32 + ((Combined[1] >> 3) & 31);
	R_temp.header.LenOfMessage = ((Combined[2] & 248) >> 3);
	R_temp.header.StateOfWork = Combined[2] & 7;

	int CycleNum =int(R_temp.header.LenOfMessage / 5);
	int remainder = R_temp.header.LenOfMessage % 5;

	unsigned char Combined1[4];
	unsigned char Combined2[4];
	unsigned char Combined3[4];
	unsigned char Combined4[4];

	switch ((R_temp.header.LenOfMessage)%5)
	{
	case 4:
		for (int i = 0; i < CycleNum; i++)
		{
			if (ReadOnePage(RTCMFile, &R_temp, d29last, d30last) == false)
			{
				RefreshRTCM1(&R_temp);
				goto StartOfReadRTCM;
			}
		}
		//读五个字;如果失败为什么不回到开头再去找同步字？
		
		if (ReadFiveByte(RTCMFile, &R_temp, d29last, d30last, Combined1) == false)
		{
			RefreshRTCM1(&R_temp);
			goto StartOfReadRTCM;
			return false;
		}
		if (ReadFiveByte(RTCMFile, &R_temp, d29last, d30last, Combined2) == false)
		{
			RefreshRTCM1(&R_temp);
			goto StartOfReadRTCM;
			return false;
		}
		if (ReadFiveByte(RTCMFile, &R_temp, d29last, d30last, Combined3) == false)
		{
			RefreshRTCM1(&R_temp);
			goto StartOfReadRTCM;
			return false;
		}
		if (ReadFiveByte(RTCMFile, &R_temp, d29last, d30last, Combined4) == false)
		{
			RefreshRTCM1(&R_temp);
			goto StartOfReadRTCM;
			return false;
		}
		DecodeRTCMFirstSV(Combined1, Combined2, &R_temp);

		DecodeRTCMSecondSV(Combined2, Combined3, Combined4, &R_temp);

		break;

	case 2:
		for (int i = 0; i < CycleNum; i++)
		{
			if (ReadOnePage(RTCMFile, &R_temp, d29last, d30last) == false)
			{
				RefreshRTCM1(&R_temp);
				goto StartOfReadRTCM;
			}
		}
		if (ReadFiveByte(RTCMFile, &R_temp, d29last, d30last, Combined1) == false)
		{
			RefreshRTCM1(&R_temp);
			goto StartOfReadRTCM;
			return false;
		}
		if (ReadFiveByte(RTCMFile, &R_temp, d29last, d30last, Combined2) == false)
		{
			RefreshRTCM1(&R_temp);
			goto StartOfReadRTCM;
			return false;
		}
		DecodeRTCMFirstSV(Combined1, Combined2, &R_temp);

		break;

	case 0:
		for (int i = 0; i < CycleNum; i++)
		{
			if (ReadOnePage(RTCMFile, &R_temp, d29last, d30last) == false)
			{
				RefreshRTCM1(&R_temp);
				goto StartOfReadRTCM;
			}
		}
		break;
	case 1:
		for (int i = 0; i < CycleNum; i++)
		{
			if (ReadOnePage(RTCMFile, &R_temp, d29last, d30last) == false)
			{
				RefreshRTCM1(&R_temp);
				goto StartOfReadRTCM;
			}
		}
		break;
	case 3:
		for (int i = 0; i < CycleNum; i++)
		{
			if (ReadOnePage(RTCMFile, &R_temp, d29last, d30last) == false)
			{
				RefreshRTCM1(&R_temp);
				goto StartOfReadRTCM;
			}
		}
		break;
	}

	//最后全部奇偶校验都通过了把R_temp的值再赋给R;
	 R->header= R_temp.header;

	for (int i = 0; i < 32; i++)
	{
		if(R_temp.rtcm1body[i].s==0)
		{
			R->rtcm1body[i] = R_temp.rtcm1body[i];
		}
	}
	return true;
}





bool ReadFiveByte(FILE * RTCMFile, RTCM1 * R_temp, unsigned char *d29last, unsigned char *d30last, unsigned char* Combined)
{
	unsigned char ByteInitial[5];
	for (int i = 0; i < 5; i++)
	{
		do
		{
			fread(ByteInitial + i, 1, 1, RTCMFile);
		} while ((ByteInitial[i] < 64) && (ByteInitial[i] > 127));
	}
	unsigned char ByteBeforeRoll[5];
	for (int i = 0; i < 5; i++)
	{
		ByteBeforeRoll[i] = ByteInitial[i] & 63;
	}

	//滚动;
	unsigned char ByteAfterRoll[5];
	for (int i = 0; i < 5; i++)
	{
		ByteAfterRoll[i] = 32 * (1 & ByteBeforeRoll[i]) + 16 * (1 & (ByteBeforeRoll[i] >> 1)) + 8 * (1 & (ByteBeforeRoll[i] >> 2)) \
			+ 4 * (1 & (ByteBeforeRoll[i] >> 3)) + 2 * (1 & (ByteBeforeRoll[i] >> 4)) + 1 * (1 & (ByteBeforeRoll[i] >> 5));
	}
	if (*d30last == 1)
	{
		ByteAfterRoll[0] = ~(ByteAfterRoll[0] + 128 + 64); ByteAfterRoll[1] = ~(ByteAfterRoll[1] + 128 + 64);
		ByteAfterRoll[2] = ~(ByteAfterRoll[2] + 128 + 64); ByteAfterRoll[3] = ~(ByteAfterRoll[3] + 128 + 64);
		//ByteAfterRoll5 = ~(ByteAfterRoll5 + 128 + 64);奇偶校验位不能取补码！;
	}

	Combined[0] = (ByteAfterRoll[0] << 2) + (3 & (ByteAfterRoll[1] >> 4)); Combined[1] = ((ByteAfterRoll[1] << 4) & 240) + ((ByteAfterRoll[2] >> 2) & 15);
	Combined[2] = ((ByteAfterRoll[2] << 6) & 192) + (ByteAfterRoll[3] & 63); Combined[3] = ByteAfterRoll[4];

	unsigned char OECRec[6];
	OECRec[0] = 1 & (ByteAfterRoll[4] >> 5); OECRec[1] = 1 & (ByteAfterRoll[4] >> 4); OECRec[2] = 1 & (ByteAfterRoll[4] >> 3);
	OECRec[3] = 1 & (ByteAfterRoll[4] >> 2); OECRec[4] = 1 & (ByteAfterRoll[4] >> 1); OECRec[5] = 1 & (ByteAfterRoll[4] >> 0);

	//不能放这，不然还没奇偶校验就改了29,30!;//d29last = OECRec[4];//d30last = OECRec[5];

	if (OEC(OECRec, ByteAfterRoll, d29last, d30last) == false)
	{
		*d29last = OECRec[4]; *d30last = OECRec[5];

		return false;
	}
	else
	{
		*d29last = OECRec[4]; *d30last = OECRec[5];
		return true;
	}

}

bool OEC(unsigned char * OECRec, unsigned char * ByteAfterRoll, unsigned char *d29last, unsigned char *d30last)
{
	unsigned char bit[30];

	bit[0] = 1 & (ByteAfterRoll[0] >> 5); bit[1] = 1 & (ByteAfterRoll[0] >> 4); bit[2] = 1 & (ByteAfterRoll[0] >> 3);
	bit[3] = 1 & (ByteAfterRoll[0] >> 2); bit[4] = 1 & (ByteAfterRoll[0] >> 1); bit[5] = 1 & (ByteAfterRoll[0] >> 0);
	bit[6] = 1 & (ByteAfterRoll[1] >> 5); bit[7] = 1 & (ByteAfterRoll[1] >> 4); bit[8] = 1 & (ByteAfterRoll[1] >> 3);
	bit[9] = 1 & (ByteAfterRoll[1] >> 2); bit[10] = 1 & (ByteAfterRoll[1] >> 1); bit[11] = 1 & (ByteAfterRoll[1] >> 0);
	bit[12] = 1 & (ByteAfterRoll[2] >> 5); bit[13] = 1 & (ByteAfterRoll[2] >> 4); bit[14] = 1 & (ByteAfterRoll[2] >> 3);
	bit[15] = 1 & (ByteAfterRoll[2] >> 2); bit[16] = 1 & (ByteAfterRoll[2] >> 1); bit[17] = 1 & (ByteAfterRoll[2] >> 0);
	bit[18] = 1 & (ByteAfterRoll[3] >> 5); bit[19] = 1 & (ByteAfterRoll[3] >> 4); bit[20] = 1 & (ByteAfterRoll[3] >> 3);
	bit[21] = 1 & (ByteAfterRoll[3] >> 2); bit[22] = 1 & (ByteAfterRoll[3] >> 1); bit[23] = 1 & (ByteAfterRoll[3] >> 0);

	bit[24] = bit[0] ^ bit[1] ^ bit[2] ^ bit[4] ^ bit[5] ^ bit[9] ^ bit[10] ^ bit[11] ^ bit[12] ^ bit[13] ^ bit[16] ^ bit[17] ^ bit[19] ^ bit[22] ^ (*d29last);

	bit[25] = bit[1] ^ bit[2] ^ bit[3] ^ bit[5] ^ bit[6] ^ bit[10] ^ bit[11] ^ bit[12] ^ bit[13] ^ bit[14] ^ bit[17] ^ bit[18] ^ bit[20] ^ bit[23] ^ (*d30last);

	bit[26] = bit[0] ^ bit[2] ^ bit[3] ^ bit[4] ^ bit[6] ^ bit[7] ^ bit[11] ^ bit[12] ^ bit[13] ^ bit[14] ^ bit[15] ^ bit[18] ^ bit[19] ^ bit[21] ^ (*d29last);

	bit[27] = bit[1] ^ bit[3] ^ bit[4] ^ bit[5] ^ bit[7] ^ bit[8] ^ bit[12] ^ bit[13] ^ bit[14] ^ bit[15] ^ bit[16] ^ bit[19] ^ bit[20] ^ bit[22] ^ (*d30last);

	bit[28] = bit[0] ^ bit[2] ^ bit[4] ^ bit[5] ^ bit[6] ^ bit[8] ^ bit[9] ^ bit[13] ^ bit[14] ^ bit[15] ^ bit[16] ^ bit[17] ^ bit[20] ^ bit[21] ^ bit[23] ^ (*d30last);

	bit[29] = bit[2] ^ bit[4] ^ bit[5] ^ bit[7] ^ bit[8] ^ bit[9] ^ bit[10] ^ bit[12] ^ bit[14] ^ bit[18] ^ bit[21] ^ bit[22] ^ bit[23] ^ (*d29last);

	if ((OECRec[0] != bit[24]) || (OECRec[1] != bit[25]) || (OECRec[2] != bit[26]) || (OECRec[3] != bit[27]) || (OECRec[4] != bit[28]) || (OECRec[5] != bit[29]))
	{
		return false;
	}
	else
	{
		return true;
	}

}

bool ReadOnePage(FILE * RTCMFile, RTCM1 * R_temp, unsigned char *d29last, unsigned char *d30last)
{

	unsigned char Combined1[4];
	if (ReadFiveByte(RTCMFile, R_temp, d29last, d30last, Combined1) == false)
	{
		return false;
	}
	unsigned char Combined2[4];
	if (ReadFiveByte(RTCMFile, R_temp, d29last, d30last, Combined2) == false)
	{
		return false;
	}
	unsigned char Combined3[4];
	if (ReadFiveByte(RTCMFile, R_temp, d29last, d30last, Combined3) == false)
	{
		return false;
	}
	unsigned char Combined4[4];
	if (ReadFiveByte(RTCMFile, R_temp, d29last, d30last, Combined4) == false)
	{
		return false;
	}
	unsigned char Combined5[4];
	if (ReadFiveByte(RTCMFile, R_temp, d29last, d30last, Combined5) == false)
	{
		return false;
	}
	//先判断是北斗(1)还是GPS(0)的差分信息;
	DecodeRTCMFirstSV(Combined1, Combined2, R_temp);

	DecodeRTCMSecondSV(Combined2, Combined3, Combined4, R_temp);

	DecodeRTCMThirdSV(Combined4, Combined5, R_temp);
	return true;
}


bool RefreshRTCM1(RTCM1 *R)
{
	R->header.RTCMType = 52428;
	R->header.BaseStationID = 3435973836;
	R->header.Zcount = 3435973836;
	R->header.LenOfMessage = 52428;
	R->header.StateOfWork = 204;
	for (int i = 0; i < 32; i++)
	{
		R->rtcm1body[i].s = 52428;
		R->rtcm1body[i].ScaleFactorForPRC = -9.2559631349317831e+61;
		R->rtcm1body[i].ScaleFactorForRRC = -9.2559631349317831e+61;
		R->rtcm1body[i].UDRE = 52428;
		R->rtcm1body[i].PRC = -9.2559631349317831e+61;
		R->rtcm1body[i].PRCwithoutScale = -13108;
		R->rtcm1body[i].RRC = -9.2559631349317831e+61;
		R->rtcm1body[i].RRCwithoutScale = -13108;
		R->rtcm1body[i].AOD = 52428;
		R->rtcm1body[i].TimeOfHour = -9.2559631349317831e+61;
	}
	return true;
}

bool DecodeRTCMFirstSV(unsigned char* Combined1, unsigned char* Combined2, RTCM1 *R_temp)
{
	if (((Combined1[0] >> 5) & 3) == 0)
	{
		unsigned char prn1 = (Combined1[0] & 31);
		if (prn1 == 0)
		{
			prn1 = 32;
		}
		R_temp->rtcm1body[prn1 - 1].UDRE = ((Combined1[0] >> 5) & 3);
		R_temp->rtcm1body[prn1 - 1].s = ((Combined1[0] >> 7) & 1);
		R_temp->rtcm1body[prn1 - 1].TimeOfHour = R_temp->header.Zcount*0.6;

		if (R_temp->rtcm1body[prn1 - 1].s == 0)
		{
			R_temp->rtcm1body[prn1 - 1].ScaleFactorForPRC = 0.02;
			R_temp->rtcm1body[prn1 - 1].ScaleFactorForRRC = 0.002;
		}
		else if (R_temp->rtcm1body[prn1 - 1].s == 1)
		{
			R_temp->rtcm1body[prn1 - 1].ScaleFactorForPRC = 0.32;
			R_temp->rtcm1body[prn1 - 1].ScaleFactorForRRC = 0.032;
		}

		R_temp->rtcm1body[prn1 - 1].PRCwithoutScale = short(Combined1[1] * 256 + Combined1[2]);
		R_temp->rtcm1body[prn1 - 1].PRC = R_temp->rtcm1body[prn1 - 1].PRCwithoutScale*R_temp->rtcm1body[prn1 - 1].ScaleFactorForPRC;
		R_temp->rtcm1body[prn1 - 1].RRC = R_temp->rtcm1body[prn1 - 1].ScaleFactorForRRC*char(Combined2[0]);
		R_temp->rtcm1body[prn1 - 1].AOD = Combined2[1];

	}
	return true;
}

bool DecodeRTCMSecondSV(unsigned char* Combined2, unsigned char* Combined3, unsigned char* Combined4, RTCM1 *R_temp)
{
	if (((Combined2[2] >> 5) & 3) == 0)
	{
		unsigned char prn2 = (Combined2[2] & 31);
		if (prn2 == 0)
		{
			prn2 = 32;
		}
		R_temp->rtcm1body[prn2 - 1].UDRE = ((Combined2[2] >> 5) & 3);
		R_temp->rtcm1body[prn2 - 1].s = ((Combined2[2] >> 7) & 1);
		if (R_temp->rtcm1body[prn2 - 1].s == 0)
		{
			R_temp->rtcm1body[prn2 - 1].ScaleFactorForPRC = 0.02;
			R_temp->rtcm1body[prn2 - 1].ScaleFactorForRRC = 0.002;
		}
		else if (R_temp->rtcm1body[prn2 - 1].s == 1)
		{
			R_temp->rtcm1body[prn2 - 1].ScaleFactorForPRC = 0.32;
			R_temp->rtcm1body[prn2 - 1].ScaleFactorForRRC = 0.032;
		}


		R_temp->rtcm1body[prn2 - 1].TimeOfHour = R_temp->header.Zcount*0.6;
		R_temp->rtcm1body[prn2 - 1].PRCwithoutScale = short(Combined3[0] * 256 + Combined3[1]);
		R_temp->rtcm1body[prn2 - 1].PRC = R_temp->rtcm1body[prn2 - 1].PRCwithoutScale * R_temp->rtcm1body[prn2 - 1].ScaleFactorForPRC;
		R_temp->rtcm1body[prn2 - 1].RRC = R_temp->rtcm1body[prn2 - 1].ScaleFactorForRRC*char(Combined3[2]);


		R_temp->rtcm1body[prn2 - 1].AOD = Combined4[0];

	}
	return true;
}


bool DecodeRTCMThirdSV(unsigned char* Combined4, unsigned char* Combined5, RTCM1 *R_temp)
{
	if (((Combined4[1] >> 5) & 3) == 0)
	{
		unsigned char prn3 = (Combined4[1] & 31);
		if (prn3 == 0)
		{
			prn3 = 32;
		}
		R_temp->rtcm1body[prn3 - 1].UDRE = ((Combined4[1] >> 5) & 3);
		R_temp->rtcm1body[prn3 - 1].s = ((Combined4[1] >> 7) & 1);
		if (R_temp->rtcm1body[prn3 - 1].s == 0)
		{
			R_temp->rtcm1body[prn3 - 1].ScaleFactorForPRC = 0.02;
			R_temp->rtcm1body[prn3 - 1].ScaleFactorForRRC = 0.002;
		}
		else if (R_temp->rtcm1body[prn3 - 1].s == 1)
		{
			R_temp->rtcm1body[prn3 - 1].ScaleFactorForPRC = 0.32;
			R_temp->rtcm1body[prn3 - 1].ScaleFactorForRRC = 0.032;
		}
		unsigned char CombinedBefore = Combined4[2];

		R_temp->rtcm1body[prn3 - 1].TimeOfHour = R_temp->header.Zcount*0.6;
		R_temp->rtcm1body[prn3 - 1].PRCwithoutScale = short(CombinedBefore * 256 + Combined5[0]);

		R_temp->rtcm1body[prn3 - 1].PRC = (R_temp->rtcm1body[prn3 - 1].PRCwithoutScale)*(R_temp->rtcm1body[prn3 - 1].ScaleFactorForPRC);
		R_temp->rtcm1body[prn3 - 1].RRC = R_temp->rtcm1body[prn3 - 1].ScaleFactorForRRC*char(Combined5[1]);
		R_temp->rtcm1body[prn3 - 1].AOD = Combined5[2];

	}
	return true;
}