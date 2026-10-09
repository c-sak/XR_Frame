// UsersGuideSample-6.cpp


#include "stdafx.h"
#include "UsersGuideSample.h"
#include "DirectInputDevice.h"
#include <MrHarmonizer.h>
#include <map>

////////////////★
#include "sim.h"
#include "light.h"
extern SimDataT simdata;
#define METER_UNIT

void copyTarget(IMrhTarget61 *target, int id, bool detected );
void copyMarker(IMrhMarker *marker, int id, bool detected);
void copyCamera(IMrhCamera60* camera, TargetT* target, bool detected);

DWORD* availableTargetIDs = NULL;

IMrhMarker** vpMarkers; //★マーカ
////////////////★

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
IMrhEngine70			*pEngine;				// エンジン
IMrhCamera60			*pCamera[2];			// カメラ
IMrhDisplayInfo			*pDispInfo;				// 表示設定
IMrhRenderer60			*pRender[2];			// 描画API
IMrhDepth70* depth = 0;							// 奥行き計測結果インターフェイス
IMrhPolygon70* polygon[2] = {};					// 手の奥行き計測結果のポリゴン
static std::map<DWORD, IMrhTarget61*>	mpTargets;	// 有効なターゲットマップ(MRH_TARGET_X と IMrhTarget)

HANDLE					hEvent			= NULL; // エンジン更新イベント
static DWORD			dwNumOfCameras	= 0;	// カメラの数
static DWORD			dwNumOfTarget	= 0;	// ターゲットの数
static DWORD			dwNumOfMarker = 0;		//★マーカの数

GLUquadricObj			*pFillObj		= NULL; // 仮想物体描画に使用
static HDC				hDC				= NULL;	// デバイスコンテキスト
static HGLRC			hRC				= NULL;	// レンダリングコンテキスト
static GLuint			bmpListID		= 0;	// ディスプレイ・リストID

static tagMrhColorRegionMaskMethod maskMethod = MRH_COLOR_REGION_METHOD_NONE; // マスク画像描画方法

// MREAL Displayボタン処理に使用する変数
DirectInputDevice		*pMREALButtonInput = NULL;	// MREAL Displayボタン制御オブジェクト
HANDLE					hRegistrationUtilityEnd;	// 位置合わせユーティリティの終了を検知するイベント

const std::string		DEVICE_NAME = "Canon MREAL S1 Button"; // MREAL S1のボタンデバイス名
//const std::string		DEVICE_NAME = "Canon MREAL Display Button MD-20 Series"; // MREAL Display MD-20のボタンデバイス名
//const std::string		DEVICE_NAME = "Canon MREAL Display Button MD-10 Series"; // MREAL Display MD-10のボタンデバイス名

static DWORD regStatus;
//IMrhTargetが必要なためこの場所で定義
void renderTarget( IMrhTarget61 *_pTarget );

//===========================================================================//
// WinMain関数
//===========================================================================//
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst,
	LPSTR lpszCmdLine, int nCmdShow) {

	////////////////★
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	printf("START MR PROJECT\n");
	//getchar();
	////////////////★

	// MREAL Platform関連インスタンスの初期設定
	if ( !initMrPlatform() ) 
		return 0;

	// ウィンドウクラスの登録
	char className[] = "UsersGuideSample"; 
	if ( !InitApp( hInstance, className ) ) {
		MessageBox( NULL, "Cannot Register WNDCLASSEX\n", "ERROR", MB_ICONERROR );
		return 0;
	}
	// ウィンドウの作成
	HWND hWnd = NULL;
	if ( ( hWnd = InitInstance( hInstance, className, nCmdShow ) ) == NULL ) {
		MessageBox( NULL, "Cannot Create Window\n", "ERROR", MB_ICONERROR );
		return 0;
	}

	// MREAL Displayボタン関連インスタンスの初期化
	initMREALButton(hWnd);

	MSG msg;
	BOOL bEndFlg = FALSE;
	const HANDLE signals[] = { hRegistrationUtilityEnd, hEvent };
	const DWORD dwNumOfSignals = ( sizeof( signals ) / sizeof( signals[0] ) );

	while ( !bEndFlg ) {
		DWORD dwRet = ::MsgWaitForMultipleObjects( dwNumOfSignals, signals, FALSE, INFINITE, QS_ALLINPUT );
		if( dwNumOfSignals <= dwRet )
		{
			// メッセージキューからキーイベントを取得
			while ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) ) {
				if ( msg.message == WM_QUIT ) {
					bEndFlg = TRUE;
					break;
				}
				// キーイベントを文字メッセージへ変換
				TranslateMessage( &msg );
				// 文字メッセージを適切なウィンドウプロシージャに転送
				DispatchMessage( &msg );
			}
		}

		// 位置合わせユーティリティの終了イベントの発生を確認する。
		else if( signals[dwRet] == hRegistrationUtilityEnd )
			SetForegroundWindow( hWnd );

		// エンジンの状態変化イベントかキーイベントを取得するまで待機
		else if( signals[dwRet] == hEvent ) {
			HRESULT hr;

			// エンジンのパラメーターを更新
			hr = pEngine->Update();
			if ( FAILED( hr ) ) {
				MessageBox( NULL, "IMrhEngine70::Update() FAILED.", "ERROR", MB_ICONERROR );
				DestroyWindow( hWnd );
				break;
			}

			//表示画面のウィンドウ矩形を取得する。
			pDispInfo->GetWindowRect( &winX, &winY, &winW, &winH );
			
			RECT rc = { winX, winY, winX + winW, winY + winH };
			::AdjustWindowRectEx( &rc, ::GetWindowLong( hWnd, GWL_STYLE ), ::GetMenu( hWnd ) != NULL, ::GetWindowLong( hWnd, GWL_EXSTYLE ) );
			::SetWindowPos( hWnd, NULL, winX, winY, rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER );

			// マスターカメラがあるかどうか確認
			BOOL isExist = FALSE;
			pEngine->QueryCamera( MRH_CAMERA_MASTER, &isExist );
			if ( isExist ) {
				//カメラごとに個々のビューポート矩形を取得する
				pDispInfo->GetViewportRect( MRH_CAMERA_MASTER, &dispOrigin[0].x, &dispOrigin[0].y, &dispSize[0].width, &dispSize[0].height );
			}
			// スレーブカメラがあるかどうか確認
			// Configuration Toolで単眼に設定した際にも動作するように
			// 毎フレーム問い合わせる
			pEngine->QueryCamera( MRH_CAMERA_SLAVE, &isExist );
			if ( isExist ) {
				//カメラごとに個々のビューポート矩形を取得する
				pDispInfo->GetViewportRect( MRH_CAMERA_SLAVE, &dispOrigin[1].x, &dispOrigin[1].y, &dispSize[1].width, &dispSize[1].height );
			}

			// 実写画像及び仮想物体の描画
			dispFunc();

			//フレーム描画の終了処理を行う
			pEngine->EndFrame();

			SwapBuffers( hDC );

			// MREAL Displayボタンを確認する。
			execMREALButtonCommand();
		}
	}
	// MREAL Displayボタン関連インスタンスの終了処理
	releaseMREALButton();
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
	::CoInitialize( NULL );

	// エンジンインスタンスを作成
	HRESULT hr = CoCreateInstance( CLSID_CMrhEngine62, NULL, CLSCTX_INPROC, IID_IMrhEngine62, (LPVOID*)&pEngine );
	
	if ( FAILED( hr ) ) {
		MessageBox( NULL, "Failed in creating a instance of the MREAL Engine.", "Failed in starting an MREAL Engine", MB_OK|MB_ICONERROR );
		return FALSE;
	}

	// エンジン初期化
	hr = pEngine->Initialize();
	if ( FAILED( hr ) ) {
		MessageBox( NULL, "Failed in initializing a instance of the MREAL Engine.", "Failed in initializing an MREAL Engine", MB_OK|MB_ICONERROR );
		return FALSE;
	}
	// 更新イベントの生成
	hEvent = ::CreateEvent( NULL, FALSE, FALSE, NULL );
	// エンジンの状態が変化したときのイベントを指定
	pEngine->SetEventNotification( hEvent );

	// 表示設定の情報を問い合わせるインスタンスを作成
	if ( FAILED( pEngine->QueryInterface( IID_IMrhDisplayInfo, (void **)&pDispInfo ) ) ){
		MessageBox( NULL, "Failed in creating a instance of the DisplayInfo.", "Failed in getting a DisplayInfo", MB_OK|MB_ICONERROR );
		return FALSE;
	}

	// マスターカメラがあるかどうか確認
	dwNumOfCameras = 0;
	BOOL isExist = FALSE;
	pEngine->QueryCamera( MRH_CAMERA_MASTER, &isExist );
	if ( isExist ) {
		// マスターカメラインスタンスを作成
		pEngine->GetCamera60( MRH_CAMERA_MASTER, &pCamera[dwNumOfCameras] );		
		pEngine->GetRenderer60( MRH_CAMERA_MASTER, &pRender[dwNumOfCameras] );
		dwNumOfCameras++;
	}
	// スレーブカメラがあるかどうか確認
	pEngine->QueryCamera( MRH_CAMERA_SLAVE, &isExist );
	if ( isExist ) {
		// スレーブカメラインスタンスを作成
		pEngine->GetCamera60( MRH_CAMERA_SLAVE, &pCamera[dwNumOfCameras] );
		pEngine->GetRenderer60( MRH_CAMERA_SLAVE, &pRender[dwNumOfCameras] );
		dwNumOfCameras++;
	}

	// スレーブカメラのインターフェイスが存在する場合にステレオ計測を行い
	//検出した色領域のポリゴンを作成する．このとき，左右でステレオ計測対象の
	//形状が異なるため，右眼，左眼用のインターフェイスをそれぞれ取得する．
	if(dwNumOfCameras == 2) { 
		maskMethod = MRH_COLOR_REGION_METHOD_MASK_IMAGE;
		//maskMethod = MRH_COLOR_REGION_METHOD_POLYGON;

		// 色領域のステレオ計測に関する設定
		BOOL exist = FALSE;
		HRESULT res;
		// 色領域のステレオ計測の結果を取得するインターフェイスを取得
		res = pEngine->QueryDepth61(MRH_DEPTH_COLOR_REGION, &exist);
		assert(res == S_OK && exist);
		res = pEngine->GetDepth70(MRH_DEPTH_COLOR_REGION, &depth);
		assert(res == S_OK && depth);

		const DWORD camera[] = { MRH_CAMERA_MASTER, MRH_CAMERA_SLAVE};
		for(DWORD i=0; i<dwNumOfCameras; ++i) {
			// ポリゴンインターフェイスを取得
			res = depth->QueryPolygon(camera[i], &exist);
			assert(res == S_OK && exist);
			res = depth->GetPolygon70(camera[i], &polygon[i]);
			assert(res == S_OK && polygon[i]);
		}
	} 

	for(DWORD i=0; i<dwNumOfCameras; ++i) {
		// マーカーキャンセラーの初期設定
		pRender[i]->SetMarkerCancelObject(MRH_MARKER_CANCEL_OBJECT_PROJECTION);
		// マスク画像描画方法の初期設定
		pRender[i]->SetColorRegionMaskMethod(maskMethod);
	}

	// ターゲットの数を取得
	//DWORD *availableTargetIDs = NULL;
	pEngine->GetAvailableTargetIDs(&dwNumOfTarget, nullptr);
	if (dwNumOfTarget != 0)
	{
		availableTargetIDs = new DWORD[dwNumOfTarget];
		pEngine->GetAvailableTargetIDs(&dwNumOfTarget, availableTargetIDs);
	}

	// ターゲットインスタンスを作成
	for (DWORD i = 0; i < dwNumOfTarget; ++i) {
		hr = pEngine->GetTarget61(availableTargetIDs[i], &mpTargets[availableTargetIDs[i]]);
		assert(hr == S_OK);
	}

	////////////////★
	pEngine->GetNumOfMarker(&dwNumOfMarker);
	vpMarkers = new IMrhMarker*[dwNumOfMarker];
	for (DWORD i = 0; i < dwNumOfMarker; i++) {
		IMrhMarker* cpMarker;
		HRESULT res = pEngine->GetMarker(i, &cpMarker);
		vpMarkers[i] = cpMarker;
	}
	////////////////★

	return TRUE;
}

//===========================================================================//
// ウィンドウクラスの登録
//===========================================================================//
BOOL InitApp( HINSTANCE hInstance, LPSTR className ) {
	WNDCLASSEX wndclsex;

	wndclsex.cbSize = sizeof(WNDCLASSEX);

	wndclsex.style			= CS_HREDRAW | CS_VREDRAW ;					// スタイル
	wndclsex.lpfnWndProc	= WndProc;									// ウィンドウプロシージャの名前
	wndclsex.cbClsExtra		= 0;										// メモリ領域の追加設定
	wndclsex.cbWndExtra		= 0;										// メモリ領域の追加設定
	wndclsex.hInstance		= hInstance;								// プログラムハンドル
	wndclsex.hIcon			= LoadIcon( hInstance, 						// アイコン
		MAKEINTRESOURCE(IDI_GUIDESAMPLE1) );
	wndclsex.hCursor		= LoadCursor( NULL, IDC_ARROW );			// カーソル
	wndclsex.hbrBackground	= (HBRUSH)GetStockObject( WHITE_BRUSH );	// ブラシ
	wndclsex.lpszMenuName	= NULL;										// メニュー
	wndclsex.lpszClassName	= className;
	wndclsex.hIconSm		= LoadIcon(wndclsex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx( &wndclsex );
}

//===========================================================================//
// ウィンドウプロシージャ
//===========================================================================//
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch ( msg ) {
		// ウィンドウ生成メッセージ -------------------------------------------
	case WM_CREATE :
		// OpenGLの初期設定
		initOpenGL(hWnd);
		break;
		// ウィンドウ破棄メッセージ -------------------------------------------
	case WM_DESTROY : 
		// OpenGL関連の終了処理
		finalizeOpenGL( hWnd );
		// メッセージキューに終了メッセージを送付
		PostQuitMessage(0);
		break;
		// 描画メッセージ -----------------------------------------------------
	case WM_PAINT:
		dispFunc();
		return DefWindowProc( hWnd, msg, wParam, lParam );
		// キーイベント -------------------------------------------------------
	case WM_CHAR :
		keyPress( hWnd, wParam );
		break;
		// メッセージのデフォルト処理 -----------------------------------------
	default:
		return DefWindowProc( hWnd, msg, wParam, lParam );
	}
	return 0;
}

//===========================================================================//
// キーイベント
//===========================================================================//
void keyPress( HWND hWnd, WPARAM wParam ) {
	switch ( wParam ) {
	case 27	 :	// プログラム終了(Escキー)
	case 'q' :
		DestroyWindow( hWnd );
		break;
	}
}

//===========================================================================//
// ウィンドウの作成
//===========================================================================//
HWND InitInstance ( HINSTANCE hInstance, LPSTR className ,int nCmdShow ) {

	// ウィンドウの作成
	HWND hWnd = CreateWindow( className,	// クラス名
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
	if ( !hWnd  ) return NULL;
	// ウィンドウの表示
	ShowWindow( hWnd, nCmdShow );
	// 再描画を指示
	UpdateWindow( hWnd );
	return hWnd;
}

//===========================================================================//
// OpenGL関連初期設定
//===========================================================================//
void initOpenGL( HWND hWnd ) {
	// デバイスコンテキストの取得
	hDC = GetDC( hWnd );
	// ピクセルフォーマットの設定
	if ( !setPixelFormat( hDC ) ) {
		MessageBox( hWnd, "Cannot set PixelFormat", "ERROR", MB_ICONERROR );
		finalizeOpenGL( hWnd );
		DestroyWindow( hWnd );
	}
	// レンダリングコンテキストを作成
	hRC = wglCreateContext( hDC );
	// レンダリングコンテキストをカレントに設定
	wglMakeCurrent( hDC, hRC );

	// ウィンドウクリア時の色指定
	glClearColor(0, 0, 0, 1);

	// 2次曲面オブジェクトを生成
	pFillObj = gluNewQuadric();
	gluQuadricDrawStyle(pFillObj, GLU_FILL);

	// 光源を設定
	//setLight();//☆

	////////////////★
	InitScene();//★
	////////////////★
}

//===========================================================================//
// MREAL Displayボタン状態更新時の処理
//===========================================================================//
void execMREALButtonCommand()
{
	// MREAL Displayボタンの[A]ボタンの状態を取得。
	DIJOYSTATE btnState{};
	static BYTE lastState = 0;

	if (FAILED(pMREALButtonInput->GetJoysticState(&btnState)))
	{
		lastState = 1;
		return;
	}
	BYTE currentState = btnState.rgbButtons[0];

	// ボタン状態が押下状態に変化したときに、空間特徴ツールを起動する。
	if (lastState == 0 && currentState != 0)
	{
		if (pEngine->IsRegistrationUtilityAvailable(nullptr) == S_OK)
			pEngine->LaunchRegistrationUtility();
		else
			MessageBox(NULL, "Registration Utility is not available.", "Win32Sample Error", MB_ICONERROR);
	}
	lastState = currentState;
}

//===========================================================================//
// MREAL Displayボタン関連インスタンスの初期化
//===========================================================================//
void initMREALButton(HWND window_handle)
{
	// MREAL Displayボタンの初期化
	pMREALButtonInput = new DirectInputDevice(window_handle);
	if (pMREALButtonInput->Initialize())
		pMREALButtonInput->Connect();
	
	// 位置合わせユーティリティの終了を知るイベントを取得
	pEngine->GetRegistrationUtilityEventNotification( &hRegistrationUtilityEnd );
}

//===========================================================================//
// MREAL Displayボタン関連インスタンス終了処理
//===========================================================================//
void releaseMREALButton()
{
	delete pMREALButtonInput;
	pMREALButtonInput = nullptr;
	// 位置合わせユーティリティ終了用Eventの破棄
	CloseHandle( hRegistrationUtilityEnd );
}

//===========================================================================//
// 仮想物体描画のための光源の設定
//===========================================================================//
void setLight( void ) {
	// ライトの設定
	// ライト０番の光源設定
	static GLfloat lightAmbient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	static GLfloat lightDiffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	static GLfloat lightPosition[] = { 0.0f, 1.0f, 0.0f, 0.0f };
	glLightfv( GL_LIGHT0, GL_AMBIENT, lightAmbient );
	glLightfv( GL_LIGHT0, GL_DIFFUSE, lightDiffuse );
	glLightfv( GL_LIGHT0, GL_POSITION, lightPosition );
	// 光源モデルの設定
	static float lmodelAmbient[] = {0.4f, 0.4f, 0.4f, 1.0f};
	static float lmodelLocalviewer[] = {0.0f};
	glLightModelfv( GL_LIGHT_MODEL_AMBIENT, lmodelAmbient );
	glLightModelfv( GL_LIGHT_MODEL_LOCAL_VIEWER, lmodelLocalviewer );
}

//===========================================================================//
//  ピクセルフォーマットを設定
//===========================================================================//
BOOL setPixelFormat( HDC hdc ) {
	// ピクセルフォーマットの設定
	PIXELFORMATDESCRIPTOR pfd ;
	memset(&pfd,0, sizeof(PIXELFORMATDESCRIPTOR)) ;

	pfd.nSize		= sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion	= 1;
	pfd.dwFlags		= PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL |
		PFD_GENERIC_FORMAT |
		PFD_DOUBLEBUFFER;
	pfd.iPixelType	= PFD_TYPE_RGBA ;
	pfd.cStencilBits = 1;
	pfd.cColorBits	= 24;
	pfd.cDepthBits	= 32;
	pfd.iLayerType	= PFD_MAIN_PLANE ;            // Layer type

	// デバイスコンテキストにピクセルフォーマットを設定
	int pixelFormat = ChoosePixelFormat( hdc, &pfd );
	if ( pixelFormat == 0 ) return FALSE;
	return SetPixelFormat(hdc, pixelFormat, &pfd );
}

//===========================================================================//
//  描画関数
//===========================================================================//
void dispFunc() {
	// 画面クリア
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
	glEnable( GL_DEPTH_TEST );
	//フラスタムパラメーター取得変数
	//double l,r,b,t,n=10,f=3000; //☆
	double l, r, b, t, n = 0.1, f = 10000.0; //★
	////////////////★
	// ターゲット情報の取得
	// カメラの位置姿勢がわからない場合でも
	// ターゲットのデータをsimdataのオブジェクトデータにコピー
	// 旧pTarget -> 新mpTargets
#if 1
	for (unsigned int j = 0; j < dwNumOfTarget; j++) {
		if (mpTargets[availableTargetIDs[j]]->GetRegistrationStatus(&regStatus) == S_OK
			&& regStatus == MRH_REGISTRATION_RESULT_OK) {
			copyTarget(mpTargets[availableTargetIDs[j]], j, true);
		}
		else {
			copyTarget(mpTargets[availableTargetIDs[j]], j, false);
		}
	}
#endif
	UpdateScene();
	glEnable(GL_NORMALIZE);
	////////////////★
	for ( unsigned int i = 0; i < dwNumOfCameras; i++ ) {
		// ビューポートの設定
		glViewport( dispOrigin[i].x, dispOrigin[i].y, dispSize[i].width, dispSize[i].height );

		// 実写画像の表示
		pRender[i]->SetDispPosition( dispOrigin[i].x, dispOrigin[i].y );
		pRender[i]->SetDispSize( dispSize[i].width, dispSize[i].height );
		pRender[i]->SetNearFar(n, f);
		pRender[i]->Render();

		// 透視投影行列の設定
		glMatrixMode( GL_PROJECTION );
		// IMrhCameraからフラスタムパラメーターを取得
		pCamera[i]->GetFrustum( &l, &r, &b, &t, n );
		// 取得したフラスタムパラメーターをglFrustumへ設定することで, 現在表示している
		// キャプチャー画像に適合した透視投影設定で仮想物体を描画することが
		// 可能になる．
		glLoadIdentity();
		glFrustum( l, r, b, t, n, f );

		// モデルビュー変換行列の設定
		glMatrixMode( GL_MODELVIEW );

		// IMrhCameraからカメラの世界座標系上での位置姿勢を取得
		GLfloat position[3] = { 0.0, 0.0, 0.0 };
		GLfloat axis[3] = { 0.0, 0.0, 0.0 };
		GLfloat angle = 0.0;

		pCamera[i]->GetPosition( &position[0], &position[1], &position[2] );
		pCamera[i]->GetOrientation( &axis[0], &axis[1], &axis[2], 
			&angle );
		// 取得した位置姿勢を設定することで, 現在表示している現実空間に
		// 適合した位置に, 仮想物体を描画することが可能になる．
		glLoadIdentity();
		glRotatef( (GLfloat)(-angle * 180./M_PI), 
			(GLfloat)axis[0], (GLfloat)axis[1], (GLfloat)axis[2] );
		//glTranslatef( -position[0], -position[1], -position[2] ); ////★

		//カメラ位置姿勢の取得フラグ
		DWORD regResult;
		//マスターカメラの位置姿勢が取得できているか確認
		pCamera[0]->GetRegistrationStatus(&regResult);

		//カメラの位置姿勢が初期位置（原点）のままだとCGが現実映像を覆ってしまい，
		//現実映像が映っていないと間違える可能性がある．そのため，カメラの
		//位置姿勢が取得できてからCGを描画する．
		if( regResult == MRH_REGISTRATION_RESULT_OK ) {
			/*
			// 円錐の表示
			glPushMatrix();
			glRotatef( -90.0f, 1.0f, 0.0f, 0.0f );
			dispCone( pFillObj, 50, 100, 1.0, 1.0, 0.0 );
			glPopMatrix();
			*/
			copyCamera(pCamera[i], &simdata.mrealCamera[i], true); //★
		}
		else {
			copyCamera(pCamera[i], &simdata.mrealCamera[i], false); //★
		}
#if 0
		//ターゲットを表示する．このとき，カメラの位置姿勢が分からない場合でも，
		//ターゲットが見えている場合はターゲットを表示する．
		glTranslatef( -position[0], -position[1], -position[2] ); ////★
		for (const std::pair<DWORD, IMrhTarget61*>& target : mpTargets) {
			DWORD regStatus;
			if (target.second->GetRegistrationStatus(&regStatus) == S_OK) {
				renderTarget(target.second); // Display the cube indicating the target position
			}
		}
#else ////////////////★
		glTranslatef( -position[0]*0.001, -position[1]*0.001, -position[2]*0.001);
		glEnable(GL_LIGHTING);
		HeadLight();
		Lighting();
		glPushMatrix();
		DrawScene();
		glPopMatrix();
		glDisable(GL_LIGHTING);
		PostDraw();
#endif

	}
	glFlush();
}
void copyCamera(IMrhCamera60* camera, TargetT* target, bool detected)
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
void copyTarget(IMrhTarget61* _pTarget, int id, bool detected)
{
	GLfloat position[3] = { 0.0, 0.0, 0.0 };
	GLfloat axis[3] = { 0.0, 0.0, 0.0 };
	GLfloat angle = 0.0;

	if (_pTarget == nullptr) return;
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

void copyMarker(IMrhMarker* _pMarker, int id, bool detected, int targetID)
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
void renderTarget( IMrhTarget61 *_pTarget ) {
	// ターゲット位置に仮想物体を表示
	glPushMatrix();

	// ターゲットの位置姿勢を取得
	GLfloat position[3] = { 0.0, 0.0, 0.0 };
	GLfloat axis[3] = { 0.0, 0.0, 0.0 };
	GLfloat angle = 0.0;

	_pTarget->GetPosition( &position[0], &position[1], &position[2] );
	_pTarget->GetOrientation( &axis[0], &axis[1], &axis[2], &angle );

	// 取得したターゲットの位置姿勢を設定することで，ターゲットの位置に
	// 仮想物体を描画することが出来る.
	glTranslatef( position[0], position[1], position[2] );
	glRotatef( (GLfloat)(angle * 180./M_PI), 
		(GLfloat)axis[0], (GLfloat)axis[1], (GLfloat)axis[2] );

	// 円柱を描画
	glRotatef( -90.0f, 1.0f, 0.0f, 0.0f );
	dispCylinder( pFillObj, 30, 80, 0.0, 0.0, 1.0 );
	glPopMatrix();
}

//===========================================================================//
// 円錐を表示する関数
//===========================================================================//
void dispCone( GLUquadric* Cylinder, GLdouble radius, GLdouble height, 
	GLfloat r, GLfloat g, GLfloat b ) {
		glEnable( GL_LIGHTING );
		glEnable( GL_LIGHT0 );

		// マテリアルの設定
		GLfloat matDiff[] = { r, g, b, 1.0f };
		GLfloat matSpec[] = { 0.6f, 0.6f, 1.0f, 1.0f };
		GLfloat matAmbient[] = { 0.6f, 0.6f, 0.7f, 1.0f };
		GLfloat matShininess = 128.0f;
		glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, matAmbient);
		glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, matDiff);
		glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, matSpec);
		glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, matShininess );

		gluCylinder( Cylinder, radius, 0.0, height, 30, 30 );
}

//===========================================================================//
// 円柱を表示する関数
//===========================================================================//
void dispCylinder ( GLUquadric* Cylinder, GLdouble radius, GLdouble height, 
	GLfloat r, GLfloat g, GLfloat b ) {
		glEnable( GL_LIGHTING );
		glEnable( GL_LIGHT0 );

		// マテリアルの設定
		GLfloat matDiff[] = { r, g, b, 1.0f };
		GLfloat matSpec[] = { 0.6f, 0.6f, 1.0f, 1.0f };
		GLfloat matAmbient[] = { 0.6f, 0.6f, 0.7f, 1.0f };
		GLfloat matShininess = 128.0f;
		glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, matAmbient);
		glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, matDiff);
		glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, matSpec);
		glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, matShininess );

		gluCylinder( Cylinder, radius, radius, height, 30, 30 );

}
//===========================================================================//
// OpenGL関連終了処理
//===========================================================================//
void finalizeOpenGL( HWND hWnd ) {
	// レンダリングコンテキストを解除
	wglMakeCurrent( NULL, NULL );
	// レンダリングコンテキストを削除
	wglDeleteContext( hRC );
	// デバイスコンテキストを解除
	ReleaseDC( hWnd, hDC );
	// 2次曲面オブジェクトの破棄
	gluDeleteQuadric( pFillObj );
}

//===========================================================================//
// MREAL Platform関連インスタンス終了処理
//===========================================================================//
void releaseMrPlatform() {
	//ターゲットのリリース
	for (std::pair<const DWORD, IMrhTarget61*>& target : mpTargets) {
		target.second->Release();
	}
	for ( unsigned int i = 0; i < dwNumOfCameras; i++ ) {
		pCamera[i]->Release();
		pRender[i]->Release();
	}

	// 色領域のステレオ計測関連のインターフェイスを解放
	if(polygon[0])
		polygon[0]->Release();
	if(polygon[1])
		polygon[1]->Release();
	if(depth)
		depth->Release();

	pDispInfo->Release();
	pEngine->Release();
	// COMライブラリーを閉じる
	::CoUninitialize();
}
