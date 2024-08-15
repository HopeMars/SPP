#define MAXRINEX 25600//一行最大字符数（大于80均可）
#define _CRT_SECURE_NO_WARNINGS
#include <stdarg.h>
#include <string>
#include <stdio.h>
#include "SPP_head.h"
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "Matrix.h"


int main()
{

	/*N文件操作*/
	//数据读取
	FILE* fp_nav = NULL;//导航星历文件指针
	pnav_head nav_h = NULL;
	pnav_body nav_b = NULL;
	//N文件读取
	fp_nav = fopen("D:\\Code\\C\\SPP\\brdm2630.23p", "r");//以只读的方式打开N文件  brdm2630.23p
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



	/*读O文件*/
	FILE* fp_obs = NULL;//观测值文件指针
	//O文件读取
	fp_obs = fopen("D:\\Code\\C\\SPP\\abmf2630.23o", "r");//以只读的方式打开O文件
	pobs_head obs_h = NULL;
	pobs_epoch obs_e = NULL;
	pobs_body obs_b = NULL;//创建结构体指针，并将其初始化为空
	//读取O文件信息
	int o_epochnum = get_epochnum(fp_obs);//获取O文件历元数
	obs_h = (pobs_head)malloc(sizeof(obs_head));//给O文件头开辟空间分配内存
	obs_e = (pobs_epoch)malloc(sizeof(obs_epoch) * o_epochnum);
	obs_b = (pobs_body)malloc(sizeof(obs_body) * o_epochnum);
	rewind(fp_obs);
	read_h(fp_obs, obs_h);
	read_b(fp_obs, obs_e, obs_b, obs_h->obstypenum);//读取O文件数据块
	fclose(fp_obs);

	/*计算卫星位置并输出*/
	FILE* fp_satpos_out = fopen("satellite_positions1.txt", "w");

	//计算卫星位置和接收机位置
	double** X_S;
	double** Y_S;
	double** Z_S;
	double** b0;
	double** b1;
	double** b2;
	double** b3;
	double** L_Matrix;//观测方程中的矩阵L
	double* X_R = (double*)malloc((sizeof(double)) * o_epochnum);//待定点坐标和接收机钟差
	double X_sim;//判断收敛条件所用
	double* Y_R = (double*)malloc((sizeof(double)) * o_epochnum);
	double Y_sim;
	double* Z_R = (double*)malloc((sizeof(double)) * o_epochnum);
	double Z_sim;
	double* deltaTR = (double*)malloc((sizeof(double)) * o_epochnum);
	double deltaTR_sim;
	double H, A, L;//电离层延时所用参数


	// 分配内存
	X_R = (double*)malloc((sizeof(double)) * o_epochnum);
	Y_R = (double*)malloc((sizeof(double)) * o_epochnum);
	Z_R = (double*)malloc((sizeof(double)) * o_epochnum);
	deltaTR = (double*)malloc((sizeof(double)) * o_epochnum);

	X_S = (double**)malloc(sizeof(double*) * o_epochnum);//给X_S分配内存
	for (int i = 0; i < o_epochnum; i++)
	{
		X_S[i] = (double*)malloc((sizeof(double)) * obs_e[i].satGPS_num);
	}

	Y_S = (double**)malloc(sizeof(double*) * o_epochnum);//给Y_S分配内存
	for (int i = 0; i < o_epochnum; i++)
	{
		Y_S[i] = (double*)malloc((sizeof(double)) * obs_e[i].satGPS_num);
	}

	Z_S = (double**)malloc(sizeof(double*) * o_epochnum);//给Z_S分配内存
	for (int i = 0; i < o_epochnum; i++)
	{
		Z_S[i] = (double*)malloc((sizeof(double)) * obs_e[i].satGPS_num);
	}

	b0 = (double**)malloc(sizeof(double*) * o_epochnum);//给b0分配内存
	for (int i = 0; i < o_epochnum; i++)
	{
		b0[i] = (double*)malloc((sizeof(double)) * obs_e[i].satGPS_num);
	}

	b1 = (double**)malloc(sizeof(double*) * o_epochnum);//给b1分配内存
	for (int i = 0; i < o_epochnum; i++)
	{
		b1[i] = (double*)malloc((sizeof(double)) * obs_e[i].satGPS_num);
	}

	b2 = (double**)malloc(sizeof(double*) * o_epochnum);//给b2分配内存
	for (int i = 0; i < o_epochnum; i++)
	{
		b2[i] = (double*)malloc((sizeof(double)) * obs_e[i].satGPS_num);
	}

	b3 = (double**)malloc(sizeof(double*) * o_epochnum);//给b3分配内存
	for (int i = 0; i < o_epochnum; i++)
	{
		b3[i] = (double*)malloc((sizeof(double)) * obs_e[i].satGPS_num);
	}

	L_Matrix = (double**)malloc(sizeof(double*) * o_epochnum);//给L矩阵分配内存
	for (int i = 0; i < o_epochnum; i++)
	{
		L_Matrix[i] = (double*)malloc((sizeof(double)) * obs_e[i].satGPS_num);
	}


	for (int i = 0; i < o_epochnum; i++)//给定待定点的初始坐标和初始接收机钟差，都设为0
	{
		X_R[i] = 0;
		Y_R[i] = 0;
		Z_R[i] = 0;
		deltaTR[i] = 0;
	}

	// 计算每个历元下每个GPS卫星的位置
	for (int i = 0; i < o_epochnum; ++i) {

		for (int j = 0; j < obs_e[i].satGPS_num; ++j) {
			// 时间转换
			double GPSsec = TimetoGPSsec(obs_e[i].y, obs_e[i].m, obs_e[i].d, obs_e[i].h, obs_e[i].min, obs_e[i].sec);

			// 根据O文件历元参考时间选择合适的N文件数据
			int n_epoch = select_epoch(GPSsec, obs_e[i].sPRN[j], nav_b, n3);

			// 观测时刻 - 参考时刻
			double detat_toc = GPSsec - nav_b[n_epoch].TOE;

			// 计算近似的信号传播时间,接收机钟差已初始化为0(伪距/光速-接收机钟差+卫星钟差)
			double delta_t = (obs_b[i].obs[j][C1] / C_V) - 0 + nav_b[n_epoch].sa0 + nav_b[n_epoch].sa1 * detat_toc + nav_b[n_epoch].sa2 * pow(detat_toc, 2);

			// 计算卫星位置
			double deltat = 0.0; // 判断收敛
			while (fabs(delta_t - deltat) > 0.0000033) {
				// 临时结构体变量tmp，用来传参
				Pos_t tmp = { 0 };
				// 计算卫星位置函数
				tmp = sat_pos(nav_b[n_epoch].deltan, nav_b[n_epoch].sqrtA, nav_b[n_epoch].TOE,
					nav_b[n_epoch].M0, nav_b[n_epoch].e, nav_b[n_epoch].omega, nav_b[n_epoch].OMEGA,
					nav_b[n_epoch].deltaomega, nav_b[n_epoch].Cuc, nav_b[n_epoch].Crc, nav_b[n_epoch].Cic,
					nav_b[n_epoch].Cus, nav_b[n_epoch].Crs, nav_b[n_epoch].Cis, nav_b[n_epoch].i0, nav_b[n_epoch].IDOT,
					delta_t, deltat, obs_h->apX, obs_h->apY, obs_h->apZ, GPSsec);
				// 传参，更新信号传播时间
				deltat = tmp.deltat;
				delta_t = tmp.delta_t;
				// 赋值给每个历元下的每个卫星的最终坐标值
				X_S[i][j] = tmp.X;
				Y_S[i][j] = tmp.Y;
				Z_S[i][j] = tmp.Z;
			}
			//计算接收机位置
			X_sim = 1.0;
			Y_sim = 1.0;
			Z_sim = 1.0;
			deltaTR_sim = 1.0;
			/*while (fabs(sqrt(pow(X_sim, 2) + pow(Y_sim, 2) + pow(Z_sim, 2)) - sqrt(pow(X_R[i], 2) + pow(Y_R[i], 2) + pow(Z_R[i], 2))) > 0.0000001 || (fabs(deltaTR_sim - deltaTR[i]) > 0.001))
			{




			}*/



			// 输出卫星位置
			fprintf(fp_satpos_out, "Epoch %d, Satellite PRN %d: X = %.2f, Y = %.2f, Z = %.2f\n", i, obs_e[i].sPRN[j], X_S[i][j], Y_S[i][j], Z_S[i][j]);

		}
	}
	fclose(fp_satpos_out);
	return 0;
}