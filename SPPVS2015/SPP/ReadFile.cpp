#include"Datadecoding.h"
#include"Referencechange.h"
#include"Timechange.h"
#include "ReadFile.h"


//观测值43，星历7，定位结果47，电离层8;
short ReadOmestarBinFile(FILE *Fin, RawData* rawdata)
{
	short Retdata;
	//unsigned char buffer[4096];
	unsigned char buffer[70000];
	//fread返回的不是字节数，当且仅当下面这么用的时候，返回值才是字节数（当然因为恰好一个数据块大小为1个字节，相当于realRead * 1）

	//realRead = fread(buff, 1, size, fp);

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

	if (fread(buffer + 3, 1, 1, Fin) == 0) return -1;
	//HeaderLgth头的长度，包括前三个字节aa4412
	int HeaderLgth = buffer[3];
	if (fread(buffer + 4, HeaderLgth - 4, 1, Fin) == 0)return -1;
	//MessageLength消息长度，不包括最后四个crc校验字节

	//这边有毒，刚开始以为消息的长度和头的长度都是一个字节，没想到消息长度是两个字节啊
	unsigned short MessageLength = Ushort2(buffer + 8);
	if (fread(buffer + HeaderLgth, MessageLength, 1, Fin) == 0)return-1;
	if (fread(buffer + HeaderLgth + MessageLength, 4, 1, Fin) == 0)return -1;

	//这里可得用unsigned char，用char的话就会有很多数据通不过检验，而且现象都是crc某位数上差1;
	unsigned char R[] = { buffer[HeaderLgth + MessageLength],buffer[HeaderLgth + MessageLength + 1],buffer[HeaderLgth + MessageLength + 2] ,buffer[HeaderLgth + MessageLength + 3] };

	//int R1 = (R[0] & 255) + (R[0] >> 8) * 16 + (R[1] & 255) * 16 * 16 + (R[1] >> 8) * 256 * 16+ (R[2] & 255)*256*256 + (R[2] >> 8) * 16*256*256 + (R[3] & 255) * 256*256*256 + (R[3] >> 8) * 256*256*256 * 16;
	//int R1 = (R[0] & 255)  + (R[1] & 255) * 16 * 16 + (R[2] & 255)*256*256  + (R[3] & 255) * 256*256*256 ;

	//unsigned int R1 = R[0] + R[1] * 16 * 16 + R[2] * 256 * 256 + R[3] * 256 * 256 * 256;



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
			printf("%lf   %d%\n", rawdata->Obs->TIME.SecOfWeek, rawdata->Obsnum);
			Retdata = 43;
			break;
		default:
			break;
		}

	}
	return Retdata;
}