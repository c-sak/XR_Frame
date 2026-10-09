/******************************************************************************
 * VR_Vicon_Project / sim.cpp
 *
 *   Vicon Shogun Live の DataStream から骨格（Subject）を受信し、
 *   骨格の可視化と、従来のプロップトラッキングを行うサンプル。
 *
 *   ■ 設定
 *     ・config.h の use_tracker / use_vicon
 *     ・VICON_HOST（Vicon PC の IPアドレス：ポート番号）
 *
 *   ■ 骨格の可視化
 *     受信した Subject（= ezTracker）への参照を subjects[] にキャッシュし、
 *     DrawTrackedSkeleton() で TrackArray を走査して parent 間を線で結ぶ。
 *     線の色は subject_colors[] で Subject ごとに変える。
 *
 *   ■ プロップトラッキング
 *     従来どおり getTrackData("名前") で CAP / TREE_A / TREE_B / Chest /
 *     Candy / RightFoot / LeftFoot を取得し、simdata のオブジェクトへ反映する。
 *
 *   ■ カメラ
 *     初期状態は固定カメラ。C キーで固定カメラ ⇔ 頭（head）カメラを切り替える。
 ******************************************************************************/

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

// draw.cpp（2D文字）
void drawString(float x, float y, float z, float xscl, float yscl, const char* string);

SimDataT simdata; //SimDataT型構造体のデータを宣言
extern MouseDataT mouse;
extern KeyDataT keydata;

//---- トラッカー ----
ezTracker *tracker = nullptr; //共有メモリ経由でトラッカーの情報を得るオブジェクト
static ezTracker_Vicon *vicon = nullptr; //ViconのSubject取得用
static bool vicon_active = false; //Viconのトラッキングが有効か

//---- 従来のプロップトラッキング（元のsim.cppと同じ） ----
//トラッカーから受け取ったデータへのポインタ
ezTrackDataT *trackBase = nullptr; //基準マーカ
ezTrackDataT *trackHead = nullptr;
ezTrackDataT *trackBody = nullptr;
ezTrackDataT *trackHandR = nullptr;
ezTrackDataT *trackHandL = nullptr;
ezTrackDataT *trackFootR = nullptr;
ezTrackDataT *trackFootL = nullptr;
//マーカが見えない場合などのダミーデータ{id,x,y,z,roll,pitch,yaw,name,parent}
ezTrackDataT localBase = { 0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, "", -1 };
ezTrackDataT localHead = { 0, 0.0, 1.5, 0.0, 0.0, 0.0, 0.0, "", -1 };
ezTrackDataT localBody = { 0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, "", -1 };
ezTrackDataT localHandR = { 0, 0.25, 1.25, -0.5, 0.0, 0.0, 0.0, "", -1 };
ezTrackDataT localHandL = { 0,-0.25, 1.25, -0.5, 0.0, 0.0, 0.0, "", -1 };
ezTrackDataT localFootR = { 0, 0.25, 0.0, -0.5, 0.0, 0.0, 0.0, "", -1 };
ezTrackDataT localFootL = { 0,-0.25, 0.0, -0.5, 0.0, 0.0, 0.0, "", -1 };

//---- 骨格表示（Subject単位） ----
#define VICON_MAX_SUBJECTS 8
static ezTracker *subjects[VICON_MAX_SUBJECTS]; //Subject（=ezTracker）への参照
static int subject_count = 0;
//Subjectごとの骨格線の色
static const float subject_colors[VICON_MAX_SUBJECTS][3] = {
	{ 0.00f, 1.00f, 0.25f }, //緑
	{ 0.20f, 0.60f, 1.00f }, //青
	{ 1.00f, 0.20f, 0.20f }, //赤
	{ 1.00f, 1.00f, 0.00f }, //黄
	{ 1.00f, 0.30f, 1.00f }, //マゼンタ
	{ 0.00f, 1.00f, 1.00f }, //シアン
	{ 1.00f, 0.60f, 0.00f }, //オレンジ
	{ 1.00f, 1.00f, 1.00f }, //白
};

//---- 確認用の固定カメラ ----
static ObjDataT fixed_camera;

//Vicon PCのIPアドレス：ポート番号
static char VICON_HOST[] = "127.0.0.1:801";
//static char VICON_HOST[] = "172.23.85.186:801";

////////////////////////////////////////////////////////

/*------------------------------------------------------------- copyTrackToObj
 * トラッカーから受け取った1つのデータを ObjDataT へコピーする
 * 戻り値：コピーできたら true（NULL・遮蔽などで無効なときは false）
 */
static bool copyTrackToObj( ezTrackDataT *src, ObjDataT *dst )
{
	float pos[3], rot[3];
	if( src == NULL ) return false;
	ezTrack_getPos( src, pos );
	ezTrack_getRot( src, rot );
	//遮蔽フレームは回転がNaNになることがある（NaNをコピーするとライトや変換が壊れる）
	if( pos[0] != pos[0] || pos[1] != pos[1] || pos[2] != pos[2] ||
	    rot[0] != rot[0] || rot[1] != rot[1] || rot[2] != rot[2] ) return false;
	setObjPos( dst, pos );
	setObjRot( dst, rot );
	return true;
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
/*------------------------------------------------------------- printViconSubject
 * 指定したSubjectの骨（Segment）一覧をコンソールに出力する
 * 例：printViconSubject("Subject1");
 */
void printViconSubject( const char *name )
{
	if( vicon == NULL ) return;
	ezTracker *body = vicon->getSubject( name );
	if( body == NULL ) return;

	ezTrackArrayT *array = body->getTrackArray();
	printf("---- %s ----\n", name);
	for( int i = 0; i < _n_tracks; i++ ){
		ezTrackDataT *track = &array->data[i];
		if( track->id == -1 ) continue;
		printf("  [%2d] %-24s parent=%2d pos=(%7.3f,%7.3f,%7.3f)m rot=(%7.1f,%7.1f,%7.1f)deg\n",
			i, track->name, track->parent,
			track->x, track->y, track->z,
			track->roll, track->pitch, track->yaw);
	}
}
/*---------------------------------------------------------------- InitScene
 * InitScene:
 *--------*/
void InitScene( void )
{
	printf(">>InitScene\n");

#ifndef MREALMODE
	//- Viconを使用する場合、ezTracker_Viconクラスを使用する:Crescent
	if (use_vicon) {
		vicon = new ezTracker_Vicon(use_tracker); //VICON使うときはtrue
		tracker = vicon;
		// 引数の文字列は、"VICON PCのIPアドレス：通信ポート番号(デフォルト：801)"
		tracker->open(VICON_HOST, false);
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
	setObjPos( &simdata.cube, 0.0, 1.0, -50.0 );
	setObjRot( &simdata.cube, 0.0, 0.0, 60.0 );
    setObjColor( &simdata.cube, 0.5, 0.3, 0.2 );
	simdata.cube.visible = true;
	simdata.cube.state = 0; //////////////◆
    simdata.cube.radius = 0.2;

	simdata.cube.xsize = 8.0; //0.6
	simdata.cube.ysize = 2.0; //0.05
	simdata.cube.zsize = 0.05; //0.25

	setObjPos( &simdata.sphere, 0.0, 1.2, -1.0 );
	setObjRot( &simdata.sphere, 0.0, 0.0, 0.0 );
	setObjColor( &simdata.sphere, 1.0, 0.5, 0.0 );
	simdata.sphere.visible = true;
	simdata.sphere.state = 0;
	simdata.sphere.radius = 0.25; //★◆04

	simdata.handR.radius = 0.125; //◆04
	simdata.handL.radius = 0.125; //◆04
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
	
	setObjPos(&simdata.head, 0.0, 1.6, 0.0);
	setObjRot(&simdata.head, 0.0, 0.0, 0.0);
	setObjColor(&simdata.head, 0.0, 0.5, 1.0);
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

	//確認用の固定カメラ（Cキーで頭カメラと切り替え）
	setObjPos( &fixed_camera, 0.0, 1.6, 4.0 );
	setObjRot( &fixed_camera, 0.0, 0.0, 0.0 );

	//初期状態は固定カメラ（Cキーで頭カメラに切り替え）
	simdata.active_camera = &fixed_camera;

	//simdata.active_camera = NULL;
	//プレイヤオブジェクトのアドレスをカメラのポインタに紐付ける

	setObjColor( &simdata.handR, 0.0, 1.0, 0.0 ); //右手グリーン
	setObjColor( &simdata.handL, 1.0, 0.0, 0.0 ); //左手レッド

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
		//解説：構造体のポインタを引数とする
		//&: アドレス（＝ポインタ）を渡すことを指定
		if (copyTrackToObj(trackHandL, &simdata.handL)) simdata.handL.pos.z -= 0.5; //########
		if (copyTrackToObj(trackHandR, &simdata.handR)) simdata.handR.pos.z -= 0.5; //########
		copyTrackToObj(trackFootL, &simdata.footL);
		copyTrackToObj(trackFootR, &simdata.footR);

		//ViconのSubject（骨格・プロップ）への参照をキャッシュする
		subject_count = 0;
		if (vicon != NULL) {
			int n = vicon->getSubjectCount();
			if (n > VICON_MAX_SUBJECTS) n = VICON_MAX_SUBJECTS;
			for (int i = 0; i < n; i++) {
				subjects[i] = vicon->getSubject(i);
			}
			subject_count = n;
		}
		vicon_active = (subject_count > 0);
	}

	//Viconが使えないときはマウスで操作（動作確認用）
	if (!vicon_active) {
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
		if ( simdata.handL.pos.y > yon) {  // simdata.handL.pos.y
			simdata.handL.state = 1;
			xo = simdata.handL.pos.x; // simdata.handL.pos.x
			zo = simdata.handL.pos.z; // simdata.handL.pos.z
			//setObjColor(&simdata.handL, 1.0, 1.0, 0.0);
		}
		break;
	case 1: //◆移動モード
		if ( simdata.handL.pos.y < yoff) { // simdata.handL.pos.y
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

	//Cキーで固定カメラ ⇔ 頭（head）カメラを切り替え
	static bool prev_c = false;
	if (keydata.charKey['c'] && !prev_c) {
		if (simdata.active_camera == &simdata.head) {
			simdata.active_camera = &fixed_camera;
		}
		else {
			simdata.active_camera = &simdata.head;
		}
	}
	prev_c = keydata.charKey['c'];

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

	if (isHitBox(&simdata.cube, &simdata.handL)) {
		setObjColor(&simdata.cube, 1.0, 0.0, 0.0);
		////simdata.movie->play(simdata.time);
	}
	else {
		setObjColor(&simdata.cube, 0.0, 1.0, 0.0);
	}

    return;
}

/*------------------------------------------------------------- DrawTrackedSkeleton
 * 受信した各Subjectの骨格を描画する
 *   SubjectのezTrackerが持つTrackArrayを走査し、parentが有効な骨同士を線で結ぶ
 *   draw.cppのDrawScene()から呼ばれる
 */
void DrawTrackedSkeleton( void )
{
	if( subject_count <= 0 ) return;

	//この関数内で変更したGL状態を元に戻す
	glPushAttrib( GL_ENABLE_BIT | GL_LIGHTING_BIT | GL_CURRENT_BIT | GL_LINE_BIT );

	//---- 骨格の線（Subjectごとの色）----
	glDisable( GL_LIGHTING );
	glLineWidth( 3.0f );
	for( int s = 0; s < subject_count; s++ ){
		ezTracker *body = subjects[s];
		if( body == NULL ) continue;
		ezTrackArrayT *array = body->getTrackArray();

		glColor3f( subject_colors[s][0], subject_colors[s][1], subject_colors[s][2] );
		glBegin( GL_LINES );
		for( int i = 0; i < _n_tracks; i++ ){
			ezTrackDataT *track = &array->data[i];
			if( track->id == -1 ) continue;
			if( track->parent < 0 || track->parent >= _n_tracks ) continue;
			ezTrackDataT *parent = &array->data[track->parent];
			if( parent->id == -1 ) continue;
			glVertex3f( track->x, track->y, track->z );
			glVertex3f( parent->x, parent->y, parent->z );
		}
		glEnd();
	}
	glLineWidth( 1.0f );

	//---- 関節（オレンジのキューブ、回転を適用）----
	glEnable( GL_LIGHTING );
	glEnable( GL_COLOR_MATERIAL );
	glColorMaterial( GL_FRONT, GL_AMBIENT_AND_DIFFUSE );
	for( int s = 0; s < subject_count; s++ ){
		ezTracker *body = subjects[s];
		if( body == NULL ) continue;
		ezTrackArrayT *array = body->getTrackArray();

		for( int i = 0; i < _n_tracks; i++ ){
			ezTrackDataT *track = &array->data[i];
			if( track->id == -1 ) continue;

			glPushMatrix();
			glTranslatef( track->x, track->y, track->z );
			glRotatef( track->yaw,   0.0f, 1.0f, 0.0f );
			glRotatef( track->pitch, 1.0f, 0.0f, 0.0f );
			glRotatef( track->roll,  0.0f, 0.0f, 1.0f );

			glColor3f( 1.0f, 0.5f, 0.0f );
			if( track->parent < 0 ){
				//ルートには姿勢が分かるように座標軸も描く
				glDisable( GL_LIGHTING );
				glBegin( GL_LINES );
				glColor3f(1.0f,0.0f,0.0f); glVertex3f(0.0f,0.0f,0.0f); glVertex3f(0.25f,0.0f,0.0f);
				glColor3f(0.0f,1.0f,0.0f); glVertex3f(0.0f,0.0f,0.0f); glVertex3f(0.0f,0.25f,0.0f);
				glColor3f(0.2f,0.4f,1.0f); glVertex3f(0.0f,0.0f,0.0f); glVertex3f(0.0f,0.0f,0.25f);
				glEnd();
				glEnable( GL_LIGHTING );
				glutSolidCube( 0.05 );
			}
			else{
				glutSolidCube( 0.03 );
			}
			glPopMatrix();
		}
	}

	glPopAttrib();
}

/*------------------------------------------------------------- DrawTrackingInfo
 * トラッキング中のSubject名とボーン数を画面に表示する
 *   draw.cppのPostDraw()から呼ばれる（2D表示、0.0～1.0の座標）
 */
void DrawTrackingInfo( void )
{
	if( vicon == NULL ) return;

	char msg[128];
	float y = 0.90f;
	const int count = vicon->getSubjectCount();
	for( int s = 0; s < count && s < VICON_MAX_SUBJECTS; s++ ){
		const char *name = vicon->getSubjectName( s );
		ezTracker *body = vicon->getSubject( s );
		if( body == NULL ) continue;

		int bones = 0;
		ezTrackArrayT *array = body->getTrackArray();
		for( int i = 0; i < _n_tracks; i++ ){
			if( array->data[i].id != -1 ) bones++;
		}

		sprintf( msg, "%s : %d bones", name, bones );
		glColor3f( 0.1f, 0.2f, 0.0f );
		drawString( 0.02, y, 0.0, 0.2, 0.4, msg );
		y -= 0.07f;
	}
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
