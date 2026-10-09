/******************************************************************************
 * glsample_frame.cpp 
 * Level 3 - base framework */

#include "platform.h"

#include "common.h" //WindowDataT, MouseDataT, KeyDataT
#include "sim.h"
#include "config.h" //◆表示環境の設定
#include "light.h"

#include "WarpTex.h"

WarpTex g_DC, g_DC_right, g_DC_left;
float g_cx = 0;
float g_cy = 0;
float g_kR = -0.0005;
float g_kG = -0.0005;
float g_kB = -0.0005;

//hmd.cpp
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
/*
//-------- light.cpp
void MainLight( int light_id, float r, float g, float b );
void SubLight( int light_id, float r, float g, float b);
void HeadLight( int light_id, float r, float g, float b, float att );
void Fog( int fog_mode, float r, float g, float b, float d, float start, float end );
void SpotLight( int light_id, float r, float g, float b, float att_half, 
	float cutoff, int exponent );

void HeadLight( void );
void Lighting( void );
*/
#include "light.h"

void applyObjTransform( ObjDataT *obj ); ////////0627

inline float _blend( float k, float a, float b )
{ return ( k * a + ( 1.0 - k ) * b ); }




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
	setFog();

	DrawScene();

	glPopMatrix();
}
/*------------------------------------------------------------------- display
 * display - GLUT display callback function
 *--------*/
void display( void )
{
	//▼背景色とフォグカラーをブレンド
    glClearColor(
		_blend( simdata.sky_color[3], simdata.sky_color[0], simdata.air_color[0] ),
		_blend( simdata.sky_color[3], simdata.sky_color[1], simdata.air_color[1] ),
		_blend( simdata.sky_color[3], simdata.sky_color[2], simdata.air_color[2] ),
		1.0 );
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
/*---------------------------------------------------------------- initWindow
 * initWindow: initializes OpenGL window
 *--------*/
void initWindow( char *winname )
{
	if( !fullscreen ){// ウィンドウモード
		if (cylindrical) {
			window.width = 1025;//4100/4
			window.height = 300;//1080/4= 270, 1200/4= 300;
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
	glutCreateWindow( window.title );

	if( fullscreen ){// フルスクリーンモード
	    //window.width = 4100;
	    //window.height = 1080;
		//window.xo = 0;
	    //window.yo = 0;
	    glutFullScreen(); //シリンドリカルスクリーンの場合 4100x1080
		glutSetCursor(GLUT_CURSOR_NONE);
	}
	else {
		if (no_cursor) glutSetCursor(GLUT_CURSOR_NONE);
	}

	window.width = glutGet( GLUT_WINDOW_WIDTH );
	window.height= glutGet( GLUT_WINDOW_HEIGHT );
	window.xo = glutGet( GLUT_WINDOW_X );
	window.yo = glutGet( GLUT_WINDOW_Y );
	window.aspect = (float)window.width/window.height; // aspect ratio

	//-------- GL mode
	glEnable( GL_NORMALIZE );
	glLightModeli( GL_LIGHT_MODEL_LOCAL_VIEWER, true );

	if( hmd && distortion ){
		g_DC_left.init();
		//g_cx = window.width/4;
		//g_cy = window.height/2;
		g_DC_left.setSize(window.width / 2, window.height);
		g_DC_left.setCenter( g_cx, g_cy );
		g_DC_right.init();
		g_DC_right.setSize(window.width / 2, window.height);
		g_DC_right.setCenter( g_cx, g_cy );
		g_DC_left.setParams(g_kR, g_kG, g_kB);
		g_DC_left.setParams(g_kR, g_kG, g_kB);
		g_DC_left.setParams(g_kR, g_kG, g_kB);
		g_DC_right.setParams(g_kR, g_kG, g_kB);
		g_DC_right.setParams(g_kR, g_kG, g_kB);
		g_DC_right.setParams(g_kR, g_kG, g_kB);
	}

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

    initWindow( argv[0] );

	//-------- basic callbacks　基本的なコールバック関数の設定
	if( hmd ) glutDisplayFunc( HMD_View );
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

	//歪み補正機能初期化
	//if( distortion ) g_DC.init();

    glutMainLoop(); // run main loop

    return 0;
}
/******************************************************************************
 * end of source code */
