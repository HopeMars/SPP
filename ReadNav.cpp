#define MAXRINEX 25600//һ������ַ���������80���ɣ�
#define _CRT_SECURE_NO_WARNINGS
#include "SPP_head.h"
#include <stdarg.h>
#include <string>
#include <stdio.h>

//��Դ�ַ���src�������n���ַ���Ŀ���ַ���des����ȷ��Ŀ���ַ���ĩβû�пո�
extern void setstr(char* des, const char* src, int n)
{
	char* p = des;
	const char* q = src;
	while (*q && q < src + n)
	{
		*p++ = *q++;
	}
	*p-- = '\0';
	//ȥ��β���ո�
	while (p >= des && *p == ' ')
	{
		*p-- = '\0';
	}
}
//��ȡ�ļ����ݿ���������END OF HEADER��ʼ����
extern int getrow(FILE* fp_nav)
{
	int row = 0;
	int flag = 0;
	char buff[MAXRINEX] = { 0 };//������Ŷ�ȡ�����ַ���
	char* lable = buff + 60;
	//gets��������ȡһ�У�����ȡ�����󷵻�NULLָ�룬��ʽ���£�
	//char * fgets ( char * str, int num, FILE * stream );
	while (fgets(buff, MAXRINEX, fp_nav))
	{
		//strstr:�����ַ����е�ָ���ַ����ַ�������ʽ���£�
		//const char * strstr ( const char * str1, const char * str2 );
		if (flag == 1)
		{
			row++;
			continue;
		}
		if (strstr(lable, "END OF HEADER"))
		{
			flag = 1;
		}
	}
	return row;
}
//GPS����
extern int getrow3(FILE* fp_nav)
{
	int row = 0;
	int flag = 0;
	char buff[MAXRINEX] = { 0 }; // ������Ŷ�ȡ�����ַ���
	char* lable = buff + 60; // �����ǩ��Ϣ�ӵ�61���ַ���ʼ

	while (fgets(buff, MAXRINEX, fp_nav))
	{
		if (flag == 1) // ����Ѿ��ҵ�"END OF HEADER"��ǩ
		{
			// �����һ���Ƿ���"G"��ͷ
			if (buff[0] == 'G')
			{
				row++; // ����ǣ������
			}
			continue;
		}

		// ����"END OF HEADER"��ǩ
		if (strstr(lable, "END OF HEADER"))
		{
			flag = 1; // �ҵ���ǩ�����ñ�־
		}
	}
	return row;
}
//���ַ���ת��Ϊ������,i��ʼλ�ã�n������ٸ��ַ�
static double strtonum(const char* buff, int i, int n)
{
	double value = 0.0;
	char str[256] = { 0 };
	char* p = str;
	/************************************
	* �������������������������0.0
	* 1.��ʼλ��<0
	* 2.��ȡ�ַ�������<i
	* 3.str�����ŵ��ֽ���<n
	*************************************/
	if (i < 0 || (int)strlen(buff) < i || (int)sizeof(str) - 1 < n)
	{
		return 0.0;
	}
	for (buff += i; *buff && --n >= 0; buff++)
	{
		//��Ŀ��������D��dΪ�ļ��п�ѧ���������֣�����ת���ɶ������ܶ�����e
		*p++ = ((*buff == 'D' || *buff == 'd') ? 'e' : *buff);
	}
	*p = '\0';
	//��Ŀ����������str�д�ŵ����Ը�ʽ����ȡ��value�С�
	return sscanf(str, "%lf", &value) == 1 ? value : 0.0;
}

//��ȡN�ļ�
extern void readrinex_n(FILE* fp_nav, pnav_head nav_h, pnav_body nav_b, int n_n,int n3)
{
	char buff[MAXRINEX] = { 0 };
	char* lable = buff + 60;
	int i = 0;
	int j = 0;

	// ���ȶ�ȡ�汾��
	while (fgets(buff, MAXRINEX, fp_nav))
	{
		if (strstr(lable, "RINEX VERSION / TYPE"))
		{
			nav_h->ver =(double)strtonum(buff, 0, 9);
			setstr((nav_h->type), buff + 20, 15);
			break; // �ҵ��汾�ź��˳�ѭ��
		}
	}

	// ���ݰ汾��ִ�в�ͬ�Ĵ����
	if (nav_h->ver < 2.99)
	{
		// ���¿�ʼ��ȡ�ļ�����Ϊ֮ǰ�Ѿ���ȡ��һ��
		rewind(fp_nav);
		while (fgets(buff, MAXRINEX, fp_nav))
		{
			if (strstr(lable, "RINEX VERSION / TYPE"))
			{
				nav_h->ver = strtonum(buff, 0, 9);
				setstr((nav_h->type), buff + 20, 15);
				continue;
			}
			else if (strstr(lable, "ION ALPHA"))
			{
				nav_h->ION_ALPHA[0] = strtonum(buff, 3, 12);
				nav_h->ION_ALPHA[1] = strtonum(buff, 3 + 12, 12);
				nav_h->ION_ALPHA[2] = strtonum(buff, 3 + 12 + 12, 12);
				nav_h->ION_ALPHA[3] = strtonum(buff, 3 + 12 + 12 + 12, 12);
				continue;
			}
			else if (strstr(lable, "ION BETA"))
			{
				nav_h->ION_BETA[0] = strtonum(buff, 3, 12);
				nav_h->ION_BETA[1] = strtonum(buff, 3 + 12, 12);
				nav_h->ION_BETA[2] = strtonum(buff, 3 + 12 + 12, 12);
				nav_h->ION_BETA[3] = strtonum(buff, 3 + 12 + 12 + 12, 12);
				continue;
			}
			else if (strstr(lable, "DELTA-UTC: A0,A1,T,W"))
			{
				nav_h->DELTA_UTC[0] = strtonum(buff, 3, 19);
				nav_h->DELTA_UTC[1] = strtonum(buff, 3 + 19, 19);
				nav_h->DELTA_UTC[2] = strtonum(buff, 3 + 19 + 19, 9);
				nav_h->DELTA_UTC[3] = strtonum(buff, 3 + 19 + 19 + 9, 9);
				continue;
			}
			else if (strstr(lable, "LEAP SECONDS"))
			{
				nav_h->leap = (int)strtonum(buff, 4, 2);
			}
			else if (strstr(lable, "END OF HEADER"))//��ʱ��ʼ�����ݽ��ж�ȡ
			{
				for (i = 0; i < (n_n / 8); i++)//n_nΪ������ͷ������������8Ϊ���ݿ�����
				{
					for (j = 0; j < 8; j++)
					{
						fgets(buff, MAXRINEX, fp_nav);
						switch (j)
						{
						case 0:
							nav_b[i].sPRN = (int)strtonum(buff, 0, 2);
							nav_b[i].TOC_Y = (int)strtonum(buff, 3, 2) + 2000;
							nav_b[i].TOC_M = (int)strtonum(buff, 6, 2);
							nav_b[i].TOC_D = (int)strtonum(buff, 9, 2);
							nav_b[i].TOC_H = (int)strtonum(buff, 12, 2);
							nav_b[i].TOC_Min = (int)strtonum(buff, 15, 2);
							nav_b[i].TOC_Sec = strtonum(buff, 18, 2);
							nav_b[i].sa0 = strtonum(buff, 22, 19);
							nav_b[i].sa1 = strtonum(buff, 22 + 19, 19);
							nav_b[i].sa2 = strtonum(buff, 22 + 19 + 19, 19);
							break;
						case 1:
							nav_b[i].IODE = strtonum(buff, 3, 19);
							nav_b[i].Crs = strtonum(buff, 3 + 19, 19);
							nav_b[i].deltan = strtonum(buff, 3 + 19 + 19, 19);
							nav_b[i].M0 = strtonum(buff, 3 + 19 + 19 + 19, 19);
							break;
						case 2:
							nav_b[i].Cuc = strtonum(buff, 3, 19);
							nav_b[i].e = strtonum(buff, 3 + 19, 19);
							nav_b[i].Cus = strtonum(buff, 3 + 19 + 19, 19);
							nav_b[i].sqrtA = strtonum(buff, 3 + 19 + 19 + 19, 19);
							break;
						case 3:
							nav_b[i].TOE = strtonum(buff, 3, 19);
							nav_b[i].Cic = strtonum(buff, 3 + 19, 19);
							nav_b[i].OMEGA = strtonum(buff, 3 + 19 + 19, 19);
							nav_b[i].Cis = strtonum(buff, 3 + 19 + 19 + 19, 19);
							break;
						case 4:
							nav_b[i].i0 = strtonum(buff, 3, 19);
							nav_b[i].Crc = strtonum(buff, 3 + 19, 19);
							nav_b[i].omega = strtonum(buff, 3 + 19 + 19, 19);
							nav_b[i].deltaomega = strtonum(buff, 3 + 19 + 19 + 19, 19);
							break;
						case 5:
							nav_b[i].IDOT = strtonum(buff, 3, 19);
							nav_b[i].L2code = strtonum(buff, 3 + 19, 19);
							nav_b[i].GPSweek = strtonum(buff, 3 + 19 + 19, 19);
							nav_b[i].L2Pflag = strtonum(buff, 3 + 19 + 19 + 19, 19);
							break;
						case 6:
							nav_b[i].sACC = strtonum(buff, 3, 19);
							nav_b[i].sHEA = strtonum(buff, 3 + 19, 19);
							nav_b[i].TGD = strtonum(buff, 3 + 19 + 19, 19);
							nav_b[i].IODC = strtonum(buff, 3 + 19 + 19 + 19, 19);
							break;
						case 7:
							nav_b[i].TTN = strtonum(buff, 3, 19);
							nav_b[i].fit = strtonum(buff, 3 + 19, 19);
							nav_b[i].spare1 = strtonum(buff, 3 + 19 + 19, 19);
							nav_b[i].spare2 = strtonum(buff, 3 + 19 + 19 + 19, 19);
							break;
						}
					}
				}
			}
		}
	}
	//rinex3�汾�Ĺ㲥�����ļ���ȡ
	else
	{	//����Ҫ�޸ĳ�rinex3���ϰ汾��
		rewind(fp_nav);
		while (fgets(buff, MAXRINEX, fp_nav))
		{
			if (strstr(lable, "RINEX VERSION / TYPE"))
			{
				nav_h->ver = strtonum(buff, 0, 9);//�汾��
				setstr((nav_h->type), buff + 20, 16);//�ļ�����
				continue;
			}
			else if (strstr(buff, "GPSA"))
			{
				nav_h->ION_ALPHA[0] = strtonum(buff, 6, 12);
				nav_h->ION_ALPHA[1] = strtonum(buff, 18, 12);
				nav_h->ION_ALPHA[2] = strtonum(buff, 30, 12);
				nav_h->ION_ALPHA[3] = strtonum(buff, 42, 12);
				continue;
			}
			else if (strstr(buff, "GPSB"))
			{
				nav_h->ION_BETA[0] = strtonum(buff, 6, 12);
				nav_h->ION_BETA[1] = strtonum(buff, 18, 12);
				nav_h->ION_BETA[2] = strtonum(buff, 30, 12);
				nav_h->ION_BETA[3] = strtonum(buff, 42, 12);
				continue;
			}
			else if (strstr(lable, "LEAP SECONDS"))
			{
				nav_h->leap = (int)strtonum(buff, 4, 2);
			}
			else if (strstr(lable, "END OF HEADER"))//��ʱ��ʼ�����ݽ��ж�ȡ
			{
				for (i = 0; i < (n3); i++)//3.04�汾  
				{
					for (j = 0; j < 8; j++)
					{
						fgets(buff, MAXRINEX, fp_nav);
						switch (j)
						{
						case 0:
							nav_b[i].sPRN = (int)strtonum(buff, 1, 2);
							nav_b[i].TOC_Y = (int)strtonum(buff, 4, 4);
							nav_b[i].TOC_M = (int)strtonum(buff, 9, 2);
							nav_b[i].TOC_D = (int)strtonum(buff, 12, 2);
							nav_b[i].TOC_H = (int)strtonum(buff, 15, 2);
							nav_b[i].TOC_Min = (int)strtonum(buff, 18, 2);
							nav_b[i].TOC_Sec = strtonum(buff, 21, 2);
							nav_b[i].sa0 = strtonum(buff, 23, 19);
							nav_b[i].sa1 = strtonum(buff, 23 + 19, 19);
							nav_b[i].sa2 = strtonum(buff, 23 + 19 + 19, 19);
							break;
						case 1:
							nav_b[i].IODE = strtonum(buff, 4, 19);
							nav_b[i].Crs = strtonum(buff, 4 + 19, 19);
							nav_b[i].deltan = strtonum(buff, 4 + 19 + 19, 19);
							nav_b[i].M0 = strtonum(buff, 4 + 19 + 19 + 19, 19);
							break;
						case 2:
							nav_b[i].Cuc = strtonum(buff, 4, 19);
							nav_b[i].e = strtonum(buff, 4 + 19, 19);
							nav_b[i].Cus = strtonum(buff, 4 + 19 + 19, 19);
							nav_b[i].sqrtA = strtonum(buff, 4 + 19 + 19 + 19, 19);
							break;
						case 3:
							nav_b[i].TOE = strtonum(buff, 4, 19);
							nav_b[i].Cic = strtonum(buff, 4 + 19, 19);
							nav_b[i].OMEGA = strtonum(buff, 4 + 19 + 19, 19);
							nav_b[i].Cis = strtonum(buff, 4 + 19 + 19 + 19, 19);
							break;
						case 4:
							nav_b[i].i0 = strtonum(buff, 4, 19);
							nav_b[i].Crc = strtonum(buff, 4 + 19, 19);
							nav_b[i].omega = strtonum(buff, 4 + 19 + 19, 19);
							nav_b[i].deltaomega = strtonum(buff, 4 + 19 + 19 + 19, 19);
							break;
						case 5:
							nav_b[i].IDOT = strtonum(buff, 4, 19);
							nav_b[i].L2code = strtonum(buff, 4 + 19, 19);
							nav_b[i].GPSweek = strtonum(buff, 4 + 19 + 19, 19);
							nav_b[i].L2Pflag = strtonum(buff, 4 + 19 + 19 + 19, 19);
							break;
						case 6:
							nav_b[i].sACC = strtonum(buff, 4, 19);
							nav_b[i].sHEA = strtonum(buff, 4 + 19, 19);
							nav_b[i].TGD = strtonum(buff, 4 + 19 + 19, 19);
							nav_b[i].IODC = strtonum(buff, 4 + 19 + 19 + 19, 19);
							break;
						case 7:
							nav_b[i].TTN = strtonum(buff, 4, 19);
							nav_b[i].fit = strtonum(buff, 4 + 19, 19);
							nav_b[i].spare1 = strtonum(buff, 4 + 19 + 19, 19);
							nav_b[i].spare2 = strtonum(buff, 4 + 19 + 19 + 19, 19);
							break;
						}
					}
				}
			}
		}
	}
}
