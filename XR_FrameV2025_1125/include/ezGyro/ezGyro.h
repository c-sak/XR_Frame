#pragma once
#include "ezGyro/JY901.h"
/*
typedef struct {
	float x, y, z;
} vector_t;

typedef struct {
	float roll, pitch, yaw;
} euler_t;
*/
typedef struct {
	float q0;
	float q1;
	float q2;
	float q3;
} quatern_t;

typedef struct {
	float longitude;//緯度
	float latitude;//経度
	float altitude; //高度
	float azimuth;  //方位角
	float velocity; //速度

	int n_sat; //number of satellites
	float pdop; //position dilution of precision
	float hdop; //horizontal dilution of precision
	float vdop; //vertical dilution of precision
} gps_t;

typedef struct {
	float pressure; //気圧
	float altitude; //高度
	float temp;     //気温
} atmos_t;

typedef struct {
	int year;
	int month;
	int day;
	int hour;
	int minute;
	float second;
} times_t;

typedef struct {

	vector_t acc;
	euler_t gyro;
	euler_t angle;
	quatern_t quatern;
	vector_t mag;
	gps_t gps;
	atmos_t atmos;
	times_t clock;

	short status[4];

	CJY901 *jy;

} WitMotionT;

WitMotionT* ConnectWitMotion(int port, int baudrate );
bool ReadWitMotion(WitMotionT *wmdata);
void PrintWitMotion(WitMotionT *wmdata);
bool DisconnectWitMotion(WitMotionT *wmdata);

class WitMotionGPSIMU: public CJY901
{
	WitMotionGPSIMU() {}
	~WitMotionGPSIMU() {}
};

class ezGyro
{
public:
	WitMotionT data;

	ezGyro() {}
	~ezGyro() {}

	void open( int com, int baud) {
		//ConnectWitMotion(com, baud);
		data.jy = new CJY901();
		data.jy->open(com, baud);
	}
	void read() {
		ReadWitMotion(&data);
	}
	void print() {
		PrintWitMotion(&data);
	}
	void close() {
		DisconnectWitMotion(&data);
	}
};
