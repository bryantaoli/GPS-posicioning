#include"Datadecoding.h"
#include"Referencechange.h"
#include"Timechange.h"
#include "ReadFile.h"


//�۲�ֵ43������7����λ���47�������8;
short ReadOmestarBinFile(FILE *Fin, RawData* rawdata)
{
	short Retdata;
	//unsigned char buffer[4096];
	unsigned char buffer[70000];
	//fread���صĲ����ֽ��������ҽ���������ô�õ�ʱ�򣬷���ֵ�����ֽ�������Ȼ��Ϊǡ��һ�����ݿ��СΪ1���ֽڣ��൱��realRead * 1��

	//realRead = fread(buff, 1, size, fp);

	//�����: fread(buff, size, 1, fp)   ����1��ʾ��ȡ��size�ֽڣ�����0��ʾ��ȡ��������size�ֽ�;
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
			if (fread(buffer + 2, 1, 1, Fin) == 0) return -1;//�мɲ�����ôдbuffer[2]= fread(buffer, 1, 1, Fin);
		}
	}

	/*****************freadһ��ָ��ͻ���λ************************/

	if (fread(buffer + 3, 1, 1, Fin) == 0) return -1;
	//HeaderLgthͷ�ĳ��ȣ�����ǰ�����ֽ�aa4412
	int HeaderLgth = buffer[3];
	if (fread(buffer + 4, HeaderLgth - 4, 1, Fin) == 0)return -1;
	//MessageLength��Ϣ���ȣ�����������ĸ�crcУ���ֽ�

	//����ж����տ�ʼ��Ϊ��Ϣ�ĳ��Ⱥ�ͷ�ĳ��ȶ���һ���ֽڣ�û�뵽��Ϣ�����������ֽڰ�
	unsigned short MessageLength = Ushort2(buffer + 8);
	if (fread(buffer + HeaderLgth, MessageLength, 1, Fin) == 0)return-1;
	if (fread(buffer + HeaderLgth + MessageLength, 4, 1, Fin) == 0)return -1;

	//����ɵ���unsigned char����char�Ļ��ͻ��кܶ�����ͨ�������飬����������crcĳλ���ϲ�1;
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