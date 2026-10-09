#ifndef __SIM_H__
#define __SIM_H__

#include "platform.h"
#include "calc.h"
#include "object.h"

// sim.cpp
void InitScene( void );
void UpdateScene( void );
void TermScene( void );

// draw.cpp
void PreDraw( void );
void PostDraw( void );
void DrawScene( void );
//void Lighting( void );
//void HeadLight( void );
/*
★ObjDataTの定義は、object.hに移行しました
*/

//////// MREAL
#define N_TARGETS 15
#define N_MARKERS 2048

//-------- simulation data
typedef struct {
	int time;

	float clip_near;
	float clip_far;
	float air_color[4];
	float sky_color[4];
    //////// ユーザ定義のシーンについてのデータをここにまとめる

	ObjDataT cube; ///////◆キューブ
	ObjDataT sphere; /////◆球体

	ObjDataT player; /////◆プレイヤ（の足元★）をあらわすオブジェクト

	ObjDataT head;//★
	ObjDataT body;
	ObjDataT handL;//★
	ObjDataT handR;//★
	ObjDataT footL;
	ObjDataT footR;

	ObjDataT *active_camera; //カメラのポインタ変数

	ObjDataT target[N_TARGET]; //////// MREAL
	
	TargetT TargetList[N_TARGET];
	MarkerT MarkerList[N_MARKER];

	////////
} SimDataT;

#endif //__SIM_H__
