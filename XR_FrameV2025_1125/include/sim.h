#ifndef __SIM_H__
#define __SIM_H__

#include "platform.h"

#include "calc.h"
//#include "object.h"
#include "image.h"
#include "ezMovie.h"
#include "ezMIDI.h"
#include "tcpData.h"
#include "ezZigSim.h"
#include "ezGyro/ezGyro.h"

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

//////// MREAL
#define N_TARGET 15
#define N_MARKER 2048

//-------- simulation data
typedef struct {

	ezCamera *camL; //右目カメラ
	ezCamera *camR; //左目カメラ

	int time;

	float clip_near;//ニアクリップ距離
	float clip_far;//ファークリップ距離
	color_t fog; //フォグカラー
	color_t sky; //背景カラー
    //////// ユーザ定義のシーンについてのデータをここにまとめる

	ObjDataT cube; ///////◆キューブ（「板」）
	ObjDataT sphere; /////◆球体（「心臓」）

	ObjDataT player; //★プレイヤ（の足元★）をあらわすオブジェクト

	ObjDataT head;//★
	ObjDataT body;
	ObjDataT handL;//★
	ObjDataT handR;//★
	ObjDataT footL;
	ObjDataT footR;

	ObjDataT *active_camera; //カメラのポインタ変数

	//---- MREAL用変数★
	ObjDataT target[N_TARGET];
	int n_cam;
	TargetT mrealCamera[8];
	TargetT TargetList[N_TARGET];
	MarkerT MarkerList[N_MARKER];
	//--------


	ezMIDI *midi;
	int scale;
	int octabe;

	ezZigSim* zigsim;
	ezGyro* gyro;
	////////

	ezImage* png_test;

} SimDataT;
#endif //__SIM_H__
