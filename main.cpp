#define MAXRINEX 25600//һ������ַ���������80���ɣ�
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

	/*N�ļ�����*/
	//���ݶ�ȡ
	FILE* fp_nav = NULL;//���������ļ�ָ��
	pnav_head nav_h = NULL;
	pnav_body nav_b = NULL;
	//N�ļ���ȡ
	fp_nav = fopen("D:\\Code\\C\\SPP\\brdm2630.23p", "r");//��ֻ���ķ�ʽ��N�ļ�  brdm2630.23p
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



	/*��O�ļ�*/
	FILE* fp_obs = NULL;//�۲�ֵ�ļ�ָ��
	//O�ļ���ȡ
	fp_obs = fopen("D:\\Code\\C\\SPP\\abmf2630.23o", "r");//��ֻ���ķ�ʽ��O�ļ�
	pobs_head obs_h = NULL;
	pobs_epoch obs_e = NULL;
	pobs_body obs_b = NULL;//�����ṹ��ָ�룬�������ʼ��Ϊ��
	//��ȡO�ļ���Ϣ
	int o_epochnum = get_epochnum(fp_obs);//��ȡO�ļ���Ԫ��
	obs_h = (pobs_head)malloc(sizeof(obs_head));//��O�ļ�ͷ���ٿռ�����ڴ�
	obs_e = (pobs_epoch)malloc(sizeof(obs_epoch) * o_epochnum);
	obs_b = (pobs_body)malloc(sizeof(obs_body) * o_epochnum);
	rewind(fp_obs);
	read_h(fp_obs, obs_h);
	read_b(fp_obs, obs_e, obs_b, obs_h->obstypenum);//��ȡO�ļ����ݿ�
	fclose(fp_obs);

	/*��������λ�ò����*/
	FILE* fp_satpos_out = fopen("satellite_positions1.txt", "w");

	//��������λ�úͽ��ջ�λ��
	double** X_S;
	double** Y_S;
	double** Z_S;
	double** b0;
	double** b1;
	double** b2;
	double** b3;
	double** L_Matrix;//�۲ⷽ���еľ���L
	double* X_R = (double*)malloc((sizeof(double)) * o_epochnum);//����������ͽ��ջ��Ӳ�
	double X_sim;//�ж�������������
	double* Y_R = (double*)malloc((sizeof(double)) * o_epochnum);
	double Y_sim;
	double* Z_R = (double*)malloc((sizeof(double)) * o_epochnum);
	double Z_sim;
	double* deltaTR = (double*)malloc((sizeof(double)) * o_epochnum);
	double deltaTR_sim;
	double H, A, L;//�������ʱ���ò���


	// �����ڴ�
	X_R = (double*)malloc((sizeof(double)) * o_epochnum);
	Y_R = (double*)malloc((sizeof(double)) * o_epochnum);
	Z_R = (double*)malloc((sizeof(double)) * o_epochnum);
	deltaTR = (double*)malloc((sizeof(double)) * o_epochnum);

	X_S = (double**)malloc(sizeof(double*) * o_epochnum);//��X_S�����ڴ�
	for (int i = 0; i < o_epochnum; i++)
	{
		X_S[i] = (double*)malloc((sizeof(double)) * obs_e[i].satGPS_num);
	}

	Y_S = (double**)malloc(sizeof(double*) * o_epochnum);//��Y_S�����ڴ�
	for (int i = 0; i < o_epochnum; i++)
	{
		Y_S[i] = (double*)malloc((sizeof(double)) * obs_e[i].satGPS_num);
	}

	Z_S = (double**)malloc(sizeof(double*) * o_epochnum);//��Z_S�����ڴ�
	for (int i = 0; i < o_epochnum; i++)
	{
		Z_S[i] = (double*)malloc((sizeof(double)) * obs_e[i].satGPS_num);
	}

	b0 = (double**)malloc(sizeof(double*) * o_epochnum);//��b0�����ڴ�
	for (int i = 0; i < o_epochnum; i++)
	{
		b0[i] = (double*)malloc((sizeof(double)) * obs_e[i].satGPS_num);
	}

	b1 = (double**)malloc(sizeof(double*) * o_epochnum);//��b1�����ڴ�
	for (int i = 0; i < o_epochnum; i++)
	{
		b1[i] = (double*)malloc((sizeof(double)) * obs_e[i].satGPS_num);
	}

	b2 = (double**)malloc(sizeof(double*) * o_epochnum);//��b2�����ڴ�
	for (int i = 0; i < o_epochnum; i++)
	{
		b2[i] = (double*)malloc((sizeof(double)) * obs_e[i].satGPS_num);
	}

	b3 = (double**)malloc(sizeof(double*) * o_epochnum);//��b3�����ڴ�
	for (int i = 0; i < o_epochnum; i++)
	{
		b3[i] = (double*)malloc((sizeof(double)) * obs_e[i].satGPS_num);
	}

	L_Matrix = (double**)malloc(sizeof(double*) * o_epochnum);//��L��������ڴ�
	for (int i = 0; i < o_epochnum; i++)
	{
		L_Matrix[i] = (double*)malloc((sizeof(double)) * obs_e[i].satGPS_num);
	}


	for (int i = 0; i < o_epochnum; i++)//����������ĳ�ʼ����ͳ�ʼ���ջ��Ӳ����Ϊ0
	{
		X_R[i] = 0;
		Y_R[i] = 0;
		Z_R[i] = 0;
		deltaTR[i] = 0;
	}

	// ����ÿ����Ԫ��ÿ��GPS���ǵ�λ��
	for (int i = 0; i < o_epochnum; ++i) {

		for (int j = 0; j < obs_e[i].satGPS_num; ++j) {
			// ʱ��ת��
			double GPSsec = TimetoGPSsec(obs_e[i].y, obs_e[i].m, obs_e[i].d, obs_e[i].h, obs_e[i].min, obs_e[i].sec);

			// ����O�ļ���Ԫ�ο�ʱ��ѡ����ʵ�N�ļ�����
			int n_epoch = select_epoch(GPSsec, obs_e[i].sPRN[j], nav_b, n3);

			// �۲�ʱ�� - �ο�ʱ��
			double detat_toc = GPSsec - nav_b[n_epoch].TOE;

			// ������Ƶ��źŴ���ʱ��,���ջ��Ӳ��ѳ�ʼ��Ϊ0(α��/����-���ջ��Ӳ�+�����Ӳ�)
			double delta_t = (obs_b[i].obs[j][C1] / C_V) - 0 + nav_b[n_epoch].sa0 + nav_b[n_epoch].sa1 * detat_toc + nav_b[n_epoch].sa2 * pow(detat_toc, 2);

			// ��������λ��
			double deltat = 0.0; // �ж�����
			while (fabs(delta_t - deltat) > 0.0000033) {
				// ��ʱ�ṹ�����tmp����������
				Pos_t tmp = { 0 };
				// ��������λ�ú���
				tmp = sat_pos(nav_b[n_epoch].deltan, nav_b[n_epoch].sqrtA, nav_b[n_epoch].TOE,
					nav_b[n_epoch].M0, nav_b[n_epoch].e, nav_b[n_epoch].omega, nav_b[n_epoch].OMEGA,
					nav_b[n_epoch].deltaomega, nav_b[n_epoch].Cuc, nav_b[n_epoch].Crc, nav_b[n_epoch].Cic,
					nav_b[n_epoch].Cus, nav_b[n_epoch].Crs, nav_b[n_epoch].Cis, nav_b[n_epoch].i0, nav_b[n_epoch].IDOT,
					delta_t, deltat, obs_h->apX, obs_h->apY, obs_h->apZ, GPSsec);
				// ���Σ������źŴ���ʱ��
				deltat = tmp.deltat;
				delta_t = tmp.delta_t;
				// ��ֵ��ÿ����Ԫ�µ�ÿ�����ǵ���������ֵ
				X_S[i][j] = tmp.X;
				Y_S[i][j] = tmp.Y;
				Z_S[i][j] = tmp.Z;
			}
			//������ջ�λ��
			X_sim = 1.0;
			Y_sim = 1.0;
			Z_sim = 1.0;
			deltaTR_sim = 1.0;
			/*while (fabs(sqrt(pow(X_sim, 2) + pow(Y_sim, 2) + pow(Z_sim, 2)) - sqrt(pow(X_R[i], 2) + pow(Y_R[i], 2) + pow(Z_R[i], 2))) > 0.0000001 || (fabs(deltaTR_sim - deltaTR[i]) > 0.001))
			{




			}*/



			// �������λ��
			fprintf(fp_satpos_out, "Epoch %d, Satellite PRN %d: X = %.2f, Y = %.2f, Z = %.2f\n", i, obs_e[i].sPRN[j], X_S[i][j], Y_S[i][j], Z_S[i][j]);

		}
	}
	fclose(fp_satpos_out);
	return 0;
}