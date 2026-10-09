#define _CRT_SECURE_NO_WARNINGS
#include "ezGyro/JY901.h"
#include "string.h"
#include "stdio.h"
#include "ezGyro/Com.h"

//--------
void CJY901::open( int port, int baudrate )
{
	signed char cResult = 1;
	while (cResult != 0)
	{
		cResult = OpenCOMDevice(port, baudrate);
	}
	this->port = port;
	this->baudrate = baudrate;
	usRxLength = 0;
	stcDStatus.sDStatus[0] = 0;
	stcDStatus.sDStatus[1] = 0;
	stcDStatus.sDStatus[2] = 0;
	stcDStatus.sDStatus[3] = 0;
}
//--------
#if 1
void CJY901::CopeSerialData(char ucData[], unsigned short usLength)
{
	memcpy(chrTemp, ucData, usLength);
	static unsigned char *p; ////////

	p = chrTemp; ////////
	////chrTemp = (unsigned char*)ucData; ////////

	usRxLength += usLength;
	while (usRxLength >= 11)
	{
		if( *p != 0x55 )
		{
			usRxLength--;
			//memcpy(&chrTemp[0], &chrTemp[1], usRxLength);
			p++; ////////

			continue;
		}
		switch(p[1])
		{
		case 0x50:	memcpy(&stcTime, &p[2], 8);break;
		case 0x51:	memcpy(&stcAcc, &p[2], 8);break;
		case 0x52:	memcpy(&stcGyro, &p[2], 8);break;
		case 0x53:	memcpy(&stcAngle, &p[2], 8);break;
		case 0x54:	memcpy(&stcMag, &p[2], 8);break;
		case 0x55:	memcpy(&stcDStatus, &p[2], 8);break; //
		case 0x56:	memcpy(&stcPress, &p[2], 8);break;
		case 0x57:	memcpy(&stcLonLat, &p[2], 8);break;
		case 0x58:	memcpy(&stcGPSV, &p[2], 8);break;
		case 0x59:	memcpy(&quatern, &p[2], 8);break;
		case 0x5A:	memcpy(&dop, &p[2], 8);break;
		default:
			printf("????????\n");
		}
		usRxLength -= 11;
		//memcpy(&chrTemp[0], &chrTemp[11], usRxLength);
		p += 11; ////////
	}
}
#else
void CJY901::CopeSerialData(char ucData[],unsigned short usLength )
{
	//static unsigned char chrTemp[2000]; ////ƒƒ“ƒo•Ï”‚ÉˆÚ“®
	//static unsigned short usRxLength = 0; ////ƒƒ“ƒo•Ï”‚ÉˆÚ“®

	memcpy(chrTemp,ucData,usLength);
	usRxLength += usLength;
    while (usRxLength >= 11)
    {
        if (chrTemp[0] != 0x55)
        {
			usRxLength--;
			memcpy(&chrTemp[0],&chrTemp[1],usRxLength);
            continue;
        }
		switch(chrTemp[1])
		{
			case 0x50:	memcpy(&stcTime,&chrTemp[2],8);break;
			case 0x51:	memcpy(&stcAcc,&chrTemp[2],8);break;
			case 0x52:	memcpy(&stcGyro,&chrTemp[2],8);break;
			case 0x53:	memcpy(&stcAngle,&chrTemp[2],8);break;
			case 0x54:	memcpy(&stcMag,&chrTemp[2],8);break;
			case 0x55:	memcpy(&stcDStatus,&chrTemp[2],8);break;
			case 0x56:	memcpy(&stcPress,&chrTemp[2],8);break;
			case 0x57:	memcpy(&stcLonLat,&chrTemp[2],8);break;
			case 0x58:	memcpy(&stcGPSV,&chrTemp[2],8);break;
		}
		usRxLength -= 11;
		memcpy(&chrTemp[0],&chrTemp[11],usRxLength);
    }
}
#endif
//--------
void CJY901::read(void)
{
	char buf[2000];
	unsigned short len = 0;

	len = CollectUARTData(port, buf);
	if (len > 0)
	{
		CopeSerialData(buf, len);
	}
	return;
}
//--------
void CJY901::print(void)
{
	CJY901 *jy = this;

	printf("Temperature:%f\n",
		(float)jy->stcAcc.T * 0.01);

	printf("Time:20%d-%d-%d %d:%d:%8.3f\n",
		(short)jy->stcTime.ucYear, 
		(short)jy->stcTime.ucMonth,
		(short)jy->stcTime.ucDay, 
		(short)jy->stcTime.ucHour,
		(short)jy->stcTime.ucMinute,
		(float)jy->stcTime.ucSecond + (float)jy->stcTime.usMiliSecond / 1000);

	printf("Acc[G]:%8.3f %8.3f %8.3f %d\n",
		(float)jy->stcAcc.a[0] / 32768 * 16,
		(float)jy->stcAcc.a[1] / 32768 * 16,
		(float)jy->stcAcc.a[2] / 32768 * 16,
		jy->stcAcc.T);

	printf("Gyro[degree/sec]:%8.3f %8.3f %8.3f %d\n",
		(float)jy->stcGyro.w[0] / 32768 * 2000,
		(float)jy->stcGyro.w[1] / 32768 * 2000,
		(float)jy->stcGyro.w[2] / 32768 * 2000,
		jy->stcGyro.T);

	printf("Angle[degree]:<Rx(roll) %8.3f> <Ry(pitch) %8.3f> <Rx(yaw) %8.3f ?%x\n",
		(float)jy->stcAngle.Angle[0] / 32768 * 180,
		(float)jy->stcAngle.Angle[1] / 32768 * 180,
		(float)jy->stcAngle.Angle[2] / 32768 * 180,
		jy->stcAngle.T);

	printf("Mag[uT]:%8.3f %8.3f %8.3f %d\n",
		(float)jy->stcMag.h[0] / 300 * 2,
		(float)jy->stcMag.h[1] / 300 * 2,
		(float)jy->stcMag.h[2] / 300 * 2,
		jy->stcMag.T);

	printf("Pressure[hPa]:%8.3f\tHeight[m]:%.2f\n",
		(float)jy->stcPress.lPressure * 0.01,
		(float)jy->stcPress.lAltitude / 100);

	printf("DStatus:%lx %lx %lx %lx\n",
		jy->stcDStatus.sDStatus[0],
		jy->stcDStatus.sDStatus[1],
		jy->stcDStatus.sDStatus[2],
		jy->stcDStatus.sDStatus[3]);

	printf("Longitude:%ldDeg%.5fm Lattitude:%ldDeg%.5fm\n",
		jy->stcLonLat.lLon / 10000000,
		(double)(jy->stcLonLat.lLon % 10000000) / 1e5,
		jy->stcLonLat.lLat / 10000000,
		(double)(jy->stcLonLat.lLat % 10000000) / 1e5);

	printf("GPSHeight:%.1fm GPSYaw:%.1fDeg GPSV:%.3fkm/h\n\n",
		(float)jy->stcGPSV.sGPSHeight / 10,
		(float)jy->stcGPSV.sGPSYaw / 10,
		(float)jy->stcGPSV.lGPSVelocity / 1000);

	printf("Quaternion:%8.3f %8.3f %8.3f %8.3f\n",
		(float)jy->quatern.q0 / 32768,
		(float)jy->quatern.q1 / 32768,
		(float)jy->quatern.q2 / 32768,
		(float)jy->quatern.q3 / 32768);

	printf("DOP:%d %8.3f %8.3f %8.3f\n",
		jy->dop.nsat,
		(float)jy->dop.pdop/100,
		(float)jy->dop.hdop/100,
		(float)jy->dop.vdop/100);
}
//--------
void CJY901::close(void)
{
	CloseCOMDevice();
}
