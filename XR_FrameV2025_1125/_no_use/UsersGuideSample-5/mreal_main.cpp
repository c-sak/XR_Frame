// UsersGuideSample-5.cpp

#include "stdafx.h"
#include "UsersGuideSample.h"
#include <MrHarmonizer.h>

#include "sim.h"
#include "light.h"
extern SimDataT simdata;

#define METER_UNIT

// MREALiVR
void renderTarget(IMrhTarget *_pTarget, int id);
void copyTarget(IMrhTarget *_pTarget, int id, bool detected);
void copyMarker(IMrhMarker *_pMarker, int id, bool detected, int targetID);
void copyCamera(IMrhCamera *camera, TargetT *target, bool detected);

// 定数・変数定義
DWORD winX = 0;
DWORD winY = 0;
DWORD winW = 1280;								// ウィンドウの幅
DWORD winH = 960;								// ウィンドウの高さ

struct {
	DWORD x;
	DWORD y;
}dispOrigin[2];									// １画面を表示する原点

struct {
	DWORD width;
	DWORD height;
}dispSize[2];									// １画面を表示するサイズ

// MREAL Platform関連変数
IMrhEngine53			*pEngine;				// エンジン
IMrhCamera				*pCamera[2];			// カメラ
IMrhDisplayInfo			*pDispInfo;				// 表示設定
IMrhRenderer53			*pRender[2];			// 描画API
IMrhDepth53* depth = 0;							// 奥行き計測結果インターフェイス
IMrhPolygon53* polygon[2] = {};					// 手の奥行き計測結果のポリゴン

IMrhTarget				*pTarget[15];			// ターゲット

IMrhMarker              **vpMarkers;         // マーカ byTatewaki20170712

HANDLE					hEvent = NULL; // エンジン更新イベント
static DWORD			dwNumOfCameras = 0;	// カメラの数
static DWORD			dwNumOfTarget = 0;	// ターゲットの数
static DWORD            dwNumOfMarker = 0;    // マーカの数 byTatewaki20170712

GLUquadricObj			*pFillObj = NULL; // 仮想物体描画に使用
static HDC				hDC = NULL;	// デバイスコンテキスト
static HGLRC			hRC = NULL;	// レンダリングコンテキスト
static GLuint			bmpListID = 0;	// ディスプレイ・リストID

static tagMrhColorRegionMaskMethod maskMethod = MRH_COLOR_REGION_METHOD_NONE; // マスク画像描画方法

static DWORD regStatus;

//===========================================================================//
// WinMain関数
//===========================================================================//
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst,
	LPSTR lpszCmdLine, int nCmdShow) {
	// MREAL Platform関連インスタンスの初期設定
	if (!initMrPlatform())
		return 0;

	// ウィンドウクラスの登録
	char className[] = "UsersGuideSample";
	if (!InitApp(hInstance, className)) {
		MessageBox(NULL, "Cannot Register WNDCLASSEX\n", "ERROR", MB_ICONERROR);
		return 0;
	}
	// ウィンドウの作成
	HWND hWnd = NULL;
	if ((hWnd = InitInstance(hInstance, className, nCmdShow)) == NULL) {
		MessageBox(NULL, "Cannot Create Window\n", "ERROR", MB_ICONERROR);
		return 0;
	}

	MSG msg;
	BOOL bEndFlg = FALSE;
	while (!bEndFlg) {
		// エンジンの状態変化イベントかキーイベントを取得するまで待機
		if (::MsgWaitForMultipleObjects(1, &hEvent, FALSE, INFINITE, QS_ALLINPUT) == 0) {
			HRESULT hr;

			// エンジンのパラメーターを更新
			hr = pEngine->Update();
			if (FAILED(hr)) {
				MessageBox(NULL, "IMrhEngine53::Update() FAILED.", "ERROR", MB_ICONERROR);
				DestroyWindow(hWnd);
			}

			pDispInfo->GetWindowRect(&winX, &winY, &winW, &winH);
			RECT rc = { winX, winY, winX + winW, winY + winH };
			::AdjustWindowRectEx(&rc, ::GetWindowLong(hWnd, GWL_STYLE), ::GetMenu(hWnd) != NULL, ::GetWindowLong(hWnd, GWL_EXSTYLE));
			::SetWindowPos(hWnd, NULL, winX, winY, rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER);

			// マスターカメラがあるかどうか確認
			dwNumOfCameras = 0;
			BOOL isExist = FALSE;
			pEngine->QueryCamera(MRH_CAMERA_MASTER, &isExist);
			if (isExist) {

				pDispInfo->GetViewportRect(MRH_CAMERA_MASTER, &dispOrigin[0].x, &dispOrigin[0].y, &dispSize[0].width, &dispSize[0].height);
				dwNumOfCameras++;
			}
			// スレーブカメラがあるかどうか確認
			// Configuration Toolで単眼に設定した際にも動作するように
			// 毎フレーム問い合わせる
			pEngine->QueryCamera(MRH_CAMERA_SLAVE, &isExist);
			if (isExist) {

				pDispInfo->GetViewportRect(MRH_CAMERA_SLAVE, &dispOrigin[1].x, &dispOrigin[1].y, &dispSize[1].width, &dispSize[1].height);
				dwNumOfCameras++;
			}

			// 実写画像及び仮想物体の描画
			dispFunc();
			SwapBuffers(hDC);

		}
		else {
			// メッセージキューからキーイベントを取得
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
				if (msg.message == WM_QUIT) {
					bEndFlg = TRUE;
					break;
				}
				// キーイベントを文字メッセージへ変換
				TranslateMessage(&msg);
				// 文字メッセージを適切なウィンドウプロシージャに転送
				DispatchMessage(&msg);
			}
		}
	}
	// MREAL Platform関連インスタンスの終了処理
	releaseMrPlatform();
	return (int)msg.wParam;
}

//===========================================================================//
// MREAL Platform関連初期設定
//===========================================================================//
BOOL initMrPlatform() {
	// COM初期化
	// MREAL Platform APIはマルチスレッドアパートメント（MTA)には非対応
	// なのでCoInitializeEx()は使用しないこと
	::CoInitialize(NULL);

	// エンジンインスタンスを作成
	HRESULT hr = S_OK;
	hr = CoCreateInstance(CLSID_CMrhEngine53, NULL, CLSCTX_INPROC, IID_IMrhEngine53, (LPVOID*)&pEngine);


	if (FAILED(hr)) {
		MessageBox(NULL, "Failed in creating a instance of the MREAL Engine.", "Failed in starting an MREAL Engine", MB_OK | MB_ICONERROR);
		return FALSE;
	}

	// エンジン初期化
	hr = pEngine->Initialize();
	if (FAILED(hr)) {
		MessageBox(NULL, "Failed in initializing a instance of the MREAL Engine.", "Failed in initializing an MREAL Engine", MB_OK | MB_ICONERROR);
		return FALSE;
	}
	// 更新イベントの生成
	hEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	// エンジンの状態が変化したときのイベントを指定
	pEngine->SetEventNotification(hEvent);

	// 表示設定の情報を問い合わせるインスタンスを作成
	if (FAILED(pEngine->QueryInterface(IID_IMrhDisplayInfo, (void **)&pDispInfo))) {
		MessageBox(NULL, "Failed in creating a instance of the DisplayInfo.", "Failed in getting a DisplayInfo", MB_OK | MB_ICONERROR);
		return FALSE;
	}

	// マスターカメラがあるかどうか確認
	dwNumOfCameras = 0;
	BOOL isExist = FALSE;
	pEngine->QueryCamera(MRH_CAMERA_MASTER, &isExist);
	if (isExist) {
		// マスターカメラインスタンスを作成
		pEngine->GetCamera(MRH_CAMERA_MASTER, &pCamera[dwNumOfCameras]);
		pEngine->GetRenderer53(MRH_CAMERA_MASTER, &pRender[dwNumOfCameras]);
		dwNumOfCameras++;
	}
	// スレーブカメラがあるかどうか確認
	pEngine->QueryCamera(MRH_CAMERA_SLAVE, &isExist);
	if (isExist) {
		// スレーブカメラインスタンスを作成
		pEngine->GetCamera(MRH_CAMERA_SLAVE, &pCamera[dwNumOfCameras]);
		pEngine->GetRenderer53(MRH_CAMERA_SLAVE, &pRender[dwNumOfCameras]);
		dwNumOfCameras++;
	}

	// スレーブカメラのインターフェイスが存在する場合にステレオ計測を行い
	//検出した色領域のポリゴンを作成する．このとき，左右でステレオ計測対象の
	//形状が異なるため，右眼，左眼用のインターフェイスをそれぞれ取得する．
	if (dwNumOfCameras == 2) {
		maskMethod = MRH_COLOR_REGION_METHOD_POLYGON;

		// 色領域のステレオ計測に関する設定
		BOOL exist = FALSE;
		HRESULT res;
		// 色領域のステレオ計測の結果を取得するインターフェイスを取得
		res = pEngine->QueryDepth(MRH_DEPTH_COLOR_REGION, &exist);
		assert(res == S_OK && exist);
		res = pEngine->GetDepth53(MRH_DEPTH_COLOR_REGION, &depth);
		assert(res == S_OK && depth);

		const DWORD camera[] = { MRH_CAMERA_MASTER, MRH_CAMERA_SLAVE };
		for (DWORD i = 0; i < dwNumOfCameras; ++i) {
			// ポリゴンインターフェイスを取得
			res = depth->QueryPolygon(camera[i], &exist);
			assert(res == S_OK && exist);
			res = depth->GetPolygon53(camera[i], &polygon[i]);
			assert(res == S_OK && polygon[i]);
		}
	}

	for (DWORD i = 0; i < dwNumOfCameras; ++i) {
		// マーカーキャンセラーの初期設定
		pRender[i]->SetMarkerCancelObject(MRH_MARKER_CANCEL_OBJECT_DETECTION);
		// マスク画像描画方法の初期設定
		pRender[i]->SetColorRegionMaskMethod(maskMethod);
	}

	// ターゲットの数を取得
	pEngine->GetNumOfTarget(&dwNumOfTarget);
	assert(S_OK == pEngine->GetNumOfMarker(&dwNumOfMarker)); // byTatewaki20170712

	//////// MessageBoxテスト用
	/*
	if( dwNumOfMarker == 13 ){
		MessageBox( NULL, "13 markers", "OK", MB_OK|MB_ICONINFORMATION );
	}
	else{
		MessageBox( NULL, "not 13 markers", "NG", MB_OK|MB_ICONERROR );
	}
	*/
	// ターゲットインスタンスを作成
	const DWORD targetObj[] = {
		MRH_TARGET_1, MRH_TARGET_2, MRH_TARGET_3,
		MRH_TARGET_4, MRH_TARGET_5, MRH_TARGET_6,
		MRH_TARGET_7, MRH_TARGET_8, MRH_TARGET_9,
		MRH_TARGET_10, MRH_TARGET_11, MRH_TARGET_12,
		MRH_TARGET_13, MRH_TARGET_14, MRH_TARGET_15
	};
	for (DWORD i = 0; i < dwNumOfTarget; ++i) {
		const HRESULT res = pEngine->GetTarget(targetObj[i], &pTarget[i]);
		assert(res == S_OK);
	}

	// マーカインスタンスを作成
	/*
	const int markerObj[] = {
		//199,200,201,202,203,204,205,206,207,208,209,210,211
		0,1,2,3,4,5,6,7,8,9,10,11,12
	};
	*/
	vpMarkers = new IMrhMarker*[dwNumOfMarker];

	for (DWORD i = 0; i < dwNumOfMarker; ++i) {
		IMrhMarker *cpMarker;
		HRESULT res = pEngine->GetMarker(i, &cpMarker);
		vpMarkers[i] = cpMarker;
		assert(res == S_OK);
	}

	return TRUE;
}

//===========================================================================//
// ウィンドウクラスの登録
//===========================================================================//
BOOL InitApp(HINSTANCE hInstance, LPSTR className) {
	WNDCLASSEX wndclsex;

	wndclsex.cbSize = sizeof(WNDCLASSEX);

	wndclsex.style = CS_HREDRAW | CS_VREDRAW;					// スタイル
	wndclsex.lpfnWndProc = WndProc;									// ウィンドウプロシージャの名前
	wndclsex.cbClsExtra = 0;										// メモリ領域の追加設定
	wndclsex.cbWndExtra = 0;										// メモリ領域の追加設定
	wndclsex.hInstance = hInstance;								// プログラムハンドル
	wndclsex.hIcon = LoadIcon(hInstance, 						// アイコン
		MAKEINTRESOURCE(IDI_GUIDESAMPLE1));
	wndclsex.hCursor = LoadCursor(NULL, IDC_ARROW);			// カーソル
	wndclsex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);	// ブラシ
	wndclsex.lpszMenuName = NULL;										// メニュー
	wndclsex.lpszClassName = className;
	wndclsex.hIconSm = LoadIcon(wndclsex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wndclsex);
}

//===========================================================================//
// ウィンドウプロシージャ
//===========================================================================//
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
		// ウィンドウ生成メッセージ -------------------------------------------
	case WM_CREATE:
		// OpenGLの初期設定
		initOpenGL(hWnd);
		break;
		// ウィンドウ破棄メッセージ -------------------------------------------
	case WM_DESTROY:
		// OpenGL関連の終了処理
		finalizeOpenGL(hWnd);
		// メッセージキューに終了メッセージを送付
		PostQuitMessage(0);
		break;
		// 描画メッセージ -----------------------------------------------------
	case WM_PAINT:
		dispFunc();
		return DefWindowProc(hWnd, msg, wParam, lParam);
		// キーイベント -------------------------------------------------------
	case WM_CHAR:
		keyPress(hWnd, wParam);
		break;
		// メッセージのデフォルト処理 -----------------------------------------
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

//===========================================================================//
// キーイベント
//===========================================================================//
void keyPress(HWND hWnd, WPARAM wParam) {
	switch (wParam) {
	case 27:	// プログラム終了(Escキー)
	case 'q':
		DestroyWindow(hWnd);
		break;
	}
}

//===========================================================================//
// ウィンドウの作成
//===========================================================================//
HWND InitInstance(HINSTANCE hInstance, LPSTR className, int nCmdShow) {

	// ウィンドウの作成
	HWND hWnd = CreateWindow(className,	// クラス名
		"UsersGuideSample",						// ウィンドウの名前
		WS_POPUP,							// 枠なしのウインドウ	
		0,				 					// X座標
		0,				 					// Y座標
		winW,								// 幅
		winH,								// 高さ
		NULL,								// 親ウィンドウハンドル
		NULL,								// メニューハンドル
		hInstance,							// インスタンスハンドル
		NULL 								// ウィンドウ作成データ
	);
	if (!hWnd) return NULL;
	// ウィンドウの表示
	ShowWindow(hWnd, nCmdShow);
	// 再描画を指示
	UpdateWindow(hWnd);
	return hWnd;
}

//===========================================================================//
// OpenGL関連初期設定
//===========================================================================//
void initOpenGL(HWND hWnd) {
	// ウィンドウクリア時の色指定
	glClearColor(0, 0, 0, 1);

	// 2次曲面オブジェクトを生成
	pFillObj = gluNewQuadric();
	gluQuadricDrawStyle(pFillObj, GLU_FILL);

	// 光源を設定
	setLight();

	// デバイスコンテキストの取得
	hDC = GetDC(hWnd);
	// ピクセルフォーマットの設定
	if (!setPixelFormat(hDC)) {
		MessageBox(hWnd, "Cannot set PixelFormat", "ERROR", MB_ICONERROR);
		finalizeOpenGL(hWnd);
		DestroyWindow(hWnd);
	}
	// レンダリングコンテキストを作成
	hRC = wglCreateContext(hDC);
	// レンダリングコンテキストをカレントに設定
	wglMakeCurrent(hDC, hRC);

	InitScene();
}

//===========================================================================//
// 仮想物体描画のための光源の設定
//===========================================================================//
void setLight(void) {
	// ライトの設定
	// ライト０番の光源設定
	static GLfloat lightAmbient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	static GLfloat lightDiffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	static GLfloat lightPosition[] = { 0.0f, 1.0f, 0.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	// 光源モデルの設定
	static float lmodelAmbient[] = { 0.4f, 0.4f, 0.4f, 1.0f };
	static float lmodelLocalviewer[] = { 0.0f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodelAmbient);
	glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, lmodelLocalviewer);
}

//===========================================================================//
//  ピクセルフォーマットを設定
//===========================================================================//
BOOL setPixelFormat(HDC hdc) {
	// ピクセルフォーマットの設定
	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));

	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL |
		PFD_GENERIC_FORMAT |
		PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cStencilBits = 1;
	pfd.cColorBits = 24;
	pfd.cDepthBits = 32;
	pfd.iLayerType = PFD_MAIN_PLANE;            // Layer type

	// デバイスコンテキストにピクセルフォーマットを設定
	int pixelFormat = ChoosePixelFormat(hdc, &pfd);
	if (pixelFormat == 0) return FALSE;
	return SetPixelFormat(hdc, pixelFormat, &pfd);
}

//===========================================================================//
//  描画関数
//===========================================================================//
void dispFunc() {
	// 画面クリア
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	//フラスタムパラメーター取得変数
#ifdef METER_UNIT
	double l, r, b, t, n = 0.1, f = 10;
#else
	double l, r, b, t, n = 10, f = 10000;
#endif

	//ターゲット情報の獲得
	//カメラの位置姿勢が分からない場合でも
	//ターゲットのデータをsimdataのオブジェクトデータにコピー
	for (unsigned int j = 0; j < dwNumOfTarget; j++) {
		if (pTarget[j]->GetRegistrationStatus(&regStatus) == S_OK &&
			regStatus == MRH_REGISTRATION_RESULT_OK) {
			//renderTarget( pTarget[j], j );
			copyTarget(pTarget[j], j, true); //★
		}
		else {
			copyTarget(pTarget[j], j, false); //★
		}
	}
	//マーカー情報の獲得ターゲットを表示する．
	//カメラの位置姿勢が分からない場合でも，
	//マーカーのデータをsimdataのオブジェクトデータにコピー
	BYTE byteStatus;
	for (unsigned int j = 0; j < dwNumOfMarker; j++) {
		short targetID;
		vpMarkers[j]->GetTargetId(&targetID);
		if (vpMarkers[j]->GetStatus(&byteStatus, MRH_CAMERA_MASTER) == S_OK &&
			byteStatus == MRH_MREAL_MARKER_STATUS_DETECTED)
		{
			copyMarker(vpMarkers[j], j, true, targetID - 1); //★
		}
		else
		{
			copyMarker(vpMarkers[j], j, false, targetID - 1); //★
		}
	}

	/////////////
	UpdateScene();
	/////////////
	glEnable(GL_NORMALIZE); //スケールしたときの法線ベクトルの単位ベクトル化
	for (unsigned int i = 0; i < dwNumOfCameras; i++) {
		// ビューポートの設定
		glViewport(dispOrigin[i].x, dispOrigin[i].y, dispSize[i].width, dispSize[i].height);

		// 実写画像の表示
		pRender[i]->SetDispPosition(dispOrigin[i].x, dispOrigin[i].y);
		pRender[i]->SetDispSize(dispSize[i].width, dispSize[i].height);
		pRender[i]->SetNearFar(n, f);
		pRender[i]->Render();

		// 透視投影行列の設定
		glMatrixMode(GL_PROJECTION);
		// IMrhCameraからフラスタムパラメーターを取得
		pCamera[i]->GetFrustum(&l, &r, &b, &t, n);
		// 取得したフラスタムパラメーターをglFrustumへ設定することで, 現在表示している
		// キャプチャー画像に適合した透視投影設定で仮想物体を描画することが
		// 可能になる．
		glLoadIdentity();
		glFrustum(l, r, b, t, n, f);

		// モデルビュー変換行列の設定
		glMatrixMode(GL_MODELVIEW);

		// IMrhCameraからカメラの世界座標系上での位置姿勢を取得
		GLfloat position[3] = { 0.0, 0.0, 0.0 };
		GLfloat axis[3] = { 0.0, 0.0, 0.0 };
		GLfloat angle = 0.0;

		// 取得した位置姿勢を設定することで, 現在表示している現実空間に
		// 適合した位置に, 仮想物体を描画することが可能になる．
		//カメラの位置姿勢が初期位置（原点）のままだとCGが現実映像を覆ってしまい，
		//現実映像が映っていないと間違える可能性がある．そのため，カメラの
		//位置姿勢が取得できてからCGを描画する．
		//カメラ位置姿勢の取得フラグ
		DWORD regResult;
		//マスターカメラの位置姿勢が取得できているか確認
		pCamera[0]->GetRegistrationStatus(&regResult);

		pCamera[i]->GetPosition(&position[0], &position[1], &position[2]); 
		pCamera[i]->GetOrientation(&axis[0], &axis[1], &axis[2], &angle);

		if (regResult == MRH_REGISTRATION_RESULT_OK) {
			////2021.08.02 T.OHSHIMA
			//マスターカメラの位置姿勢が取得できている場合の処理
			/*
			simdata.mrealCamera[i].pos.x = position[0] * 0.001;
			simdata.mrealCamera[i].pos.y = position[1] * 0.001;
			simdata.mrealCamera[i].pos.z = position[2] * 0.001;
			simdata.mrealCamera[i].ori.angle * 180.0 / M_PI;
			simdata.mrealCamera[i].ori.x = axis[0];
			simdata.mrealCamera[i].ori.y = axis[1];
			simdata.mrealCamera[i].ori.z = axis[2];
			*/
			copyCamera( pCamera[i], &simdata.mrealCamera[i], true );
			////////////////////////////////////////////////////
		}
		else{
			copyCamera( pCamera[i], &simdata.mrealCamera[i], false );
		}

		glLoadIdentity();

		glRotatef((GLfloat)(-angle * 180. / M_PI),
				(GLfloat)axis[0], (GLfloat)axis[1], (GLfloat)axis[2]);
#ifdef METER_UNIT
		glTranslatef(-position[0] * 0.001, -position[1] * 0.001, -position[2] * 0.001);
#else
		glTranslatef(-position[0], -position[1], -position[2]);
#endif

		glPushMatrix();
#ifndef METER_UNIT
		glScalef(1000.0, 1000.0, 1000.0);
#endif
		//setLight();//グローバルライト20170613 ohshima
		glEnable(GL_LIGHTING);
		//glEnable(GL_LIGHT0);
		//glEnable(GL_LIGHT1);
		//glEnable(GL_LIGHT2);
		HeadLight(); //20210802 GL_LIGHT2
		Lighting();  //20210802 GL_LIGHT0, GL_LIGHT1

		DrawScene();

		glPopMatrix();

		glDisable(GL_LIGHTING);
		PostDraw();
	}
	glFlush();
}
void copyCamera(IMrhCamera *camera, TargetT *target, bool detected)
{
	GLfloat position[3] = { 0.0, 0.0, 0.0 };
	GLfloat axis[3] = { 0.0, 0.0, 0.0 };
	GLfloat angle = 0.0;

	if (!detected) {
		if (target->detected) target->state = -1;
		else target->state = 0;
		target->detected = false;
		return;
	}
	else {
		if (target->detected) target->state = 0;
		else target->state = 1;
		target->detected = true;
	}

	camera->GetPosition(&position[0], &position[1], &position[2]);
	camera->GetOrientation(&axis[0], &axis[1], &axis[2], &angle);

	target->pos.x = position[0] * 0.001;
	target->pos.y = position[1] * 0.001;
	target->pos.z = position[2] * 0.001;

	target->ori.x = axis[0];
	target->ori.y = axis[1];
	target->ori.z = axis[2];
	target->ori.angle = angle * 180.0 / M_PI;
	return;
}
void copyTarget(IMrhTarget *_pTarget, int id, bool detected)
{
	GLfloat position[3] = { 0.0, 0.0, 0.0 };
	GLfloat axis[3] = { 0.0, 0.0, 0.0 };
	GLfloat angle = 0.0;

	if (!detected) {
		if (simdata.TargetList[id].detected)
		{
			//前回見えていたが見えなくなった
			simdata.TargetList[id].state = -1;
		}
		else
		{
			//見えていないまま
			simdata.TargetList[id].state = 0;
		}
		simdata.TargetList[id].detected = false;
		//見えていない場合には前回の値を保持したまま戻る
		return;
	}
	else {
		if (simdata.TargetList[id].detected)
		{
			//見えたまま
			simdata.TargetList[id].state = 0;
		}
		else
		{
			//前回見えていなかったが見つかった
			simdata.TargetList[id].state = 1;
		}
		simdata.TargetList[id].detected = true;
	}

	_pTarget->GetPosition(&position[0], &position[1], &position[2]);
	_pTarget->GetOrientation(&axis[0], &axis[1], &axis[2], &angle);

	simdata.TargetList[id].pos.x = position[0] * 0.001;
	simdata.TargetList[id].pos.y = position[1] * 0.001;
	simdata.TargetList[id].pos.z = position[2] * 0.001;

	simdata.TargetList[id].ori.x = axis[0];
	simdata.TargetList[id].ori.y = axis[1];
	simdata.TargetList[id].ori.z = axis[2];
	simdata.TargetList[id].ori.angle = angle * 180.0 / M_PI;
	return;
}

void copyMarker(IMrhMarker *_pMarker, int id, bool detected, int targetID)
{
	GLfloat position[3] = { 0.0, 0.0, 0.0 };
	GLfloat axis[3] = { 0.0, 0.0, 0.0 };
	GLfloat angle = 0.0;

	DWORD mid;

	simdata.MarkerList[id].targetID = targetID;
	if (!detected) {

		if (simdata.MarkerList[id].detected)
			simdata.MarkerList[id].state = -1;
		else
			simdata.MarkerList[id].state = 0;

		simdata.MarkerList[id].detected = false;
		return;
	}

	if (simdata.MarkerList[id].detected)
		simdata.MarkerList[id].state = 0;
	else
		simdata.MarkerList[id].state = 1;

	simdata.MarkerList[id].detected = true;

	_pMarker->GetPosition(&position[0], &position[1], &position[2]);
	_pMarker->GetOrientation(&axis[0], &axis[1], &axis[2], &angle);

	_pMarker->GetMarkerId(&mid);
	simdata.MarkerList[id].markerID = mid;

	simdata.MarkerList[id].pos.x = position[0] * 0.001;
	simdata.MarkerList[id].pos.y = position[1] * 0.001;
	simdata.MarkerList[id].pos.z = position[2] * 0.001;

	simdata.MarkerList[id].ori.x = axis[0];
	simdata.MarkerList[id].ori.y = axis[1];
	simdata.MarkerList[id].ori.z = axis[2];
	simdata.MarkerList[id].ori.angle = angle * 180.0 / M_PI;
	return;
}


//===========================================================================//
// ターゲットオブジェクトの描画
//===========================================================================//
void renderTarget(IMrhTarget *_pTarget, int id) {
	// ターゲット位置に仮想物体を表示
	glPushMatrix();

	// ターゲットの位置姿勢を取得
	GLfloat position[3] = { 0.0, 0.0, 0.0 };
	GLfloat axis[3] = { 0.0, 0.0, 0.0 };
	GLfloat angle = 0.0;

	_pTarget->GetPosition(&position[0], &position[1], &position[2]);
	_pTarget->GetOrientation(&axis[0], &axis[1], &axis[2], &angle);

	// 取得したターゲットの位置姿勢を設定することで，ターゲットの位置に
	// 仮想物体を描画することが出来る.
	glTranslatef(position[0], position[1], position[2]);
	glRotatef((GLfloat)(angle * 180. / M_PI),
		(GLfloat)axis[0], (GLfloat)axis[1], (GLfloat)axis[2]);

	//////// ここにターゲットの描画コード ////////

	glPopMatrix();
}

//===========================================================================//
// 円錐を表示する関数
//===========================================================================//
void dispCone(GLUquadric* Cylinder, GLdouble radius, GLdouble height,
	GLfloat r, GLfloat g, GLfloat b) {
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	// マテリアルの設定
	GLfloat matDiff[] = { r, g, b, 1.0f };
	GLfloat matSpec[] = { 0.6f, 0.6f, 1.0f, 1.0f };
	GLfloat matAmbient[] = { 0.6f, 0.6f, 0.7f, 1.0f };
	GLfloat matShininess = 128.0f;
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matAmbient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matDiff);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpec);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, matShininess);

	gluCylinder(Cylinder, radius, 0.0, height, 30, 30);
}

//===========================================================================//
// 円柱を表示する関数
//===========================================================================//
void dispCylinder(GLUquadric* Cylinder, GLdouble radius, GLdouble height,
	GLfloat r, GLfloat g, GLfloat b) {
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	// マテリアルの設定
	GLfloat matDiff[] = { r, g, b, 1.0f };
	GLfloat matSpec[] = { 0.6f, 0.6f, 1.0f, 1.0f };
	GLfloat matAmbient[] = { 0.6f, 0.6f, 0.7f, 1.0f };
	GLfloat matShininess = 128.0f;
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matAmbient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matDiff);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpec);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, matShininess);

	gluCylinder(Cylinder, radius, radius, height, 30, 30);

}
//===========================================================================//
// OpenGL関連終了処理
//===========================================================================//
void finalizeOpenGL(HWND hWnd) {
	// レンダリングコンテキストを解除
	wglMakeCurrent(NULL, NULL);
	// レンダリングコンテキストを削除
	wglDeleteContext(hRC);
	// デバイスコンテキストを解除
	ReleaseDC(hWnd, hDC);
	// 2次曲面オブジェクトの破棄
	gluDeleteQuadric(pFillObj);
}

//===========================================================================//
// MREAL Platform関連インスタンス終了処理
//===========================================================================//
void releaseMrPlatform() {
	for (unsigned int i = 0; i < dwNumOfTarget; i++)
		pTarget[i]->Release();
	for (unsigned int i = 0; i < dwNumOfCameras; i++) {
		pCamera[i]->Release();
		pRender[i]->Release();
	}

	// 色領域のステレオ計測関連のインターフェイスを解放
	if (polygon[0])
		polygon[0]->Release();
	if (polygon[1])
		polygon[1]->Release();
	if (depth)
		depth->Release();

	pDispInfo->Release();
	pEngine->Release();
	// COMライブラリーを閉じる
	::CoUninitialize();
}
