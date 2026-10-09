/******************************************************************************
 * glut_mouse.cpp 
 */

#include "platform.h"

#include "common.h"
#include "sim.h"

extern SimDataT simdata;
extern WindowDataT window;

MouseDataT mouse;

static void _mouse( int x, int y )
{
	mouse.xp = mouse.xAbs;
	mouse.yp = mouse.yAbs;
	mouse.xAbs = (float)x/window.width  * 2.0 - 1.0; //normalize x position
	mouse.yAbs = (float)y/window.height * 2.0 - 1.0; //normalize y position

	mouse.dx = mouse.xAbs - mouse.xp;
	mouse.dy = mouse.yAbs - mouse.yp;
	mouse.xRel = mouse.xAbs - mouse.xo;
	mouse.yRel = mouse.yAbs - mouse.yo;
}
/*------------------------------------------------------------- mouseClick
 * mouseClick - GLUT mouse callback function
 * button: mouse button
 * stateÅFpress or release
 * x, yÅFmouse position
 *--------*/
void mouseClick( int button , int state, int x, int y )
{
    //printf( "mouse click: %d, %d, %d, %d\n", button, state, x, y );
	switch( state ){
	  case GLUT_DOWN:
		_mouse( x, y );
		mouse.xp = mouse.xo = mouse.xAbs;
        mouse.yp = mouse.yo = mouse.yAbs;
		mouse.xRel = mouse.dx = 0.0;
		mouse.yRel = mouse.dy = 0.0;
		break;
      case GLUT_UP:
		glutWarpPointer( ( mouse.xo + 1.0 ) * window.width / 2.0, ( mouse.yo + 1.0 ) * window.height / 2.0 );
		_mouse( x, y );
		mouse.xp = mouse.xo = mouse.xAbs;
        mouse.yp = mouse.yo = mouse.yAbs;
		mouse.xRel = mouse.dx = 0.0;
		mouse.yRel = mouse.dy = 0.0;
		break;
	}

    switch( button ){
      case GLUT_LEFT_BUTTON:
        if( state == GLUT_DOWN ){ // press
            mouse.left = true;
			// begin user code
			//moveWorldToLocal(&simdata.sphere, &simdata.cube);
			// end
		}
		else{ // GLUT_UP - release
            mouse.left = false;

        }
        break;
      case GLUT_MIDDLE_BUTTON:
        if( state == GLUT_DOWN ){ // press
            mouse.middle = true;
			// begin user code
			//simdata.sphere.pos.x += 0.05;
			// end
		}
		else{ //GLUT_UP - release
            mouse.middle = false;

        }
        break;
      case GLUT_RIGHT_BUTTON:
        if( state == GLUT_DOWN ){ // press
            mouse.right = true;
			// begin user code
			//moveLocalToWorld(&simdata.sphere);
			// end
		}
		else{ //GLUT_UP - release
            mouse.right = false;

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
    _mouse( x, y );
    //printf( "mouse drag: %d, %d\n", x, y );
    return;
}
/*-------------------------------------------------------------- mouseMotion
 * mouseMotion - passive mouse motion callback function
 */
void mouseMotion( int x, int y )
{
	mouse.x = (float)x/window.width  * 2.0 - 1.0; //normalize x position
	mouse.y = (float)y/window.height * 2.0 - 1.0; //normalize y position
    //printf( "mouse motion: %d, %d\n", x, y );
    return;
}
/******** end of file ********/
