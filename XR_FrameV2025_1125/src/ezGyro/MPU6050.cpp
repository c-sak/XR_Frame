#include "calc.h"
#include "ezGyro/Com.h"
#include "stdio.h"

int comm;

//------------------------------------------------------------------------------------
static void CopeSerialData2(unsigned char* buf, int len, vector_t* acc, euler_t* rot)
{
	float ax, ay, az;
	float gx, gy, gz;
	float roll, pitch, yaw;
	float temp;

	unsigned short data[10]; /////////

	int i = 0;
	if (buf[0] != 0xff) {//先頭が255でなかったら
		//先頭がずれているので、いくつかデータを読み飛ばす

		while (buf[i] != 0xff) {//255まで飛ばす
			i++;
			if (i > len) return;
		}
	}
	i++;
	unsigned char* p = &buf[i]; ////////

	//buf[i]より後の数をbufbuf[0]の後に代入する
	for (int j = 0; j < 10; j++) {
		data[j] = (*p << 8) | *p;
		p += 2;
	}

	const float ACCMAX = 20.0;
	const float GYROMAX = 500.0;
	const float K = 32767;

	acc->x = ((float)data[0] / K - 1.0) * ACCMAX;
	acc->y = ((float)data[1] / K - 1.0) * ACCMAX;
	acc->z = ((float)data[2] / K - 1.0) * ACCMAX;
	gx = ((float)data[3] / K - 1.0) * GYROMAX;
	gy = ((float)data[4] / K - 1.0) * GYROMAX;
	gz = ((float)data[5] / K - 1.0) * GYROMAX;
	rot->roll = ((float)data[6] / K - 1.0) * 180;
	rot->pitch = ((float)data[7] / K - 1.0) * 90;
	rot->yaw = ((float)data[8] / K - 1.0) * 180;

	temp = ((float)data[9] / K - 1.0) * 100;

	//Sleep(0); //// 削除してよい
	/////////////////////////////////////////////////////////
}
//-------------------------------------------------------------------------------------------
static int CopeSerialData3(unsigned char* buf, int len, vector_t* acc, euler_t* rot, euler_t* gyro)
{
	float ax, ay, az;
	float gx, gy, gz;
	float roll, pitch, yaw;
	float temp;
	int mpu;

	unsigned short data[10]; /////////

	int i = 0;
	if (buf[0] != 0xff) {//先頭が255でなかったら
		//先頭がずれているので、いくつかデータを読み飛ばす

		while (buf[i] != 0xff) {//255まで飛ばす
			i++;
			if (i > len) return 0; /////
		}
	}
	i++;
	mpu = buf[i]; ////////
	i++; ////////
	unsigned char* p = &buf[i];

	//buf[i]より後の数をbufbuf[0]の後に代入する
	for (int j = 0; j < 10; j++) {
		data[j] = (*p << 8) | *p;
		p += 2;
	}

	const float ACCMAX = 20.0;
	const float GYROMAX = 500.0;
	const float K = 32767;

	acc[mpu].x = ((float)data[0] / K - 1.0) * ACCMAX;
	acc[mpu].y = ((float)data[1] / K - 1.0) * ACCMAX;
	acc[mpu].z = ((float)data[2] / K - 1.0) * ACCMAX;
	gyro[mpu].roll = ((float)data[3] / K - 1.0) * GYROMAX;
	gyro[mpu].pitch = ((float)data[4] / K - 1.0) * GYROMAX;
	gyro[mpu].yaw = ((float)data[5] / K - 1.0) * GYROMAX;
	rot[mpu].roll = ((float)data[6] / K - 1.0) * 180;
	rot[mpu].pitch = ((float)data[7] / K - 1.0) * 90;
	rot[mpu].yaw = ((float)data[8] / K - 1.0) * 180;

	temp = ((float)data[9] / K - 1.0) * 100;

	//Sleep(0); //// 削除してよい
	/////////////////////////////////////////////////////////

	i += 20; ////////
	return i;////////
}

void GetDataFromArduino(vector_t* acc, euler_t* rot, euler_t* gyro)
{
	unsigned char buf[2000];
	/////////////////////////////////////////////////
	static int count = 0;
	static int loop = 0;

	unsigned short len = 0;

	len = CollectUARTData(comm, (char*)buf);
	loop++;

	/*
	system("cls");

	for (int i = 0; i < len; i++) {
		printf(" %02x", buf[i]);
	}
	printf("\n");
	*/

	int head = 0;////////
	//if (len > 1)
	while (len - head > 1) ////////
	{
		////printf("count %d/%d\n", count++, loop);
		//CopeSerialData2(buf, len, acc, rot );
		int stat;
		stat = CopeSerialData3(&buf[head], len - head, acc, rot, gyro);
		if (stat < 1) break;
		else head += stat;
	}
	/*
	else {
		////printf("count %d/%d\t%d\n", count++, loop, len);
	}
	*/
	////printf("ACC: %8.3f %8.3f %8.3f\n",   acc->x, acc->y, acc->z);
	printf("ANGLE: %8.3f %8.3f %8.3f\n", rot->roll, rot->pitch, rot->yaw);

	//Sleep(0);
	return;
}

