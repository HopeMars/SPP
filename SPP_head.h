#define _CRT_SECURE_NO_WARNINGS
#include <stdarg.h>
#include <string>
#include <stdio.h>

/*�۲�ֵ�ṹ��*/
//�۲�ֵͷ�ļ�
typedef struct obs_head
{
	double ver;//RINEX�ļ��汾��
	char type[30];//�ļ�����
	double apX;//��վ����λ��XYZ��WGS84��
	double apY;
	double apZ;
	double ANTH;//���߸�
	double ANTdeltaE;//�������Ķ��ڲ�վ��־�ڶ������ϵ�ƫ����
	double ANTdeltaN;//�������Ķ��ڲ�վ��־�ڱ������ϵ�ƫ����
	int obstypenum;//�۲�ֵ��������
	char obstype[200];//�۲�ֵ����
	double interval;//�۲�ֵ����Ԫ���
	int f_y;//��һ���۲��¼��ʱ��
	int f_m;
	int f_d;
	int f_h;
	int f_min;
	double f_sec;
	char tsys[5];//ʱ��ϵͳ
}obs_head, * pobs_head;

//�۲�ֵ��Ԫ���ݽṹ��
typedef struct obs_epoch
{
	//�۲���Ԫʱ��
	int y;
	int m;
	int d;
	int h;
	int min;
	double sec;
	char p_flag;//��Ԫ��־
	int sat_num;//��ǰ��Ԫ���۲⵽����������
	int satGPS_num;//��ǰ��Ԫ���۲⵽��GPS��������
	int sPRN[1000000];//��ǰ��Ԫ���ܹ۲⵽�����ǵ�PRN�б�
}obs_epoch, * pobs_epoch;

//�۲�ֵ���ݽṹ��
typedef struct obs_body
{
	double obs[100][300];//�۲�ֵ
}obs_body, * pobs_body;

extern int get_epochnum(FILE* fp_obs);
extern void read_h(FILE* fp_obs, pobs_head obs_h);
extern void trim_spaces(char* str);
extern void read_b(FILE* fp_obs, pobs_epoch obs_e, pobs_body obs_b, int type);


/*�㲥�����ļ��ṹ��*/
// ����ͷ�ļ��ṹ��
typedef struct nav_head
{
	double ver;//rinex �汾��
	char type[20];//��ȡ����������
	double ION_ALPHA[4];//8����������
	double ION_BETA[4];
	double DELTA_UTC[4];
	int leap;
}nav_head, * pnav_head;

//�������ݽṹ��
typedef struct nav_body
{
	//���ݿ��һ�����ݣ�
	int sPRN;//����PRN��
	//��Ԫ��TOC�������ӵĲο�ʱ��
	int TOC_Y;//��
	int TOC_M;//��
	int TOC_D;//��
	int TOC_H;//ʱ
	int TOC_Min;//��
	int TOC_Sec;//��
	double sa0;//�����Ӳ�
	double sa1;//������ƫ
	double sa2;//������Ư

	//���ݿ�ڶ������ݣ�
	double IODE;//���ݡ���������ʱ��(��������)
	double Crs;//����뾶�����ҵ��͸�������������λ��m��
	double deltan;//����ƽ���˶����������ֵ֮��(rad/s)
	double M0;//�ο�ʱ���ƽ�����(rad)

	//���ݿ���������ݣ�
	double Cuc;//ά�ȷ��ǵ����ҵ��͸���������(rad)
	double e;//���ƫ����
	double Cus;//������ǵ����ҵ��͸���������(rad)
	double sqrtA;//������ƽ����

	//���ݿ���������ݣ�
	double TOE;//�����Ĳο�ʱ��(GPS������)
	double Cic;//�����ǵ����ҵ��͸���������(rad)
	double OMEGA;//�ο�ʱ�̵�������ྭ
	double Cis;//ά����ǵ����ҵ��͸���������(rad)

	//���ݿ���������ݣ�
	double i0;//�ο�ʱ��Ĺ�����(rad)
	double Crc;//���ƽ�������ҵ��͸���������(m)
	double omega;//���ص�Ǿ�
	double deltaomega;//������ྭ�仯��(rad)

	//���ݿ���������ݣ�
	double IDOT;//���ص�Ǿ�(rad/s)
	double L2code;//L2�ϵ���
	double GPSweek;//GPS��,��TOEһͬ��ʾ
	double L2Pflag;//L2,p�����ݱ��

	//���ݿ����������
	double sACC;//���Ǿ���
	double sHEA;//���ǽ���״̬
	double TGD;//sec
	double IODC;//�ӵ���������

	//���ݿ�ڰ�������
	double TTN;//���ķ���ʱ��
	double fit;//�������
	double spare1;//��
	double spare2;//��

}nav_body, * pnav_body;

extern int getrow(FILE* fp_nav);
extern int getrow3(FILE* fp_nav);
extern void readrinex_n(FILE* fp_nav, pnav_head nav_h, pnav_body nav_b, int n_n, int n3);
extern void setstr(char* des, const char* src, int n);


/*��������λ�ýṹ��*/

//����λ�ô���
typedef struct Pos_t
{
	double X;
	double Y;
	double Z;
	double deltat;//����ǰ���źŴ���ʱ��
	double delta_t;//��������źŴ���ʱ��
}Pos_t;


#define C_V 299792458//���٣�m��
#define GM 398600500000000//������������
#define math_e 2.718281828459 //eֵ
#define PI 3.141592653589793
#define Earth_e 7.2921151467e-5 //������ת���ٶ�
#define C1 0

double TimetoGPSsec(int year, int month, int day, int hour, int min, double sec);
extern int select_epoch(double SecofWeek, int sPRN, pnav_body nav_b, int n3);
extern Pos_t sat_pos(double deltan, double sqrtA, double TOE, double M0, double e,
	double omega, double OMEGA, double deltaomega, double Cuc, double Crc, double Cic,
	double Cus, double Crs, double Cis, double i0, double IDOT, double delta_t, double deltat, double X_R,
	double Y_R, double Z_R, double GPSsec);

// ����һ���ṹ�����洢ÿ����Ԫ��ÿ�����ǵ���������ֵ
struct SatPos {
	double X;
	double Y;
	double Z;
};


extern void readnavfile();
extern void readobsfile();

