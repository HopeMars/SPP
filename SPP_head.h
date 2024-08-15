#define _CRT_SECURE_NO_WARNINGS
#include <stdarg.h>
#include <string>
#include <stdio.h>

/*观测值结构体*/
//观测值头文件
typedef struct obs_head
{
	double ver;//RINEX文件版本号
	char type[30];//文件类型
	double apX;//测站近似位置XYZ（WGS84）
	double apY;
	double apZ;
	double ANTH;//天线高
	double ANTdeltaE;//天线中心对于测站标志在东方向上的偏移量
	double ANTdeltaN;//天线中心对于测站标志在北方向上的偏移量
	int obstypenum;//观测值类型数量
	char obstype[200];//观测值类型
	double interval;//观测值的历元间隔
	int f_y;//第一个观测记录的时刻
	int f_m;
	int f_d;
	int f_h;
	int f_min;
	double f_sec;
	char tsys[5];//时间系统
}obs_head, * pobs_head;

//观测值历元数据结构体
typedef struct obs_epoch
{
	//观测历元时刻
	int y;
	int m;
	int d;
	int h;
	int min;
	double sec;
	char p_flag;//历元标志
	int sat_num;//当前历元所观测到的卫星数量
	int satGPS_num;//当前历元所观测到的GPS卫星数量
	int sPRN[1000000];//当前历元所能观测到的卫星的PRN列表
}obs_epoch, * pobs_epoch;

//观测值数据结构体
typedef struct obs_body
{
	double obs[100][300];//观测值
}obs_body, * pobs_body;

extern int get_epochnum(FILE* fp_obs);
extern void read_h(FILE* fp_obs, pobs_head obs_h);
extern void trim_spaces(char* str);
extern void read_b(FILE* fp_obs, pobs_epoch obs_e, pobs_body obs_b, int type);


/*广播星历文件结构体*/
// 导航头文件结构体
typedef struct nav_head
{
	double ver;//rinex 版本号
	char type[20];//读取的数据类型
	double ION_ALPHA[4];//8个电离层参数
	double ION_BETA[4];
	double DELTA_UTC[4];
	int leap;
}nav_head, * pnav_head;

//导航数据结构体
typedef struct nav_body
{
	//数据块第一行内容：
	int sPRN;//卫星PRN号
	//历元：TOC中卫星钟的参考时刻
	int TOC_Y;//年
	int TOC_M;//月
	int TOC_D;//日
	int TOC_H;//时
	int TOC_Min;//分
	int TOC_Sec;//秒
	double sa0;//卫星钟差
	double sa1;//卫星钟偏
	double sa2;//卫星钟漂

	//数据块第二行内容：
	double IODE;//数据、星历发布时间(数据期龄)
	double Crs;//轨道半径的正弦调和改正项的振幅（单位：m）
	double deltan;//卫星平均运动速率与计算值之差(rad/s)
	double M0;//参考时间的平近点角(rad)

	//数据块第三行内容：
	double Cuc;//维度幅角的余弦调和改正项的振幅(rad)
	double e;//轨道偏心率
	double Cus;//轨道幅角的正弦调和改正项的振幅(rad)
	double sqrtA;//长半轴平方根

	//数据块第四行内容：
	double TOE;//星历的参考时刻(GPS周内秒)
	double Cic;//轨道倾角的余弦调和改正项的振幅(rad)
	double OMEGA;//参考时刻的升交点赤经
	double Cis;//维度倾角的正弦调和改正项的振幅(rad)

	//数据块第五行内容：
	double i0;//参考时间的轨道倾角(rad)
	double Crc;//轨道平径的余弦调和改正项的振幅(m)
	double omega;//近地点角距
	double deltaomega;//升交点赤经变化率(rad)

	//数据块第六行内容：
	double IDOT;//近地点角距(rad/s)
	double L2code;//L2上的码
	double GPSweek;//GPS周,于TOE一同表示
	double L2Pflag;//L2,p码数据标记

	//数据块第七行内容
	double sACC;//卫星精度
	double sHEA;//卫星健康状态
	double TGD;//sec
	double IODC;//钟的数据龄期

	//数据块第八行内容
	double TTN;//电文发送时间
	double fit;//拟合区间
	double spare1;//空
	double spare2;//空

}nav_body, * pnav_body;

extern int getrow(FILE* fp_nav);
extern int getrow3(FILE* fp_nav);
extern void readrinex_n(FILE* fp_nav, pnav_head nav_h, pnav_body nav_b, int n_n, int n3);
extern void setstr(char* des, const char* src, int n);


/*计算卫星位置结构体*/

//卫星位置传参
typedef struct Pos_t
{
	double X;
	double Y;
	double Z;
	double deltat;//改正前的信号传播时间
	double delta_t;//改正后的信号传播时间
}Pos_t;


#define C_V 299792458//光速（m）
#define GM 398600500000000//地心引力常数
#define math_e 2.718281828459 //e值
#define PI 3.141592653589793
#define Earth_e 7.2921151467e-5 //地球自转角速度
#define C1 0

double TimetoGPSsec(int year, int month, int day, int hour, int min, double sec);
extern int select_epoch(double SecofWeek, int sPRN, pnav_body nav_b, int n3);
extern Pos_t sat_pos(double deltan, double sqrtA, double TOE, double M0, double e,
	double omega, double OMEGA, double deltaomega, double Cuc, double Crc, double Cic,
	double Cus, double Crs, double Cis, double i0, double IDOT, double delta_t, double deltat, double X_R,
	double Y_R, double Z_R, double GPSsec);

// 定义一个结构体来存储每个历元下每个卫星的最终坐标值
struct SatPos {
	double X;
	double Y;
	double Z;
};


extern void readnavfile();
extern void readobsfile();

