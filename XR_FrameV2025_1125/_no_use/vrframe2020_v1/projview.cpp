#include "platform.h"

#include "common.h"
#include "sim.h"

extern SimDataT simdata;
extern WindowDataT window;

/*---------------------------------------------------------------- Viewing
 * Viewing:
 *--------*/
void Viewing( void )
{
	//主観カメラの場合
    //gluLookAt( 0.0, 1.0, 5.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0 );

	applyObjInverse( &simdata.head );
	/*
	glRotatef( - simdata.head.roll, 0.0, 0.0, 1.0 ); //rollの逆変換
	glRotatef( - simdata.head.pitch, 1.0, 0.0, 0.0 ); //pitchの逆変換
	glRotatef( - simdata.head.yaw, 0.0, 1.0, 0.0 ); //yawの逆変換
	glTranslatef( - simdata.head.x, - simdata.head.y, - simdata.head.z );
	*/
	//applyObjInverse( &simdata.player );
	/*	
	glRotatef( - simdata.active_camera->roll, 0.0, 0.0, 1.0 );
	glRotatef( - simdata.active_camera->pitch, 1.0, 0.0, 0.0 );
	glRotatef( - simdata.active_camera->yaw, 0.0, 1.0, 0.0 );
	//分かりやすいように改行していますが、改行なしでも構いません。
	glTranslatef( 	- simdata.active_camera->x,
			- simdata.active_camera->y, 
			- simdata.active_camera->z );
	*/
	//----------------------------------------------------
	//客観カメラ（固定カメラ）でプレイヤを注視する場合はこちら
	//gluLookAt( 0.0, 1.0, 3.0, simdata.player.x, simdata.player.y, simdata.player.z, 0.0, 1.0, 0.0 );
}
/*---------------------------------------------------------------- Projection
 * Projection:
 *--------*/
void Projection( void )
{
	//---- begin projection transformation
    gluPerspective( 45.0, window.aspect, simdata.clip_near, simdata.clip_far );
}
