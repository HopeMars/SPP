#define MAXRINEX 25600//һ������ַ���������80���ɣ�
#define _CRT_SECURE_NO_WARNINGS
#include "SPP_head.h"
#include <stdarg.h>
#include <string>
#include <stdio.h>

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

//��ȡO�ļ�����Ԫ��
extern int get_epochnum(FILE* fp_obs)
{
	int n = 0;//��¼��Ԫ��
	int satnum = 0;//��¼ÿ����Ԫ��������
	char flag;//������Ǳ�־����'>'
	char buff[MAXRINEX];//��Ŷ�ȡ���ַ���
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

// ȥ���ַ����еĶ���ո�
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

//��O�ļ�ͷ
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
				// ֻ��һ������
				char* start = buff + 6; // ����ǰ6���ַ�
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
						// �����������
						fprintf(stderr, "Buffer overflow in obs_h->obstype\n");
						continue;
					}
				}
			}
			else
			{
				// ����������
				char* start = buff + 6; // ����ǰ6���ַ�
				char* end = strstr(start, "SYS / # / OBS TYPES");
				if (end)
				{
					size_t len = end - start;
					if (len < sizeof(obs_h->obstype))
					{
						strncpy(obs_h->obstype, start, len);
						obs_h->obstype[len] = '\0';

						// ��ȡ�ڶ�������
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
									// �����������
									fprintf(stderr, "Buffer overflow in obs_h->obstype\n");
									continue;
								}
							}
						}
					}
					else
					{
						// �����������
						fprintf(stderr, "Buffer overflow in obs_h->obstype\n");
						continue;
					}
				}
			}
			// ȥ������ո�
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


//��ȡO�ļ����ݿ�
extern void read_b(FILE* fp_obs, pobs_epoch obs_e, pobs_body obs_b, int type)
{
	int n = 0; // ��Ԫ��
	int i = 0; // ��i��GPS����
	int j = 0; // ��i�����ǵĵ�j���۲�ֵ
	char buff[MAXRINEX] = { 0 };

	while (fgets(buff, MAXRINEX, fp_obs)) {
		// ����Ƿ�Ϊ����Ԫ�Ŀ�ʼ
		if (buff[0] == '>') {
			// ������Ԫ��Ϣ
			obs_e[n].y = (int)strtonum(buff, 2, 4);
			obs_e[n].m = (int)strtonum(buff, 7, 2);
			obs_e[n].d = (int)strtonum(buff, 10, 2);
			obs_e[n].h = (int)strtonum(buff, 13, 2);
			obs_e[n].min = (int)strtonum(buff, 16, 2);
			obs_e[n].sec = strtonum(buff, 19, 11);
			obs_e[n].sat_num = (int)strtonum(buff, 33, 2);
			obs_e[n].satGPS_num = 0; // ��ʼ��GPS��������

			// ��ȡÿ�����ǵĹ۲�����
			for (i = 0; i < obs_e[n].sat_num; ++i) {
				fgets(buff, MAXRINEX, fp_obs);

				// �����GPS���ǣ������� 'G' ��ͷ����
				if (buff[0] == 'G') {
					obs_e[n].sPRN[obs_e[n].satGPS_num] = (int)strtonum(buff, 1, 2);

					// ��ȡ�۲�ֵ
					for (j = 0; j < type; ++j) {
						double val = strtonum(buff, 3 + j * 16, 14);
						if (val != 0.0) { // ֻ�ڴ�����Ч����ʱ�洢
							obs_b[n].obs[obs_e[n].satGPS_num][j] = val;
						}
						else {
							obs_b[n].obs[obs_e[n].satGPS_num][j] = 0.0;
						}
					}
					obs_e[n].satGPS_num++;
				}
			}
			// ��Ԫ������ϣ��ƶ�����һ����Ԫ
			n++;
		}
	}
}
