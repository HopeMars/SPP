#define _CRT_SECURE_NO_WARNINGS
#include <stdarg.h>
#include <string>
#include <stdio.h>
#include "SPP_head.h"
#include <stdlib.h>
#include <math.h>
#include <time.h>

extern void readnavfile()
{
	/*N�ļ�����*/
	//���ݶ�ȡ
	FILE* fp_nav = NULL;//���������ļ�ָ��
	pnav_head nav_h = NULL;
	pnav_body nav_b = NULL;
	//N�ļ���ȡ
	fp_nav = fopen("D:\\Code\\C\\SPPV0.1\\brdm2630.23p", "r");//��ֻ���ķ�ʽ��N�ļ�  brdm2630.23p
	int n_n = getrow(fp_nav);//��ȡ�����ļ��۲�����
	rewind(fp_nav);
	int n3 = getrow3(fp_nav);
	rewind(fp_nav);//���ļ�ָ�뷵��ֵ��ʼλ��
	nav_h = (pnav_head)malloc(sizeof(nav_head));//��N�ļ�ͷ���ٿռ�
	nav_b = (pnav_body)malloc(sizeof(nav_body) * (n3));
	if (nav_h && nav_b)
	{
		readrinex_n(fp_nav, nav_h, nav_b, n_n, n3);//��ȡ����
	}
	fclose(fp_nav);//�ر�N�ļ�
}

extern void readobsfile()
{
	/*O�ļ�*/
	FILE* fp_obs = NULL;//�۲�ֵ�ļ�ָ��
	//O�ļ���ȡ
	fp_obs = fopen("D:\\Code\\C\\SPPV0.1\\abmf2630.23o", "r");//��ֻ���ķ�ʽ��O�ļ�
	pobs_head obs_h = NULL;
	pobs_epoch obs_e = NULL;
	pobs_body obs_b = NULL;//�����ṹ��ָ�룬�������ʼ��Ϊ��
	//��ȡO�ļ���Ϣ
	int o_epochnum = get_epochnum(fp_obs);//��ȡO�ļ���Ԫ��
	obs_h = (pobs_head)malloc(sizeof(obs_head));//��O�ļ�ͷ���ٿռ�����ڴ�
	obs_e = (pobs_epoch)malloc(sizeof(obs_epoch) * o_epochnum);
	obs_b = (pobs_body)malloc(sizeof(obs_body) * o_epochnum);
	rewind(fp_obs);
	if (obs_h && obs_e && obs_b)
	{
		read_h(fp_obs, obs_h);//��ȡO�ļ�ͷ
		read_b(fp_obs, obs_e, obs_b, obs_h->obstypenum);//��ȡO�ļ����ݿ�
	}
	fclose(fp_obs);//�ر�O�ļ�
}