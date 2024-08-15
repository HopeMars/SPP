#define MAXRINEX 25600//一行最大字符数（大于80均可）
#define _CRT_SECURE_NO_WARNINGS
#include "SPP_head.h"
#include <stdarg.h>
#include <string>
#include <stdio.h>

//从源字符串src复制最多n个字符到目标字符串des，并确保目标字符串末尾没有空格
extern void setstr(char* des, const char* src, int n)
{
	char* p = des;
	const char* q = src;
	while (*q && q < src + n)
	{
		*p++ = *q++;
	}
	*p-- = '\0';
	//去掉尾部空格
	while (p >= des && *p == ' ')
	{
		*p-- = '\0';
	}
}
//获取文件数据块行数，从END OF HEADER后开始起算
extern int getrow(FILE* fp_nav)
{
	int row = 0;
	int flag = 0;
	char buff[MAXRINEX] = { 0 };//用来存放读取到的字符串
	char* lable = buff + 60;
	//gets函数，读取一行，当读取结束后返回NULL指针，格式如下：
	//char * fgets ( char * str, int num, FILE * stream );
	while (fgets(buff, MAXRINEX, fp_nav))
	{
		//strstr:查找字符串中的指定字符或字符串，格式如下：
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
//GPS卫星
extern int getrow3(FILE* fp_nav)
{
	int row = 0;
	int flag = 0;
	char buff[MAXRINEX] = { 0 }; // 用来存放读取到的字符串
	char* lable = buff + 60; // 假设标签信息从第61个字符开始

	while (fgets(buff, MAXRINEX, fp_nav))
	{
		if (flag == 1) // 如果已经找到"END OF HEADER"标签
		{
			// 检查这一行是否以"G"开头
			if (buff[0] == 'G')
			{
				row++; // 如果是，则计数
			}
			continue;
		}

		// 查找"END OF HEADER"标签
		if (strstr(lable, "END OF HEADER"))
		{
			flag = 1; // 找到标签后，设置标志
		}
	}
	return row;
}
//将字符串转换为浮点数,i起始位置，n输入多少个字符
static double strtonum(const char* buff, int i, int n)
{
	double value = 0.0;
	char str[256] = { 0 };
	char* p = str;
	/************************************
	* 当出现以下三种情况报错，返回0.0
	* 1.起始位置<0
	* 2.读取字符串个数<i
	* 3.str里面存放的字节数<n
	*************************************/
	if (i < 0 || (int)strlen(buff) < i || (int)sizeof(str) - 1 < n)
	{
		return 0.0;
	}
	for (buff += i; *buff && --n >= 0; buff++)
	{
		//三目操作符：D和d为文件中科学计数法部分，将其转换成二进制能读懂的e
		*p++ = ((*buff == 'D' || *buff == 'd') ? 'e' : *buff);
	}
	*p = '\0';
	//三目操作符，将str中存放的数以格式化读取到value中。
	return sscanf(str, "%lf", &value) == 1 ? value : 0.0;
}

//读取N文件
extern void readrinex_n(FILE* fp_nav, pnav_head nav_h, pnav_body nav_b, int n_n,int n3)
{
	char buff[MAXRINEX] = { 0 };
	char* lable = buff + 60;
	int i = 0;
	int j = 0;

	// 首先读取版本号
	while (fgets(buff, MAXRINEX, fp_nav))
	{
		if (strstr(lable, "RINEX VERSION / TYPE"))
		{
			nav_h->ver =(double)strtonum(buff, 0, 9);
			setstr((nav_h->type), buff + 20, 15);
			break; // 找到版本号后退出循环
		}
	}

	// 根据版本号执行不同的代码块
	if (nav_h->ver < 2.99)
	{
		// 重新开始读取文件，因为之前已经读取了一行
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
			else if (strstr(lable, "END OF HEADER"))//这时开始对数据进行读取
			{
				for (i = 0; i < (n_n / 8); i++)//n_n为不包含头的行数，除以8为数据块数量
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
	//rinex3版本的广播星历文件读取
	else
	{	//这里要修改成rinex3以上版本的
		rewind(fp_nav);
		while (fgets(buff, MAXRINEX, fp_nav))
		{
			if (strstr(lable, "RINEX VERSION / TYPE"))
			{
				nav_h->ver = strtonum(buff, 0, 9);//版本号
				setstr((nav_h->type), buff + 20, 16);//文件类型
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
			else if (strstr(lable, "END OF HEADER"))//这时开始对数据进行读取
			{
				for (i = 0; i < (n3); i++)//3.04版本  
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
