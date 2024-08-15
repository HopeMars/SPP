#define _CRT_SECURE_NO_WARNINGS
#include "SPP_head.h"
#include <stdarg.h>
#include <string>
#include <stdio.h>
#include <ctime>
#include <time.h>
#include <cmath>  
#include<math.h>


/*GPS时转换*/
double TimetoGPSsec(int year, int month, int day, int hour, int min, double sec)
{
    // 定义GPS时间的基准日期：1980年1月6日
    int gpsEpochYear = 1980;
    int gpsEpochMonth = 1;
    int gpsEpochDay = 6;

    // 创建一个结构来表示输入的时间
    std::tm inputTime = {};
    inputTime.tm_year = year - 1900; // 年份从1900年开始
    inputTime.tm_mon = month - 1;    // 月份从0开始
    inputTime.tm_mday = day;
    inputTime.tm_hour = hour;
    inputTime.tm_min = min;
    inputTime.tm_sec = static_cast<int>(sec);

    // 将输入时间转换为秒数
    std::time_t inputTimeSec = std::mktime(&inputTime);

    // 创建一个结构来表示GPS时间基准
    std::tm gpsEpochTime = {};
    gpsEpochTime.tm_year = gpsEpochYear - 1900;
    gpsEpochTime.tm_mon = gpsEpochMonth - 1;
    gpsEpochTime.tm_mday = gpsEpochDay;
    gpsEpochTime.tm_hour = 0;
    gpsEpochTime.tm_min = 0;
    gpsEpochTime.tm_sec = 0;

    // 将GPS基准时间转换为秒数
    std::time_t gpsEpochTimeSec = std::mktime(&gpsEpochTime);

    // 计算从GPS基准时间到输入时间的总秒数
    double totalSec = difftime(inputTimeSec, gpsEpochTimeSec);

    // 计算周数（GPS时间从1980年1月6日星期天开始，每周604800秒）
    double gpsWeekSec = 604800;
    double SecofWeek = fmod(totalSec, gpsWeekSec);

    // 加上秒的小数部分
    SecofWeek += sec - static_cast<int>(sec);

    // 如果秒数为负值（即时间早于GPS基准时间），则将其转换为正数
    if (SecofWeek < 0) {
        SecofWeek += gpsWeekSec;
    }

    return SecofWeek;
}

//匹配最佳星历
extern int select_epoch(double SecofWeek, int sPRN, pnav_body nav_b, int n3)
{
    int n_epoch = 0;
    double min = 3600;//假设最小值是3600秒
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





//计算卫星位置
extern Pos_t sat_pos(double deltan, double sqrtA, double TOE, double M0, double e,
	double omega, double OMEGA, double deltaomega, double Cuc, double Crc, double Cic,
	double Cus, double Crs, double Cis, double i0, double IDOT, double delta_t, double deltat, double X_R,
	double Y_R, double Z_R, double GPSsec)
{
	Pos_t pos_t = { 0 };
	//计算信号发射时刻=O文件观测时刻-信号传播时间
	double T_S = GPSsec - delta_t;
	//计算卫星的平均角速度n
	double n0 = sqrt(GM) / pow(sqrtA, 3);
	double n = n0 + deltan;
	//计算归划时间tk
	double tk = T_S - TOE;
	if (tk > 302400)tk -= 604800;//规划时间改正
	else if (tk < -302400)tk += 604800;
	//计算卫星发射时卫星的平近角M
	double M = M0 + n * tk;
	//迭代计算偏近点角
	double E = M, E0;
	do
	{
		E0 = E;
		E = M + e * sin(E);
	} while (fabs(E - E0) > 1.0e-5);
	//计算真近点角V
	double cosv = (cos(E) - e) / (1 - e * cos(E));//cosV
	double sinv = sqrt(1 - pow(e, 2)) * sin(E) / (1 - e * cos(E));//sinV
	double Vk = atan2(sinv, cosv);//注意atan与atan2的不同
	//计算升交角距u
	double u = Vk + omega;
	//计算摄动改正项
	double Epsilon_u = Cuc * cos(2 * u) + Cus * sin(2 * u);
	double Epsilon_r = Crc * cos(2 * u) + Crs * sin(2 * u);
	double Epsilon_i = Cic * cos(2 * u) + Cis * sin(2 * u);
	//计算改正后的升交角距、轨道向径、轨道倾角
	double uk = u + Epsilon_u;
	double rk = pow(sqrtA, 2) * (1 - e * cos(E)) + Epsilon_r;
	double ik = i0 + Epsilon_i + IDOT * tk;
	//计算卫星在升交点轨道直角坐标系的坐标
	double xk = rk * cos(uk);
	double yk = rk * sin(uk);
	//计算升交点经度
	double L = OMEGA + (deltaomega - Earth_e) * tk - Earth_e * TOE;
	//计算卫星在地固系下的直角坐标
	double X = xk * cos(L) - yk * cos(ik) * sin(L);
	double Y = xk * sin(L) + yk * cos(ik) * cos(L);
	double Z = yk * sin(ik);
	//通过 Z 轴的旋转变换，对该坐标进行地球自转改正，得到新坐标
	pos_t.X = cos(Earth_e * delta_t) * X + sin(Earth_e * delta_t) * Y;
	pos_t.Y = -sin(Earth_e * delta_t) * X + cos(Earth_e * delta_t) * Y;
	pos_t.Z = Z;
	//重新计算信号传播的时间
	double R = sqrt(pow(X - X_R, 2) + pow(Y - Y_R, 2) + pow(Z - Z_R, 2));
	pos_t.deltat = delta_t;
	pos_t.delta_t = R / C_V;
	return pos_t;
}



