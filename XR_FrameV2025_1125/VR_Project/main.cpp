/******************************************************************************
 * glsample_frame.cpp 
 * Level 3 - base framework */

#include "platform.h"

#include "common.h" //WindowDataT, MouseDataT, KeyDataT
#include "sim.h"
#include "config.h" //◆表示環境の設定
#include "light.h"
#include "Shapes.h"
#include "WarpTex.h"

//extern WarpTex g_DC_right, g_DC_left;
//extern float g_cx;
//extern float g_cy;
//extern float g_kR;
//extern float g_kG;
//extern float g_kB;

//hmd.cpp
void HMD_Config(void);
void HMD_View( void );

//-------- callback functions
void display( void );
static void update( void );
static void reshape( int width, int height );

//-------- init GL window
static void initWindow( char *title );

//-------- simple view
void singleView( float dx );

// projview.cpp
void Viewing( void );
void Projection( void );

//-------- mouse/kbdmouse.cpp: mouse callback functions
void mouseClick( int button , int state, int x, int y );
void mouseDrag( int x, int y );
void mouseMotion( int x, int y );

//-------- keyboard/kbdmouse.cpp: keyboard callback functions
void charKeyDown( unsigned char key, int x, int y );
void charKeyUp( unsigned char key, int x, int y );
void funcKeyDown( int key, int x, int y );
void funcKeyUp( int key, int x, int y );

WindowDataT window;
extern SimDataT simdata;

// cylindrical.cpp
void cylindricalView( float dx );

//stereo.cpp
void Stereo( void );

//light.cpp
#include "light.h"

void applyObjTransform( ObjDataT *obj ); ////////0627

int windowLeft, windowFront, windowRight;

/*------------------------------------------------------------------- update
 * update - GLUT idle callback function
 *--------*/
void update( void )
{
	//simdata.time = glutGet( GLUT_ELAPSED_TIME );

    UpdateScene();

	//-------- ready to redraw
	glutPostRedisplay();

	//-------- sync
	Sleep( 0 ); // NVIDIA Quadro
	//Sleep( 16 ); // for cheap video chips @ ~60Hz

	return;
}

/*---------------------------------------------------------------- singleView
 * singleView - draw view on single plane
 */
void singleView( float d )
{
	//-------- projection transformation
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	Projection();

	//-------- viewing transformation
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadIdentity();
	glTranslatef( d, 0.0, 0.0 );
	Viewing();

	//-------- head light
	HeadLight();

	//-------- draw
	Lighting();
	setFog(&simdata);

	DrawScene();

	glPopMatrix();
}
/*------------------------------------------------------------------- display
 * display - GLUT display callback function
 *--------*/
void display( void )
{
	//▼背景色とフォグカラーをブレンド
	ezBackground(&simdata);
	glViewport( 0, 0, window.width, window.height );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	PreDraw();

    //-------- draw --------
	glEnable( GL_DEPTH_TEST ); // ---- begin: 
    glEnable( GL_LIGHTING );

	if( cylindrical ){
		cylindricalView( 0.0 );
	}
	else{
		singleView( 0.0 );
	}

    glDisable( GL_LIGHTING );
    glDisable( GL_DEPTH_TEST );

	glViewport( 0, 0, window.width, window.height ); //20211210+
	PostDraw();
	
	//-------- swapbuffers --------
    glutSwapBuffers();

    return;
}
/*--------------------------------------------------------------------- reshape
 * reshape - GLUT reshape callback function
 *--------*/
void reshape( int width, int height )
{
	window.width = width;
	window.height = height;
	window.aspect = (float)window.width/window.height;
}
/*-------------------------------------------------------------- configWindow
 * configWindow: configure GLUT window
 *--------*/
void configWindow(int win_id)
{
	glutSetWindow(win_id);
}
/*---------------------------------------------------------------- initWindow
 * initWindow: initializes OpenGL window
 *--------*/
void initWindow( char *winname )
{
	if( !fullscreen ){// ウィンドウモード
		if (cylindrical) {
			window.width = 1681;//3362/4
			window.height = 360;//720/4
		}
		else { // >>>> HMD <<<<
			float k = 0.5;
			window.width = 1920.0 * k;
			window.height = 1080.0 * k;
		}
	    window.xo = 100;
	    window.yo = 100;
	    //-------- window properties
	    glutInitWindowPosition( window.xo, window.yo );
        glutInitWindowSize( window.width, window.height );
	}
	
    //-------- config buffers
	if( !hmd && ( stereo && quadbuffer ) )
		glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STEREO );
	else         
		glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );

    //-------- open window
	window.title = winname;
	windowFront = glutCreateWindow( window.title );

	if (fullscreen) {// フルスクリーンモード
		if (cylindrical) { //シリンドリカルスクリーンの場合
			window.width = 3362;
			window.height = 720;
			window.xo = 3200;
			window.yo = 0;
			//GLのデバイスコンテキストハンドル取得
			HDC glDc = wglGetCurrentDC();
			//ウィンドウハンドル取得
			HWND hWnd = WindowFromDC(glDc);
			//ウィンドウの属性(枠無し)と位置変更
			SetWindowLong(hWnd, GWL_STYLE, WS_POPUP);
			SetWindowPos(hWnd, HWND_TOP, window.xo, window.yo, window.width, window.height, SWP_SHOWWINDOW);
		}
		else {
			/*
			window.width = 1920;
			window.height = 1080;
			window.xo = 0;
			window.yo = 0;
			*/
			glutFullScreen();
			window.width = glutGet(GLUT_WINDOW_WIDTH);
			window.height = glutGet(GLUT_WINDOW_HEIGHT);
			window.xo = glutGet(GLUT_WINDOW_X);
			window.yo = glutGet(GLUT_WINDOW_Y);
		}
		glutSetCursor(GLUT_CURSOR_NONE);
	}
	else {
		if (no_cursor) glutSetCursor(GLUT_CURSOR_NONE);
	}
	
	window.aspect = (float)window.width/window.height; // aspect ratio

	//-------- GL mode
	glEnable( GL_NORMALIZE );
	glLightModeli( GL_LIGHT_MODEL_LOCAL_VIEWER, true );

	//-------- check profiles
	printf( "OPENGL PROFILES\n" );
	printf( "VENDER: %s\n", glGetString( GL_VENDOR ) );
	printf( "RENDERER: %s\n", glGetString( GL_RENDERER ) );
	printf( "VERSION: %s\n", glGetString( GL_VERSION ) );
	//printf( "%s\n", glGetString( GL_SHADING_LANGUAGE_VERSION ) );
	printf( "EXTENSIONS:\n" );
	int len;
	const char *extension = (char *)glGetString( GL_EXTENSIONS );
	char *head = (char *)malloc( len = strlen( extension ) + 1 );
	char *tail;
	strcpy( head, extension );
	do{
		tail = strchr( head, ' ' );
		if( tail ){
			*tail = '\0';
			printf( "%s\n", head );
			head = tail + 1;
		}
	} while( tail );
	//printf( "ENTER ANY KEY TO PROCEED>" );
	//getchar();
	printf("WINDOW SIZE: %d x %d\n", window.width, window.height);

    return;
}
/*---------------------------------------------------------------------- main
 * main
 *--------*/
int main( int argc, char *argv[] )
{
	/// ウィンドウを準備
    glutInit( &argc, argv );

	/*
	if (!debug) {
		use_tracker = true;
		use_vicon = true;
		cylindrical = true;
		fullscreen = true;
		stereo = true;
		quadbuffer = true;
	}
	*/
    initWindow( argv[0] );

	//-------- basic callbacks　基本的なコールバック関数の設定
	if (hmd) {
		if (hmd && distortion) {
			HMD_Config();
		}
		glutDisplayFunc(HMD_View);
	}
	else{
		if( stereo ){
			glutDisplayFunc( Stereo );  // display callback function
		}
		else{
			glutDisplayFunc( display );
		}
	}
    glutIdleFunc( update );    // idle callback function
	glutReshapeFunc( reshape ); // reshape callback function

	//-------- keyboard callbacks
    glutKeyboardFunc( charKeyDown ); // character key down callback
	glutKeyboardUpFunc( charKeyUp ); // character key up callback
	glutSpecialFunc( funcKeyDown ); // function key down callback
	glutSpecialUpFunc( funcKeyUp ); // function key up callback
	glutIgnoreKeyRepeat( 1 );   // disable key-repeat

	//-------- mouse callbacks
    glutMouseFunc( mouseClick );       // mouse click callback
	glutPassiveMotionFunc( mouseMotion ); // passive motion callback
	glutMotionFunc( mouseDrag ); // mouse drag callback	
	
    printf( "[H]:Help\n" );     // indicate help instruction

    InitScene(); //★状態の初期化

	simdata.time = glutGet( GLUT_ELAPSED_TIME );

	printf( "//////// プログラムを終了するときには[Q]を押してください////////\n" );
	printf( "では、ENTERキーを押すとプログラムがスタートします\n" );
	getchar();

    glutMainLoop(); // run main loop

    return 0;
}
/******************************************************************************
 * end of source code */
