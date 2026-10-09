/******************************************************************************
 * glut_keyboard.cpp 
 */

#include "platform.h"

#include "common.h"
#include "sim.h"

#include "config.h"
#include "WarpTex.h"
#include "gyro.h"

 //グローバル変数
extern WarpTex g_DC, g_DC_right, g_DC_left;
extern float g_cx;
extern float g_cy;
extern float g_kR;
extern float g_kG;
extern float g_kB;

extern WindowDataT window;
extern SimDataT simdata;

KeyDataT keydata;

/*-------------------------------------------------- modifier key status
 * Shift, Ctrl, Alt key status
 */
bool isShiftKeyDown( void )
{ return (bool)(glutGetModifiers() & GLUT_ACTIVE_SHIFT); }
bool isCtrlKeyDown( void )
{ return (glutGetModifiers() & GLUT_ACTIVE_CTRL)==GLUT_ACTIVE_CTRL; }
bool isAltKeyDown( void )
{ return (glutGetModifiers() & GLUT_ACTIVE_ALT)==GLUT_ACTIVE_ALT; }
/*------------------------------------------------------------- charKeyDown/Up
 * charKeyDown/Up - GLUT keyboard callback function
 * key: ASCII code
 * x, y: mouse position
 *--------*/
static void _charKey( unsigned char key, int x, int y, bool status )
{
    keydata.x = (float)x/window.width  * 2.0 - 1.0;// mouse x position
    keydata.y = (float)y/window.height * 2.0 - 1.0;// mouse y position
	keydata.charKey[ key ] = status; //true:DOWN, false:UP
	printf( "[%c] = %d\n", key, status );
}
void charKeyDown( unsigned char key, int x, int y )
{
	_charKey( key, x, y, true );

	//----------------------- begin user code
    switch( key ){
      case 'h': // help
        printf( "Instruction\n" );
        printf( "[H]:Help\n" );
        printf( "[Q]:Quit\n" );
        break;
      case 'q': // quit
		TermScene();
        exit(0);
        break;
	  case 'z':
		  if( use_gyro ) ResetGyro();
		  break;
		//RGBそれぞれの歪み系数設定
	  case 'R':
		  if( !distortion ) break;
		  g_kR += 0.00005f;
		  g_DC_left.setParams(g_kR, g_kG, g_kB);
		  g_DC_right.setParams(g_kR, g_kG, g_kB);
		  break;
	  case 'r':
		  if( !distortion ) break;
		  g_kR -= 0.00005f;
		  g_DC_left.setParams(g_kR, g_kG, g_kB);
		  g_DC_right.setParams(g_kR, g_kG, g_kB);
		  break;
	  case 'G':
		  if( !distortion ) break;
		  g_kG += 0.00005f;
		  g_DC_left.setParams(g_kR, g_kG, g_kB);
		  g_DC_right.setParams(g_kR, g_kG, g_kB);
		  break;
	  case 'g':
		  if( !distortion ) break;
		  g_kG -= 0.00005f;
		  g_DC_left.setParams(g_kR, g_kG, g_kB);
		  g_DC_right.setParams(g_kR, g_kG, g_kB);
		  break;
	  case 'B':
		  if( !distortion ) break;
		  g_kB += 0.00005f;
		  g_DC_left.setParams(g_kR, g_kG, g_kB);
		  g_DC_right.setParams(g_kR, g_kG, g_kB);
		  break;
	  case 'b':
		  if( !distortion ) break;
		  g_kB -= 0.00005f;
		  g_DC_left.setParams(g_kR, g_kG, g_kB);
		  g_DC_right.setParams(g_kR, g_kG, g_kB);
		  break;
	  case 'X':
		  if( !distortion ) break;
		  g_cx -= 5.0f;
		  g_DC_left.setCenter(g_cx, g_cy);
		  g_DC_right.setCenter(-g_cx, g_cy);
		  break;
	  case 'x':
		  if( !distortion ) break;
		  g_cx += 5.0f;
		  g_DC_left.setCenter(g_cx, g_cy);
		  g_DC_right.setCenter(-g_cx, g_cy);
		  break;
	  case 'Y':
		  if( !distortion ) break;
		  g_cy -= 5.0f;
		  g_DC_left.setCenter(g_cx, g_cy);
		  g_DC_right.setCenter(-g_cx, g_cy);
		  break;
	  case 'y':
		  if( !distortion ) break;
		  g_cy += 5.0f;
		  g_DC_left.setCenter(g_cx, g_cy);
		  g_DC_right.setCenter(-g_cx, g_cy);
		  break;
		  //デバッグ表示切り替え
	  case 'D':
	  case 'd':
		  if( !distortion ) break;
		  g_DC_left.switchDebugMode();
		  g_DC_right.switchDebugMode();
		  break;
	  case ' ':
		  if (simdata.cube.base != NULL) {
			  moveLocalToWorld(&simdata.cube);
		  }
		  else {
			  moveWorldToLocal(&simdata.cube, &simdata.handR);
		  }
		  break;
      default:
        break;
    }
	/*
	switch( key ){
      case 'h': // help
        printf( "Instruction\n" );
        printf( "[H]:Help\n" );
        printf( "[Q]:Quit\n" );
        break;
      case 'q': // quit
        //TermScene();
        exit(0);

        break;
      default:
        break;
    }
	*/
	//------------------- end user code
    return;
}
void charKeyUp( unsigned char key, int x, int y )
{
	_charKey( key, x, y, false );

    //------------------- begin user code

	//------------------- end user code

	return;
}
/*--------------------------------------------------------------- funcKeyDown/Up
 * funcKeyDown/Up - GLUT special key callback
 * key: GLUT key code
 * x, y: mouse position
 */
static void _funcKey( int key, int x, int y, bool status )
{
    keydata.x = (float)x/window.width  * 2.0 - 1.0; // mouse x position
    keydata.y = (float)y/window.height * 2.0 - 1.0; // mouse y position

	if( key <= GLUT_KEY_F12 ){ // F1-F12: 1-12
		keydata.funcKey[ key -1 ] = status;
		printf( "[F%d]", key );
	}
	else{
		if( key <= GLUT_KEY_DOWN ){ // CURSOR KEYS
			switch( key ){
 			  case GLUT_KEY_LEFT:
				  keydata.arrowLeft = status;
				  printf( "[LEFTARROW]" );
				  break;
			  case GLUT_KEY_UP:
				  keydata.arrowUp = status;
				  printf( "[UPARROW]" );
				  break;
			  case GLUT_KEY_RIGHT:
				  keydata.arrowRight = status;
				  printf( "[RIGHTARROW]" );
				  break;
			  case GLUT_KEY_DOWN: 
				  keydata.arrowDown = status;
				  printf( "[DOWNARROW]" );
				  break;
			}
		}
		else{
			switch( key ){
			  case GLUT_KEY_PAGE_UP:
				  keydata.pageUp = status;   
				  printf( "[PAGEUP]" );
				  break;
			  case GLUT_KEY_PAGE_DOWN:
				  keydata.pageDown = status; 
				  printf( "[PAGEDOWN]" );
				  break;
			  case GLUT_KEY_HOME:
				  keydata.home = status;     
				  printf( "[HOME]" );
				  break;
			  case GLUT_KEY_END:
				  keydata.end = status;      
				  printf( "[END]" );
				  break;
			  case GLUT_KEY_INSERT:   keydata.insert = status;   
				  printf( "[INSERT]" );
				  break;
			}
		}
	}
	if( isShiftKeyDown() ) printf( "+[SHIFT]" );
	if( isCtrlKeyDown() )  printf( "+[CTRL]" );
	if( isAltKeyDown() )   printf( "+[ALT]" );
	printf( " = %d\n", status );
	return;
}
void funcKeyDown( int key, int x, int y)
{
	_funcKey( key, x, y, true );
	//-------------------- begin user code
    switch( key ){
	  case GLUT_KEY_F1: // [F1] key
		
		break;
	  case GLUT_KEY_F2: // [F2] key
		
		break;
	  case GLUT_KEY_F3: // [F3] key
		
		break;
	  case GLUT_KEY_F4: // [F4] key
		
		break;
	  case GLUT_KEY_F12: // [F12] key
        
		break;
      case GLUT_KEY_LEFT: // cursor key - left
		
        break;
      case GLUT_KEY_RIGHT:// cursor key - right
		
        break;
      case GLUT_KEY_UP:   // cursor key - up

        break;
      case GLUT_KEY_DOWN: // cursor key - down

        break;
      default:
        break;
    }
	//---------------- end user code
    return;
}
void funcKeyUp( int key, int x, int y )
{
	_funcKey( key, x, y, false );
	//-------------------- begin user code
    switch( key ){
      case GLUT_KEY_LEFT: // cursor key - left
        
        break;
      case GLUT_KEY_RIGHT:// cursor key - right
        
        break;
	  case GLUT_KEY_UP:   // cursor key - up
        
        break;
      case GLUT_KEY_DOWN: // cursor key - down
        
        break;
      default:
        break;
    }
	//------------------ end user code
	return;
}

/******** end of file ********/
