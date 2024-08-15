#define _CRT_SECURE_NO_WARNINGS
#include "SPP_head.h"
#include <stdarg.h>
#include <string>
#include <stdio.h>
#include <ctime>
#include <time.h>
#include <cmath>  
#include<math.h>


/*GPSʱת��*/
double TimetoGPSsec(int year, int month, int day, int hour, int min, double sec)
{
    // ����GPSʱ��Ļ�׼���ڣ�1980��1��6��
    int gpsEpochYear = 1980;
    int gpsEpochMonth = 1;
    int gpsEpochDay = 6;

    // ����һ���ṹ����ʾ�����ʱ��
    std::tm inputTime = {};
    inputTime.tm_year = year - 1900; // ��ݴ�1900�꿪ʼ
    inputTime.tm_mon = month - 1;    // �·ݴ�0��ʼ
    inputTime.tm_mday = day;
    inputTime.tm_hour = hour;
    inputTime.tm_min = min;
    inputTime.tm_sec = static_cast<int>(sec);

    // ������ʱ��ת��Ϊ����
    std::time_t inputTimeSec = std::mktime(&inputTime);

    // ����һ���ṹ����ʾGPSʱ���׼
    std::tm gpsEpochTime = {};
    gpsEpochTime.tm_year = gpsEpochYear - 1900;
    gpsEpochTime.tm_mon = gpsEpochMonth - 1;
    gpsEpochTime.tm_mday = gpsEpochDay;
    gpsEpochTime.tm_hour = 0;
    gpsEpochTime.tm_min = 0;
    gpsEpochTime.tm_sec = 0;

    // ��GPS��׼ʱ��ת��Ϊ����
    std::time_t gpsEpochTimeSec = std::mktime(&gpsEpochTime);

    // �����GPS��׼ʱ�䵽����ʱ���������
    double totalSec = difftime(inputTimeSec, gpsEpochTimeSec);

    // ����������GPSʱ���1980��1��6�������쿪ʼ��ÿ��604800�룩
    double gpsWeekSec = 604800;
    double SecofWeek = fmod(totalSec, gpsWeekSec);

    // �������С������
    SecofWeek += sec - static_cast<int>(sec);

    // �������Ϊ��ֵ����ʱ������GPS��׼ʱ�䣩������ת��Ϊ����
    if (SecofWeek < 0) {
        SecofWeek += gpsWeekSec;
    }

    return SecofWeek;
}

//ƥ���������
extern int select_epoch(double SecofWeek, int sPRN, pnav_body nav_b, int n3)
{
    int n_epoch = 0;
    double min = 3600;//������Сֵ��3600��
    double Min;
    int i;
    for (i = 0; i < n3; i++)
    {
        if (sPRN == nav_b[i].sPRN)
        {
            Min = fabs(SecofWeek - nav_b[i].TOE);
            if (Min <= min)
            {
                n_epoch = i;
                min = Min;
            }
        }
    }
    return n_epoch;
}





//��������λ��
extern Pos_t sat_pos(double deltan, double sqrtA, double TOE, double M0, double e,
	double omega, double OMEGA, double deltaomega, double Cuc, double Crc, double Cic,
	double Cus, double Crs, double Cis, double i0, double IDOT, double delta_t, double deltat, double X_R,
	double Y_R, double Z_R, double GPSsec)
{
	Pos_t pos_t = { 0 };
	//�����źŷ���ʱ��=O�ļ��۲�ʱ��-�źŴ���ʱ��
	double T_S = GPSsec - delta_t;
	//�������ǵ�ƽ�����ٶ�n
	double n0 = sqrt(GM) / pow(sqrtA, 3);
	double n = n0 + deltan;
	//����黮ʱ��tk
	double tk = T_S - TOE;
	if (tk > 302400)tk -= 604800;//�滮ʱ�����
	else if (tk < -302400)tk += 604800;
	//�������Ƿ���ʱ���ǵ�ƽ����M
	double M = M0 + n * tk;
	//��������ƫ�����
	double E = M, E0;
	do
	{
		E0 = E;
		E = M + e * sin(E);
	} while (fabs(E - E0) > 1.0e-5);
	//����������V
	double cosv = (cos(E) - e) / (1 - e * cos(E));//cosV
	double sinv = sqrt(1 - pow(e, 2)) * sin(E) / (1 - e * cos(E));//sinV
	double Vk = atan2(sinv, cosv);//ע��atan��atan2�Ĳ�ͬ
	//���������Ǿ�u
	double u = Vk + omega;
	//�����㶯������
	double Epsilon_u = Cuc * cos(2 * u) + Cus * sin(2 * u);
	double Epsilon_r = Crc * cos(2 * u) + Crs * sin(2 * u);
	double Epsilon_i = Cic * cos(2 * u) + Cis * sin(2 * u);
	//���������������Ǿࡢ����򾶡�������
	double uk = u + Epsilon_u;
	double rk = pow(sqrtA, 2) * (1 - e * cos(E)) + Epsilon_r;
	double ik = i0 + Epsilon_i + IDOT * tk;
	//������������������ֱ������ϵ������
	double xk = rk * cos(uk);
	double yk = rk * sin(uk);
	//���������㾭��
	double L = OMEGA + (deltaomega - Earth_e) * tk - Earth_e * TOE;
	//���������ڵع�ϵ�µ�ֱ������
	double X = xk * cos(L) - yk * cos(ik) * sin(L);
	double Y = xk * sin(L) + yk * cos(ik) * cos(L);
	double Z = yk * sin(ik);
	//ͨ�� Z �����ת�任���Ը�������е�����ת�������õ�������
	pos_t.X = cos(Earth_e * delta_t) * X + sin(Earth_e * delta_t) * Y;
	pos_t.Y = -sin(Earth_e * delta_t) * X + cos(Earth_e * delta_t) * Y;
	pos_t.Z = Z;
	//���¼����źŴ�����ʱ��
	double R = sqrt(pow(X - X_R, 2) + pow(Y - Y_R, 2) + pow(Z - Z_R, 2));
	pos_t.deltat = delta_t;
	pos_t.delta_t = R / C_V;
	return pos_t;
}



