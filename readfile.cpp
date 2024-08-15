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
	/*N文件操作*/
	//数据读取
	FILE* fp_nav = NULL;//导航星历文件指针
	pnav_head nav_h = NULL;
	pnav_body nav_b = NULL;
	//N文件读取
	fp_nav = fopen("D:\\Code\\C\\SPPV0.1\\brdm2630.23p", "r");//以只读的方式打开N文件  brdm2630.23p
	int n_n = getrow(fp_nav);//获取导航文件观测行数
	rewind(fp_nav);
	int n3 = getrow3(fp_nav);
	rewind(fp_nav);//将文件指针返回值起始位置
	nav_h = (pnav_head)malloc(sizeof(nav_head));//给N文件头开辟空间
	nav_b = (pnav_body)malloc(sizeof(nav_body) * (n3));
	if (nav_h && nav_b)
	{
		readrinex_n(fp_nav, nav_h, nav_b, n_n, n3);//读取数据
	}
	fclose(fp_nav);//关闭N文件
}

extern void readobsfile()
{
	/*O文件*/
	FILE* fp_obs = NULL;//观测值文件指针
	//O文件读取
	fp_obs = fopen("D:\\Code\\C\\SPPV0.1\\abmf2630.23o", "r");//以只读的方式打开O文件
	pobs_head obs_h = NULL;
	pobs_epoch obs_e = NULL;
	pobs_body obs_b = NULL;//创建结构体指针，并将其初始化为空
	//读取O文件信息
	int o_epochnum = get_epochnum(fp_obs);//获取O文件历元数
	obs_h = (pobs_head)malloc(sizeof(obs_head));//给O文件头开辟空间分配内存
	obs_e = (pobs_epoch)malloc(sizeof(obs_epoch) * o_epochnum);
	obs_b = (pobs_body)malloc(sizeof(obs_body) * o_epochnum);
	rewind(fp_obs);
	if (obs_h && obs_e && obs_b)
	{
		read_h(fp_obs, obs_h);//读取O文件头
		read_b(fp_obs, obs_e, obs_b, obs_h->obstypenum);//读取O文件数据块
	}
	fclose(fp_obs);//关闭O文件
}