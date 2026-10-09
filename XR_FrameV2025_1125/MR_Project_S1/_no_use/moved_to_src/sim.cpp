#include "platform.h"

#include "common.h" //WindowDataT, MouseDataT, KeyDataT
#include "calc.h"
#include "sim.h"
#include "config.h"

#include "ezTrack.h"

#define MREALMODE

#ifndef MREALMODE
#include "gyro.h"
#endif

#include "mymodel.h" //★

inline float ObjX( ObjDataT *obj ){ return OBJ_X(obj);}
inline float ObjY( ObjDataT *obj ){ return OBJ_Y(obj);}
inline float ObjZ( ObjDataT *obj ){ return OBJ_Z(obj);}
inline float ObjRoll( ObjDataT *obj ){ return OBJ_ROLL(obj);}
inline float ObjPitch( ObjDataT *obj ){ return OBJ_PITCH(obj);}
inline float ObjYaw( ObjDataT *obj ){ return OBJ_YAW(obj);}

SimDataT simdata; //SimDataT型構造体のデータを宣言
extern MouseDataT mouse;
extern KeyDataT keydata; //★修正★

///////////////////////////////////////////////////////
//トラッカーデバイスを有効にするフラグ
//bool use_tracker = false; //有効にするときtrue

ezTracker *tracker; //共有メモリ経由でトラッカーの情報を得るオブジェクト
//トラッカーから受け取ったデータへのポインタ
ezTrackDataT *trackBase; //基準マーカ
ezTrackDataT *trackHead;
ezTrackDataT *trackBody;
ezTrackDataT *trackHandR;
ezTrackDataT *trackHandL;
ezTrackDataT *trackFootR;
ezTrackDataT *trackFootL;
//マーカが見えない場合などのダミーデータ
ezTrackDataT localBase = { 0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
ezTrackDataT localHead = { 0, 0.0, 1.5, 0.0, 0.0, 0.0, 0.0 };
ezTrackDataT localBody = { 0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
ezTrackDataT localHandR = { 0, 0.25, 1.25, -2.0, 0.0, 0.0, 0.0 };//★-2.0
ezTrackDataT localHandL = { 0,-0.25, 1.25, -2.0, 0.0, 0.0, 0.0 };//★-2.0
ezTrackDataT localFootR = { 0, 0.25, 0.0, -0.5, 0.0, 0.0, 0.0 };
ezTrackDataT localFootL = { 0,-0.25, 0.0, -0.5, 0.0, 0.0, 0.0 };
////////////////////////////////////////////////////////

/*--------------------
 * copyTrackToObj
 */
static void copyTrackToObj( ezTrackDataT *src, ObjDataT *dst )
{
	float pos[3], rot[3];
	if( src == NULL ) return;
	ezTrack_getPos( src, pos );
	setObjPos( dst, pos );
	ezTrack_getRot( src, rot );
	setObjRot( dst, rot );
}

/*---------------------------------------------------------------- InitScene
 * InitScene:
 *--------*/
void InitScene( void )
{
	////// シーンデータの初期化
	simdata.clip_far = 200.0; //◆ファークリッププレーン
	simdata.clip_near = 0.1;
	simdata.air_color[0] = 1.0;
	simdata.air_color[1] = 1.0;
	simdata.air_color[2] = 1.0;
	simdata.air_color[3] = 0.5; // fog density factor
	simdata.sky_color[0] = 0.2;
	simdata.sky_color[1] = 0.3;
	simdata.sky_color[2] = 0.4;
	simdata.sky_color[3] = 0.5; // sky color factor
	//////

	///▼追加したオブジェクトの初期化
	setObjPos( &simdata.cube, 1.0, 0.0, -3.0 );
	setObjRot( &simdata.cube, 0.0, 45.0, 30.0 );
    setObjColor( &simdata.cube, 0.0, 0.0, 0.0 );
	simdata.cube.visible = true;
	simdata.cube.state = 0; //////////////◆
    simdata.cube.radius = 0.5;

	simdata.cube.xsize = 2.0;
	simdata.cube.ysize = 4.0;
	simdata.cube.zsize = 0.2;

	setObjPos( &simdata.sphere, -1.0, 0.0, -5.0 );
	setObjRot( &simdata.sphere, 0.0, 0.0, 0.0 );
	setObjColor( &simdata.sphere, 1.0, 0.5, 0.0 );
	simdata.sphere.visible = true;
	simdata.sphere.state = 0;
	simdata.sphere.radius = 0.5; //★◆04

	simdata.handR.radius = 0.25; //◆04
	simdata.handL.radius = 0.25; //◆04
	simdata.handR.state = 0;
	simdata.handL.state = 0;
	
	setObjPos( &simdata.player, 0.0, 0.0, 0.0 );
	setObjRot( &simdata.player, 0.0, 0.0, 0.0 );
	setObjColor( &simdata.player, 0.0, 0.5, 1.0 );
	simdata.player.visible = true;
	simdata.player.state = 0;
	simdata.player.turn = 0.0;
	simdata.player.move = 0.0;
	simdata.player.radius = 0.5;
	
	//右手（ローカル座標）をプレイヤの子座標系とする
	setObjLocal( &simdata.handR, &simdata.player ); //★

	//★左手も同様
	setObjLocal( &simdata.handL, &simdata.player ); //★

	//頭をプレイヤーの子座標系にする
	setObjLocal(&simdata.head, &simdata.player );

	simdata.active_camera = &simdata.player;
	//simdata.active_camera = NULL;
	//プレイヤオブジェクトのアドレスをカメラのポインタに紐付ける

	setObjColor( &simdata.handR, 0.0, 1.0, 0.0 ); //右手グリーン
	setObjColor( &simdata.handL, 1.0, 0.0, 0.0 ); //左手レッド

#ifndef MREALMODE
	tracker = new ezTracker( use_tracker ); //VICON使うときはtrue
	tracker->open( "VICON", false ); //識別名, Wフラグ(false:R/O)

	trackHead = &localHead;
	trackBody = &localBody;
    trackHandR = &localHandR;
	trackHandL = &localHandL;
	trackBase = &localBase;
	trackFootL = &localFootL;
	trackFootR = &localFootR;

	//★修正★UpdataSceneからコピー
	copyTrackToObj( trackHead, &simdata.head );
    copyTrackToObj( trackBody, &simdata.body );
	copyTrackToObj( trackHandL, &simdata.handL );
	copyTrackToObj( trackHandR, &simdata.handR );
	copyTrackToObj( trackFootL, &simdata.footL );
	copyTrackToObj( trackFootR, &simdata.footR );

	if( use_gyro ) InitGyro();
#endif

	CreateMyModels(); //★

	return;
}
/*-------------------------------------------------------------- UpdateScene
 * UpdateScene:
 *--------*/
void UpdateScene( void )
{
#ifdef MREALMODE
	for( int i = 0; i< N_TARGET; i++ ){
		TargetToObjData( &simdata.TargetList[i], &simdata.target[i] ); 
	}
	copyObj( &simdata.target[0], &simdata.handR );
	copyObj( &simdata.target[1], &simdata.handL );
#else
	//////// データ更新 ////////
	if( use_tracker ){
		//トラッカーからのデータをゲット
		tracker->read(); //共有メモリからデータを読み出す
		trackHead = tracker->getTrackData( "CAP" ); //VICONマーカの名前
		trackBody = tracker->getTrackData( "Chest" );
		trackHandR = tracker->getTrackData("TREE_A" );
		trackHandL = tracker->getTrackData(  "TREE_B" );
		trackBase = tracker->getTrackData( "Candy" );
		trackFootR = tracker->getTrackData( "RightFoot" );
		trackFootL = tracker->getTrackData( "LeftFoot" );
		
		copyTrackToObj( trackHead, &simdata.head );
		copyTrackToObj( trackBody, &simdata.body );
		copyTrackToObj( trackHandL, &simdata.handL );
		copyTrackToObj( trackHandR, &simdata.handR );
		copyTrackToObj( trackFootL, &simdata.footL );
		copyTrackToObj( trackFootR, &simdata.footR );
		//解説：構造体のポインタを引数とする
		//&: アドレス（＝ポインタ）を渡すことを指定
		simdata.handR.pos.z -= 0.5; //########
		simdata.handL.pos.z -= 0.5; //########
	}
	else{
		//◆01◆コメント化
		/*
		copyTrackToObj( trackHandL, &simdata.handL );
		copyTrackToObj( trackHandR, &simdata.handR );
		*/
		//◆02◆マウスで右手handRを動かす
		//感度調整＋基準位置調整 ########
		simdata.handR.pos.x = mouse.x * 2.0 + 0.25; // mouse.x: -1.0（左端）～1.0（右端）
		simdata.handR.pos.y = -mouse.y * 2.0 + 1.2; // mouse.y; -1.0（上端）～1.0（下端）
		//simdata.handR.z = -2.0;//★コメントに
		//▲
		
		//◆03◆ついでに左手handLも動かす～右手と上下左右を逆にしたりする
		//感度調整＋基準位置調整 ########
		simdata.handL.pos.x = - mouse.x * 2.0 - 0.25; // mouse.x: -1.0（左端）～1.0（右端）
		simdata.handL.pos.y = mouse.y * 2.0 + 1.2; // mouse.y; -1.0（上端）～1.0（下端）
		//simdata.handL.z = -2.0;//★コメントに
		//▲
		
		////★追加
		if (keydata.arrowUp) {
			simdata.handL.pos.z -= 0.01; //###### VECTOR Z
		}
		if (keydata.arrowDown) {
			simdata.handL.pos.z += 0.01; //###### VECTOR Z
		}
		//////////★
	}
	if( use_gyro ){
		float rot[3];
		UpdateGyro( rot );
		setObjRot( &simdata.head, rot ); //////////VECTOR OBJ
		/*
		simdata.head.roll = rot[0];
		simdata.head.pitch = rot[1];
		simdata.head.yaw = rot[2];
		*/
	}
#endif
	/*
	//----------------------------------------------- マウスで移動する
	simdata.player.turn = - 0.5 * mouse.xRel;
	simdata.player.move = - 0.2 * mouse.yRel;
	MoveObject( &simdata.player ); 
	*/

	//★定数として変数を使いたいときには「const」をつける
	const float yon = 1.5, yoff = 1.0;

	//★前の値を保持したいときには「static」をつける
	static float xo, zo;

	//★左手の動作で移動する操作
	switch (simdata.handL.state) {
	case 0: //◆非移動モード
		if (ObjY(&simdata.handL) > yon) {  ////////VECTOR Y
			simdata.handL.state = 1;
			xo = ObjX(&simdata.handL); ////////VECTOR X
			zo = ObjZ(&simdata.handL); ////////VECTOR Z
			setObjColor(&simdata.handL, 1.0, 1.0, 0.0);
		}
		break;
	case 1: //◆移動モード
		if (ObjY(&simdata.handL) < yoff) { ////////VECTOR Y
			simdata.handL.state = 0;
			setObjColor(&simdata.handL, 1.0, 0.0, 0.0);
		}
		//移動の処理
		simdata.player.turn = - 0.5 * (ObjX(&simdata.handL) - xo); ////////VECTOR X
		simdata.player.move = - 0.2 * (ObjZ(&simdata.handL) - zo); ////////VECTOR Z
		MoveObject(&simdata.player);
		break;
	}

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
			moveWorldToLocal( &simdata.sphere, &simdata.handR );

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

    return;
}
