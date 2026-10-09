#include "platform.h"

#include "common.h" //WindowDataT, MouseDataT, KeyDataT
#include "calc.h"
#include "sim.h"
#include "config.h"

#include "ezTrack.h"
#include "ezTrack_Vicon.h" // VICON用のez_Tracker : Crescent

#include "Shapes.h"
#include "mymodel.h" //★

#include <stdio.h>

//-------------------------------------------------------------------------------------------------------
// VRFL
//-------------------------------------------------------------------------------------------------------
#include "VRFLInterface.h" // VRFL利用の為のインクルード
#include "vrconfig.h"
const char* configfile = "";
unsigned short remotePort = 12345;	// Unityの待ち受けポート

/**/
//inline float ObjX( ObjDataT *obj ){ return OBJ_X(obj);}
//inline float ObjY( ObjDataT *obj ){ return OBJ_Y(obj);}
//inline float ObjZ( ObjDataT *obj ){ return OBJ_Z(obj);}
//inline float ObjRoll( ObjDataT *obj ){ return OBJ_ROLL(obj);}
//inline float ObjPitch( ObjDataT *obj ){ return OBJ_PITCH(obj);}
//inline float ObjYaw( ObjDataT *obj ){ return OBJ_YAW(obj);}
/**/

SimDataT simdata; //SimDataT型構造体のデータを宣言
extern MouseDataT mouse;
extern KeyDataT keydata; //★修正★

ezTracker* tracker = nullptr; //共有メモリ経由でトラッカーの情報を得るオブジェクト
//トラッカーから受け取ったデータへのポインタ
ezTrackDataT* trackBase = nullptr; //基準マーカ
ezTrackDataT* trackHead = nullptr;
ezTrackDataT* trackBody = nullptr;
ezTrackDataT* trackHandR = nullptr;
ezTrackDataT* trackHandL = nullptr;
ezTrackDataT* trackFootR = nullptr;
ezTrackDataT* trackFootL = nullptr;
//マーカが見えない場合などのダミーデータ{id,x,y,z,roll,pitch,yaw}
ezTrackDataT localBase = { 0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
ezTrackDataT localHead = { 0, 0.0, 1.5, 0.0, 0.0, 0.0, 0.0 };
ezTrackDataT localBody = { 0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
ezTrackDataT localHandR = { 0, 0.25, 1.25, -0.5, 0.0, 0.0, 0.0 };
ezTrackDataT localHandL = { 0,-0.25, 1.25, -0.5, 0.0, 0.0, 0.0 };
ezTrackDataT localFootR = { 0, 0.25, 0.0, -0.5, 0.0, 0.0, 0.0 };
ezTrackDataT localFootL = { 0,-0.25, 0.0, -0.5, 0.0, 0.0, 0.0 };
////////////////////////////////////////////////////////

/*------------------------------------------------------------- copyTrackToObj
 * copyTrackToObj
 */
static void copyTrackToObj(ezTrackDataT* src, ObjDataT* dst)
{
	float pos[3], rot[3];
	if (src == NULL) return;
	ezTrack_getPos(src, pos);
	setObjPos(dst, pos);
	ezTrack_getRot(src, rot);
	setObjRot(dst, rot);
}
/*------------------------------------------------------------- setColor
 * setColor
 *--------*/
void setColor(color_t* col, float r, float g, float b, float a)
{
	col->red = r;
	col->green = g;
	col->blue = b;
	col->alpha = a;
}

/*---------------------------------------------------------------- InitScene
 * InitScene:
 *--------*/
void InitScene(void)
{
	printf(">>InitScene\n");

#ifndef MREALMODE
	/*
	tracker = new ezTracker( use_tracker ); //VICON使うときはtrue
	if (use_vicon) {
		tracker->open("VICON", false); //識別名, Wフラグ(false:R/O)
	}
	else {
		tracker->open("ARTOOLKIT", false); //識別名, Wフラグ(false:R/O)
	}
	*/
	//- Viconを使用する場合、ezTracker_Viconクラスを使用する:Crescent
	if (use_vicon) {
		tracker = new ezTracker_Vicon(use_tracker); //VICON使うときはtrue
		// 引数の文字列は、"VICON PCのIPアドレス：通信ポート番号(デフォルト：801)"
		tracker->open("172.23.85.186:801", false);
		//tracker->open("127.0.0.1:801", false);
	}
	else {
		tracker = new ezTracker(use_tracker); //VICON使うときはtrue
		tracker->open("ARTOOLKIT", false); //識別名, Wフラグ(false:R/O)
	}

	trackHead = &localHead;
	trackBody = &localBody;
	trackHandR = &localHandR;
	trackHandL = &localHandL;
	trackBase = &localBase;
	trackFootL = &localFootL;
	trackFootR = &localFootR;

	copyTrackToObj(trackHead, &simdata.head);
	copyTrackToObj(trackBody, &simdata.body);
	copyTrackToObj(trackHandL, &simdata.handL);
	copyTrackToObj(trackHandR, &simdata.handR);
	copyTrackToObj(trackFootL, &simdata.footL);
	copyTrackToObj(trackFootR, &simdata.footR);

#endif

	ezInitShape();

	////// 描画空間の奥行き・空気感・背景色設定
	simdata.clip_far = 100.0; //◆ファークリッププレーン
	simdata.clip_near = 0.1; //◆ニアクリッププレーン
	setColor(&simdata.fog, 1.0, 1.0, 1.0, 1.0);//◆フォグカラー
	setColor(&simdata.sky, 0.2, 0.3, 0.4, 0.2);//◆背景カラー
	//////

	///▼追加したオブジェクトの初期化
	setObjPos(&simdata.cube, 0.0, 1.0, -50.0);
	setObjRot(&simdata.cube, 0.0, 0.0, 60.0);
	setObjColor(&simdata.cube, 0.5, 0.3, 0.2);
	simdata.cube.visible = true;
	simdata.cube.state = 0; //////////////◆
	simdata.cube.radius = 0.2;

	simdata.cube.xsize = 8.0; //0.6
	simdata.cube.ysize = 2.0; //0.05
	simdata.cube.zsize = 0.05; //0.25

	setObjPos(&simdata.sphere, 0.0, 1.2, -1.0);
	setObjRot(&simdata.sphere, 0.0, 0.0, 0.0);
	setObjColor(&simdata.sphere, 1.0, 0.5, 0.0);
	simdata.sphere.visible = true;
	simdata.sphere.state = 0;
	simdata.sphere.radius = 0.25; //★◆04

	simdata.handR.radius = 0.125; //◆04
	simdata.handL.radius = 0.125; //◆04
	simdata.handR.state = 0;
	simdata.handL.state = 0;

	setObjPos(&simdata.player, 0.0, 0.0, 0.0);
	setObjRot(&simdata.player, 0.0, 0.0, 0.0);
	setObjColor(&simdata.player, 0.0, 0.5, 1.0);
	simdata.player.visible = true;
	simdata.player.state = 0;
	simdata.player.turn = 0.0;
	simdata.player.move = 0.0;
	simdata.player.radius = 0.5;

	setObjPos(&simdata.head, 0.0, 1.6, 0.0);
	setObjRot(&simdata.head, 0.0, 0.0, 0.0);
	setObjColor(&simdata.head, 0.0, 0.5, 1.0);
	simdata.player.visible = true;
	simdata.player.state = 0;
	simdata.player.turn = 0.0;
	simdata.player.move = 0.0;
	simdata.player.radius = 0.5;

	//右手（ローカル座標）をプレイヤの子座標系とする
	setObjLocal(&simdata.handR, &simdata.player); //★

	//★左手も同様
	setObjLocal(&simdata.handL, &simdata.player); //★

	//頭をプレイヤーの子座標系にする
	setObjLocal(&simdata.head, &simdata.player);

	simdata.active_camera = &simdata.head;

	//simdata.active_camera = NULL;
	//プレイヤオブジェクトのアドレスをカメラのポインタに紐付ける

	setObjColor(&simdata.handR, 0.0, 1.0, 0.0); //右手グリーン
	setObjColor(&simdata.handL, 1.0, 0.0, 0.0); //左手レッド

	CreateMyModels(); //★

	simdata.cube.visible = true;
	simdata.cube.state = 0; //////////////◆
	simdata.cube.radius = 0.2;

	simdata.scale = 0;
	simdata.octabe = 5;

	ezMIDI::Open(true); //polyphonic
	simdata.midi = new ezMIDI(9);
	//simdata.midi = new ezMIDI(10, 27 );

#ifdef ZIGSIM
	simdata.zigsim = ezZigSim::get();
	simdata.zigsim->init();
#endif

#ifdef WITMOTION
	simdata.gyro = new ezGyro();
	simdata.gyro->open(4, 115200);
#endif

	simdata.png_test = new ezImage("../images/window.png");

	//-------------------------------------------------------------------------------------------------------
	// VRFL
	//-------------------------------------------------------------------------------------------------------
	VRFL::InitializeSim(remotePort);				// VRFL利用前に初期化, remotePortはUnity側の待ち受けポート番号
	vrconfig::LoadConfig(configfile);				// 設定ファイル(CSV)の読込(詳細はvrconfig::LoadConfigに記載)
	simdata.head.id = 0;							// Head(カメラ)のオブジェクトには "0" を振る(必須)
	simdata.handR.id = vrconfig::GetID("handR");	// csvファイルからID取得
	simdata.handL.id = vrconfig::GetID("handL");	// csvファイルからID取得
	simdata.sphere.id = vrconfig::GetID("heart");	// csvファイルからID取得
	simdata.handR.offrot.yaw = 90;					// モデル調整の為のオフセット回転
	simdata.handL.offrot.yaw = 90;					// モデル調整の為のオフセット回転

	printf(".\n");
	Sleep(1000);

	return;
}

/*-------------------------------------------------------------- UpdateScene
 * UpdateScene:
 *--------*/
void UpdateScene(void)
{
	simdata.time = glutGet(GLUT_ELAPSED_TIME);
	//printf("UpdateScene %d\n", simdata.time);

#ifdef ZIGSIM
	simdata.zigsim->update(true);
#endif

#ifdef WITMOTION
	simdata.gyro->read();
#endif

#ifdef MREALMODE
	for (int i = 0; i < N_TARGET; i++) {
		TargetToObjData(&simdata.TargetList[i], &simdata.target[i]);
	}
	TargetToObjData(&simdata.TargetList[0], &simdata.handR);
	TargetToObjData(&simdata.TargetList[1], &simdata.handL);
	TargetToObjData(&simdata.mrealCamera[0], &simdata.head);
	//copyObj( &simdata.target[0], &simdata.handR );
	//copyObj( &simdata.target[1], &simdata.handL );
#else
	//////// データ更新：手の動きや体の位置・姿勢 ////////
	if (use_tracker) {
		//トラッカーからのデータをゲット
		tracker->read(); //（共有メモリ）ネット経由でデータを読み出す
		if (use_vicon) {
			trackHead = tracker->getTrackData("CAP"); //VICONマーカの名前
			trackHandR = tracker->getTrackData("TREE_A");
			trackHandL = tracker->getTrackData("TREE_B");
		}
		else {
			trackHead = tracker->getTrackData(0); //ARTOOLKITマーカの番号
			trackHandR = tracker->getTrackData(1);
			trackHandL = tracker->getTrackData(2);
		}

		trackBody = tracker->getTrackData("Chest");
		trackBase = tracker->getTrackData("Candy");
		trackFootR = tracker->getTrackData("RightFoot");
		trackFootL = tracker->getTrackData("LeftFoot");

		copyTrackToObj(trackHead, &simdata.head);
		copyTrackToObj(trackBody, &simdata.body);
		copyTrackToObj(trackHandL, &simdata.handL);
		copyTrackToObj(trackHandR, &simdata.handR);
		copyTrackToObj(trackFootL, &simdata.footL);
		copyTrackToObj(trackFootR, &simdata.footR);

		//解説：構造体のポインタを引数とする
		//&: アドレス（＝ポインタ）を渡すことを指定
		simdata.handR.pos.z -= 0.5; //########
		simdata.handL.pos.z -= 0.5; //########
	}
	else {
		//◆01◆コメント化

		//copyTrackToObj( trackHandL, &simdata.handL );
		//copyTrackToObj( trackHandR, &simdata.handR );

		//◆02◆マウスで右手handRを動かす
		//感度調整＋基準位置調整 ########
		simdata.handR.pos.x = mouse.x * 2.0 + 0.25; // mouse.x: -1.0（左端）～1.0（右端）
		simdata.handR.pos.y = -mouse.y * 2.0 + 1.2; // mouse.y; -1.0（上端）～1.0（下端）
		simdata.handR.rot.pitch = (simdata.handR.pos.y - 1.2) * 100.0;
		simdata.handR.rot.yaw = (simdata.handR.pos.x - 0.25) * (-100.0);
		simdata.handR.rot.roll = simdata.handR.rot.yaw - 30.0;
		//Z座標はlocalHandRで初期設定のまま
		//▲

		//◆03◆ついでに左手handLも動かす～右手と上下左右を逆にしたりする
		//感度調整＋基準位置調整 ########
		simdata.handL.pos.x = -mouse.x * 2.0 - 0.25; // mouse.x: -1.0（左端）～1.0（右端）
		simdata.handL.pos.y = mouse.y * 2.0 + 1.2; // mouse.y; -1.0（上端）～1.0（下端）
		simdata.handL.rot.pitch = (simdata.handL.pos.y - 1.2) * 100.0;
		simdata.handL.rot.yaw = (simdata.handL.pos.x + 0.25) * (-100.0);
		simdata.handL.rot.roll = simdata.handL.rot.yaw + 30.0;
		//Z座標はlocalHandRで初期設定のまま
		//Z座標はlocalHandLで初期設定のまま
		//▲

		////★前後移動追加
		if (keydata.arrowUp) {
			simdata.handL.pos.z -= 0.01; //###### VECTOR Z
		}
		if (keydata.arrowDown) {
			simdata.handL.pos.z += 0.01; //###### VECTOR Z
		}
		//////////★
	}

#endif
	/*
	//----------------------------------------------- マウスで移動する
	{
		simdata.player.turn = - 0.5 * mouse.xRel;
		simdata.player.move = - 0.2 * mouse.yRel;
		MoveObject( &simdata.player );
	}
	*/

	//★定数として変数を使いたいときには「const」をつける
	const float yon = 1.25, yoff = 1.15; //値が違うことには意味がある

	//★前の値を保持したいときには「static」をつける
	static float xo, zo; //移動モードがオンになったときの手の位置

	//★左手の動作で移動する操作
	switch (simdata.handL.state) {
	case 0: //◆非移動モード
		if (simdata.handL.pos.y > yon) {  // simdata.handL.pos.y
			simdata.handL.state = 1;
			xo = simdata.handL.pos.x; // simdata.handL.pos.x
			zo = simdata.handL.pos.z; // simdata.handL.pos.z
			//setObjColor(&simdata.handL, 1.0, 1.0, 0.0);
		}
		break;
	case 1: //◆移動モード
		if (simdata.handL.pos.y < yoff) { // simdata.handL.pos.y
			simdata.handL.state = 0;
			//setObjColor(&simdata.handL, 1.0, 0.0, 0.0);
		}
		//移動の処理

		simdata.player.turn = -0.25 * (simdata.handL.pos.x - xo); // simdata.handL.pos.x

		simdata.player.move = -0.01 * (simdata.handL.pos.z - zo); // simdata.handL.pos.z

		break;
	}

#ifdef ZIGSIM
	{
		//左右に傾けると左右旋回
		if (simdata.zigsim->data.angle.roll > 5) {
			simdata.player.turn = (simdata.zigsim->data.angle.roll - 5) * 0.0125;
		}
		if (simdata.zigsim->data.angle.roll < -5) {
			simdata.player.turn = (simdata.zigsim->data.angle.roll + 5) * 0.0125;
		}
		//前後に傾けると前進後退
		if (simdata.zigsim->data.angle.pitch < -5) {
			simdata.player.move = -(simdata.zigsim->data.angle.pitch + 5) * 0.0025;
		}
		if (simdata.zigsim->data.angle.pitch > 5) {
			simdata.player.move = -(simdata.zigsim->data.angle.pitch - 5) * 0.0025;
		}
	}
#endif

#ifdef WITMOTION
	{
		//左右に傾けると左右旋回
		if (simdata.gyro->data.angle.roll > 5) {
			simdata.player.turn = (simdata.gyro->data.angle.roll - 5) * 0.0125;
		}
		if (simdata.gyro->data.angle.roll < -5) {
			simdata.player.turn = (simdata.gyro->data.angle.roll + 5) * 0.0125;
		}
		//前後に傾けると前進後退
		if (simdata.gyro->data.angle.pitch < -5) {
			simdata.player.move = -(simdata.gyro->data.angle.pitch + 5) * 0.0025;
		}
		if (simdata.gyro->data.angle.pitch > 5) {
			simdata.player.move = -(simdata.gyro->data.angle.pitch - 5) * 0.0025;
		}
	}
#endif
	/*
	if (isHitBox(&simdata.cube, &simdata.player)) {
		simdata.player.pos.z += 0.5;
	}
	*/

	MoveObject(&simdata.player);

	//状態遷移のチェック、状態遷移、
	//◆04
	bool ishit;
	switch (simdata.sphere.state) {
	case 0://★右手から外れ、かつ右手に触れていない状態
		ishit = isHit(&simdata.sphere, &simdata.handR);
		if (ishit) {
			setObjColor(&simdata.sphere, 0.0, 1.0, 0.5);

			//◆05
			simdata.sphere.state = 1;
			moveWorldToLocal(&simdata.sphere, &simdata.handR);
		}
		else {
			setObjColor(&simdata.sphere, 0.7, 0.7, 0.7);
		}
		break;

	case 1: //◆06★右手に把持されている状態
		ishit = isHit(&simdata.sphere, &simdata.handL);
		if (ishit) {
			setObjColor(&simdata.sphere, 1.0, 0.5, 0.0);
			moveLocalToWorld(&simdata.sphere);
			simdata.sphere.state = 2;//◆07 0 -> 2
		}
		break;

	case 2://◆08 右手の把持から外れたが触っている状態
		ishit = isHit(&simdata.sphere, &simdata.handR);
		if (!ishit) {
			setObjColor(&simdata.sphere, 0.7, 0.7, 0.7);
			simdata.sphere.state = 0;
		}
		break;

	}

	if (isHitBox(&simdata.cube, &simdata.handL)) {
		setObjColor(&simdata.cube, 1.0, 0.0, 0.0);
		////simdata.movie->play(simdata.time);
	}
	else {
		setObjColor(&simdata.cube, 0.0, 1.0, 0.0);
	}

	// ---------------------------------------------------------
	// VRFL
	// ---------------------------------------------------------
	ObjDataT* pdata;
	pdata = &simdata.head;
	VRFL::Send((void**)&pdata, 1); // head情報送信
	pdata = &simdata.handL;
	VRFL::Send((void**)&pdata, 1); // handL情報送信
	pdata = &simdata.handR;
	VRFL::Send((void**)&pdata, 1); // handR情報送信
    pdata = &simdata.sphere;
    VRFL::Send((void**)&pdata, 1); // sphere情報送信

    return;
}
////////
void TermScene(void)
{
	//// 終了処理
	printf("GOING TO EXIT..\n");

#ifdef ZIGSIM
	simdata.zigsim->term();
#endif

#ifdef WITMOTION
	simdata.gyro->close();
#endif

	//Sleep( 5000 );
	printf("BYE\n");

	return;
}
