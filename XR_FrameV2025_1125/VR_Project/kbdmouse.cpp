/******************************************************************************
 * keybdmouse.cpp: keyboard and mouse callback functions 
 */
#include <stdlib.h>
#include <stdio.h>

#include "platform.h"

#include "common.h"

#include "sim.h"

extern SimDataT simdata;
extern WindowDataT window;

//-------- modifier key status
bool isShiftKeyDown( void );
bool isCtrlKeyDown( void );
bool isAltKeyDown( void );

/*-------------------------------------------------- modifier key status
 * Shift, Ctrl, Alt key status
 */
bool isShiftKeyDown( void )
{ return (bool)(glutGetModifiers() & GLUT_ACTIVE_SHIFT); }
bool isCtrlKeyDown( void )
{ return (bool)(glutGetModifiers() & GLUT_ACTIVE_CTRL); }
bool isAltKeyDown( void )
{ return (bool)(glutGetModifiers() & GLUT_ACTIVE_ALT); }

/*------------------------------------------------------------- charKeyDown/Up
 * charKeyDown/Up - GLUT keyboard callback function
 * key: ASCII code
 * x, y: mouse position
 *--------*/
void charKeyDown( unsigned char key, int x, int y )
{
    switch( key ){
      case 'h': // help
        printf( "Instruction\n" );
        printf( "[H]:Help\n" );
        printf( "[Q]:Quit\n" );
        break;
      case 'q': // quit
        exit(0);
        break;

	  case 'r':
		simdata.sphere.red = 1.0;
		simdata.sphere.green = 0.0;
		simdata.sphere.blue = 0.0;
		break;

	  case 'g':
		simdata.sphere.red = 0.0;
		simdata.sphere.green = 1.0;
		simdata.sphere.blue = 0.0;
		break;

	  case 'b':
		simdata.sphere.red = 0.0;
		simdata.sphere.green = 0.0;
		simdata.sphere.blue = 1.0;
		break;

	  case '1':
		simdata.cube.state = 1;
		break;

	  case '2':
		  simdata.cube.state = 3;
		  break;

	  case '3':
		  simdata.cube.state = 5;
		  break;

	  case ' ': //スペースキーを押したとき
		  simdata.cube.state = 7;
		  break;

	  case 'a': //左旋回
		  //simdata.player.x -= 0.05;
		  simdata.player.turn += 0.05;
		  break;
	  case 's': //後退
		  //simdata.player.z += 0.05;
		  simdata.player.move += -0.001;
		  break;
      case 'd': //右旋回
		  //simdata.player.x += 0.05;
		  simdata.player.turn -= 0.05;
		  break;
	  case 'w': //前進
		  //simdata.player.z -= 0.05;
		  simdata.player.move += 0.001;
		  break;

      default:
        break;
    }
    return;
}
void charKeyUp( unsigned char key, int x, int y )
{
    switch( key ){
	  default:
		break;
	}
}
/*--------------------------------------------------------------- funcKeyDown/Up
 * funcKeyDown/Up - GLUT special key callback
 * key: GLUT key code
 * x, y: mouse position
 */
void funcKeyDown( int key, int x, int y )
{
	//ファンクションキー
	//[F*]: GLUT_KEY_F*(*は1～12)
	//[F1]: GLUT_KEY_F1
	//[F12]:GLUT_KEY_F12
	//その他の機能キー
    //[PageUp]  : GLUT_KEY_PAGE_UP:
    //[PageDown]: GLUT_KEY_PAGE_DOWN:
    //[Home]    : GLUT_KEY_HOME:
    //[End]     : GLUT_KEY_END:
    //[Insert]  : GLUT_KEY_INSERT:

	//カーソルキー
	switch( key ){
	  case GLUT_KEY_LEFT: //[←]

		break;
  	  case GLUT_KEY_RIGHT://[→]

		break;
	  case GLUT_KEY_UP: //[↑]

		break;
	  case GLUT_KEY_DOWN://[↓]

		break;
	}
}
void funcKeyUp( int key, int x, int y )
{
	switch( key ){
	  case GLUT_KEY_LEFT: //[←]

		break;
  	  case GLUT_KEY_RIGHT://[→]

		break;
	  case GLUT_KEY_UP: //[↑]

		break;
	  case GLUT_KEY_DOWN://[↓]

		break;
	}
}
/*------------------------------------------------------------- mouseClick
 * mouseClick - GLUT mouse callback function
 * button: mouse button
 * state：press or release
 * x, y：mouse position
 *--------*/
void mouseClick( int button , int state, int x, int y )
{
    switch( button ){
      case GLUT_LEFT_BUTTON:
        switch( state ){
          case GLUT_DOWN: // press
        
            break;
          case GLUT_UP: // release

            break;
          default:
            break;
        }
        break;
      case GLUT_MIDDLE_BUTTON:
        switch( state ){
          case GLUT_DOWN: // press

            break;
          case GLUT_UP: // release

            break;
          default:
            break;
        }
        break;
      case GLUT_RIGHT_BUTTON:
        switch( state ){
          case GLUT_DOWN: // press

            break;
          case GLUT_UP: // release

            break;
          default:
            break;
        }
        break;
      default:
        break;
    }
    return;
}
/*---------------------------------------------------------------- mouseDrag 
 * mouseDrag - mouse drag callback function
 */
void mouseDrag( int x, int y )
{
	////////


	////////
    return;
}
/*-------------------------------------------------------------- mouseMotion
 * mouseMotion - passive mouse motion callback function
 */
void mouseMotion( int x, int y )
{
	////////


	////////
    return;
}