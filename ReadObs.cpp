#define MAXRINEX 25600//一行最大字符数（大于80均可）
#define _CRT_SECURE_NO_WARNINGS
#include "SPP_head.h"
#include <stdarg.h>
#include <string>
#include <stdio.h>

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

//获取O文件中历元数
extern int get_epochnum(FILE* fp_obs)
{
	int n = 0;//记录历元数
	int satnum = 0;//记录每个历元的卫星数
	char flag;//存放卫星标志符号'>'
	char buff[MAXRINEX];//存放读取的字符串
	while (fgets(buff, MAXRINEX, fp_obs))
	{
		satnum = (int)strtonum(buff, 33, 2);
		strncpy(&flag, buff, 1);
		if (flag == '>')
		{
			n++;
		}
	}
	return n;
}

// 去除字符串中的多余空格
extern void trim_spaces(char* str) {
	char* src = str, * dst = str;
	int space = 0;

	while (*src) {
		if (isspace(*src)) {
			space = 1;
		}
		else {
			if (space) {
				*dst++ = ' ';
				space = 0;
			}
			*dst++ = *src;
		}
		src++;
	}
	*dst = '\0';
}

//读O文件头
extern void read_h(FILE* fp_obs, pobs_head obs_h)
{
	char buff[MAXRINEX] = { 0 };
	char* lable = buff + 60;
	int i = 0;
	int j = 0;

	while (fgets(buff, MAXRINEX, fp_obs))
	{
		if (strstr(lable, "RINEX VERSION / TYPE"))
		{
			obs_h->ver = strtonum(buff, 0, 9);
			setstr(obs_h->type, buff + 20, 30);
			continue;
		}
		else if (strstr(lable, "APPROX POSITION XYZ"))
		{
			obs_h->apX = strtonum(buff, 0, 14);
			obs_h->apY = strtonum(buff, 0 + 14, 14);
			obs_h->apZ = strtonum(buff, 0 + 14 + 14, 14);
			continue;
		}
		else if (strstr(lable, "ANTENNA: DELTA H/E/N"))
		{
			obs_h->ANTH = strtonum(buff, 0, 14);
			obs_h->ANTdeltaE = strtonum(buff, 14, 14);
			obs_h->ANTdeltaN = strtonum(buff, 14 + 14, 14);
			continue;
		}
		else if (strstr(lable, "SYS / # / OBS TYPES") && strstr(buff, "G"))
		{
			obs_h->obstypenum = strtonum(buff, 1, 5);
			if (obs_h->obstypenum <= 13)
			{
				// 只有一行数据
				char* start = buff + 6; // 跳过前6个字符
				char* end = strstr(start, "SYS / # / OBS TYPES");
				if (end)
				{
					size_t len = end - start;
					if (len < sizeof(obs_h->obstype))
					{
						strncpy(obs_h->obstype, start, len);
						obs_h->obstype[len] = '\0';
					}
					else
					{
						// 处理缓冲区溢出
						fprintf(stderr, "Buffer overflow in obs_h->obstype\n");
						continue;
					}
				}
			}
			else
			{
				// 有两行数据
				char* start = buff + 6; // 跳过前6个字符
				char* end = strstr(start, "SYS / # / OBS TYPES");
				if (end)
				{
					size_t len = end - start;
					if (len < sizeof(obs_h->obstype))
					{
						strncpy(obs_h->obstype, start, len);
						obs_h->obstype[len] = '\0';

						// 读取第二行数据
						if (fgets(buff, MAXRINEX, fp_obs))
						{
							start = buff;
							end = strstr(start, "SYS / # / OBS TYPES");
							if (end)
							{
								len = end - start;
								if (strlen(obs_h->obstype) + len < sizeof(obs_h->obstype))
								{
									strncat(obs_h->obstype, start, len);
									obs_h->obstype[strlen(obs_h->obstype) + len] = '\0';
								}
								else
								{
									// 处理缓冲区溢出
									fprintf(stderr, "Buffer overflow in obs_h->obstype\n");
									continue;
								}
							}
						}
					}
					else
					{
						// 处理缓冲区溢出
						fprintf(stderr, "Buffer overflow in obs_h->obstype\n");
						continue;
					}
				}
			}
			// 去除多余空格
			trim_spaces(obs_h->obstype);
			continue;
		}
		else if (strstr(lable, "INTERVAL"))
		{
			obs_h->interval = strtonum(buff, 0, 10);
			continue;
		}
		else if (strstr(lable, "TIME OF FIRST OBS"))
		{
			obs_h->f_y = (int)strtonum(buff, 0, 6);
			obs_h->f_m = (int)strtonum(buff, 6, 6);
			obs_h->f_d = (int)strtonum(buff, 6 + 6, 6);
			obs_h->f_h = (int)strtonum(buff, 6 + 6 + 6, 6);
			obs_h->f_min = (int)strtonum(buff, 6 + 6 + 6 + 6, 6);
			obs_h->f_sec = strtonum(buff, 6 + 6 + 6 + 6 + 6, 6);
			setstr(obs_h->tsys, buff + 6 + 6 + 6 + 6 + 6 + 18, 3);
			continue;
		}
		else if (strstr(lable, "END OF HEADER"))
			break;

	}
}


//读取O文件数据块
extern void read_b(FILE* fp_obs, pobs_epoch obs_e, pobs_body obs_b, int type)
{
	int n = 0; // 历元数
	int i = 0; // 第i颗GPS卫星
	int j = 0; // 第i颗卫星的第j个观测值
	char buff[MAXRINEX] = { 0 };

	while (fgets(buff, MAXRINEX, fp_obs)) {
		// 检查是否为新历元的开始
		if (buff[0] == '>') {
			// 解析历元信息
			obs_e[n].y = (int)strtonum(buff, 2, 4);
			obs_e[n].m = (int)strtonum(buff, 7, 2);
			obs_e[n].d = (int)strtonum(buff, 10, 2);
			obs_e[n].h = (int)strtonum(buff, 13, 2);
			obs_e[n].min = (int)strtonum(buff, 16, 2);
			obs_e[n].sec = strtonum(buff, 19, 11);
			obs_e[n].sat_num = (int)strtonum(buff, 33, 2);
			obs_e[n].satGPS_num = 0; // 初始化GPS卫星数量

			// 读取每个卫星的观测数据
			for (i = 0; i < obs_e[n].sat_num; ++i) {
				fgets(buff, MAXRINEX, fp_obs);

				// 如果是GPS卫星，处理以 'G' 开头的行
				if (buff[0] == 'G') {
					obs_e[n].sPRN[obs_e[n].satGPS_num] = (int)strtonum(buff, 1, 2);

					// 读取观测值
					for (j = 0; j < type; ++j) {
						double val = strtonum(buff, 3 + j * 16, 14);
						if (val != 0.0) { // 只在存在有效数据时存储
							obs_b[n].obs[obs_e[n].satGPS_num][j] = val;
						}
						else {
							obs_b[n].obs[obs_e[n].satGPS_num][j] = 0.0;
						}
					}
					obs_e[n].satGPS_num++;
				}
			}
			// 历元处理完毕，移动到下一个历元
			n++;
		}
	}
}
