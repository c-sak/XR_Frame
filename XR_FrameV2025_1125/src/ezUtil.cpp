//##############################################################################
/* ezUtil.cpp
 * 
 * SAMPLE CODE FOR INTERACTIVE CG PROGRAMMING COURSES
 * AT COLLEGE OF IMAGE ARTS AND SCIENCES, RITSUMEIKAN UNIVERSITY
 * COPYRIGHT(C)2008 TOSHIKAZU OHSHIMA, ALL RIGHTS RESERVED
 */
//##############################################################################

#include "platform.h"
/*
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
*/
#include "ezUtil.h"
//#include "ezFont.h"

#define EZ_PI 3.1415926
#define EZ_RADIAN (EZ_PI/180.0)
#define EZ_DEGREE (180.0/EZ_PI)

/********
uniformRandom -> calc.cpp
◆描画系関数はmymodels.cppに順次移行◆
・ezUtil_makeStars -> makeStars/mymodels.cpp
・ezUtil_drawStars -> drawStars/mymodels.cpp


▼HUD用情報提示＊試作中
・ezUtil_drawCircle
・ezUtil_drawScale

▼MREALターゲット対応版
回転変換≫回転角度＋回転軸ベクトル
・ezUtil_postMultInv2
・ezUtil_postMult2
*////////

//--------------------------------------------------------------------- drawGrid
/* drawGrid: グリッドを描画する
 */
void ezUtil_drawGrid( int n, float range )
{
    int i;
    float x, y;
    float d = range/(float)n;
    float min = -0.5 * range;
    float max =  0.5 * range;

    glPushMatrix();
    glRotatef( -90.0, 1.0, 0.0, 0.0 );

    x = min;
    for( i=0; i<n; i++ ){
        glBegin( GL_LINES );
        glVertex2f( x, min );
        glVertex2f( x, max );
        glEnd();
        x += d;
    }
    y = min;
    for( i=0; i<n; i++ ){
        glBegin( GL_LINES );
        glVertex2f( min, y );
        glVertex2f( max, y );
        glEnd();
        y += d;
    }
    glPopMatrix();
    return;
}
//--------------------------------------------------------------------- drawAxes
/* drawAxes: 座標軸を描画する
 */  
void ezUtil_drawAxes( float l )
{
	glNormal3f( 0.0, 0.0, 0.0 );
    /* x軸を赤で描画する */
    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_LINES);
    glVertex3f(-l, 0.001, 0.0 );
    glVertex3f( l, 0.001, 0.0 );
    glEnd();
    /* y軸を緑で描画する */
    glColor3f(0.0, 1.0, 0.0); 
    glBegin(GL_LINES);
    glVertex3f( 0.0, l, 0.0 );
    glVertex3f( 0.0,-l, 0.0 );
    glEnd();
    /* z軸を青で描画する */
    glColor3f(0.0, 0.0, 1.0); 
    glBegin(GL_LINES);
    glVertex3f( 0.0, 0.001, l );
    glVertex3f( 0.0, 0.001, -l );
    glEnd();
    return;
}
/*----------------------------------------------------------------- drawCircle
 * drawCircle
 */
void ezUtil_drawCircle( float r, int n, bool fill )
{
	float x, y, da, a;
	int i;
	a = 0.0;
	da = 360.0 / n * EZ_RADIAN;
	if( fill ) glBegin( GL_POLYGON );
	else       glBegin( GL_LINE_LOOP);
	for( i=0; i<n; i++ ){
		x = r * cosf( a );
		y = r * sinf( a );
		glVertex2f( x, y );
		a += da;
	}
	glEnd();
/*
	da = 360.0 / n;
	char angle[8];
	a = 0.0;

	for( i=0; i<n; i++ ){
		x = r * sinf( a );
		y = r * cosf( a );
		sprintf( angle, "%d", (int)a ); 
		ezFont_drawGlutStroke( x, y, 0.0, angle, 0.1, 0.1 );
		a += da;
	}
*/
	return;
}
#if 0
/*----------------------------------------------------------------- drawScale
 * drawScale
 */
void ezUtil_drawScale( float r, int n, int axis, float offset, int mode )
{
	float x, y, da, a;
	int i;
	a = 0.0;
	da = 360.0 / n * EZ_RADIAN;

	glBegin( GL_LINE_LOOP);
	for( i=0; i<n; i++ ){
		x = r * sinf( a );
		y = r * cosf( a );
		a += da;
		switch( axis ){
		  case 0:
     		glVertex3f( 0.0, y, -x );
			break;
		  case 1:
     		glVertex3f( x, 0.0, -y );
			break;
		  case 2:
     		glVertex3f( x, y, 0 );
			break;
		}

	}
	glEnd();

	da = 360.0 / n;
	char angle[8];
	a = 0.0;
	for( i=0; i<n; i++ ){
		x = r * sinf( a * EZ_RADIAN );
		y = r * cosf( a * EZ_RADIAN );
		glPushMatrix();
		switch( axis ){
		  case 0: //仰角スケール
            if( i == 3 ){
                glPushMatrix();
                glRotatef( - offset, 1.0, 0.0, 0.0 );
                glTranslatef( 0.0, 0.0, - r );
                sprintf( angle, "%3d", - (int)offset );
                glLineWidth( 2.0 );
                ezFont_drawGlutStroke( 0.02, 0.0, 0.0, angle, 0.025, 0.025 );
                glLineWidth( 1.0 );
                glPopMatrix();
            }
            glTranslatef( 0.0, y, -x );
			glRotatef( 90.0 - a, 1.0, 0.0, 0.0 );
           	sprintf( angle, "%3d", 90 - (int)a );
		    ezFont_drawGlutStroke( -0.04, 0.0, 0.0, angle, 0.025, 0.025 );
			break;
		  case 1: //方位角スケール
            if( i == 0 ){
                glPushMatrix();
                glRotatef( - offset, 0.0, 1.0, 0.0 );
                glTranslatef( 0.0, 0.0, - r );
                if( offset >= 0.0 ) sprintf( angle, "%3d", (int)offset );
                else sprintf( angle, "%3d", (int)(offset + 360.0) );
                glLineWidth( 2.0 );
                ezFont_drawGlutStroke( 0.0, -0.02, 0.0, angle, 0.025, 0.025 );
                glLineWidth( 1.0 );
                glPopMatrix();
            }
    		glTranslatef( x, 0.0, -y );
	    	glRotatef( - a, 0.0, 1.0, 0.0 );
       		sprintf( angle, "%3d", (int)a );
		  	ezFont_drawGlutStroke( 0.0, 0.01, 0.0, angle, 0.025, 0.025 );
			break;
		  case 2: //
      	    ezFont_drawGlutStroke( x, y, 0.0, angle, 0.025, 0.025 );
			break;
		}
		glPopMatrix();
     	a += da;
	}
	return;
}
#endif
//-------------------------------------------------------------------- transform
/* transform:
 */
void ezUtil_transform( float x, float y, float z,
                       float roll, float pitch, float yaw )
{
    glTranslatef( x, y, z );
    glRotatef( yaw, 0., 1., 0. );
    glRotatef( pitch, 1., 0., 0. );
    glRotatef( roll, 0., 0., 1. );
    return;
}
void ezUtil_transform( float *pos, float *rot )
{
	ezUtil_transform( pos[0], pos[1], pos[2], rot[0], rot[1], rot[2] );
}
//---------------------------------------------------------------------- inverse
/* inverse:
 */
void ezUtil_inverse( float x, float y, float z,
                     float roll, float pitch, float yaw )
{
    glRotatef( -roll, 0., 0., 1. );
    glRotatef( -pitch, 1., 0., 0. );
    glRotatef( -yaw, 0., 1., 0. );
    glTranslatef( -x, -y, -z );
    return;
}
void ezUtil_inverse( float *pos, float *rot )
{
	ezUtil_inverse( pos[0], pos[1], pos[2], rot[0], rot[1], rot[2] );
}
//----------------------------------------------------------------- postMultInv
void ezUtil_postMultInv( float *base_pos, float *base_rot,
	float *target_pos, float *target_rot,
	float *out_pos, float *out_rot )
{
	float mat[16];
	glPushMatrix();
	glLoadIdentity();
	ezUtil_inverse( base_pos, base_rot );
	ezUtil_transform( target_pos, target_rot );
	glGetFloatv( GL_MODELVIEW_MATRIX, mat );
	ezUtil_getRot( mat, out_rot );
	ezUtil_getPos( mat, out_pos );
	glPopMatrix();
}
//----------------------------------------------------------------- postMultInv
void ezUtil_postMultInv2( float *base_pos, float *base_rot,
	float *target_pos, float *target_rot,
	float *out_pos, float *out_rot )
{
	float mat[16];
	glPushMatrix();
	glLoadIdentity();

	glRotatef( - base_rot[3], base_rot[0], base_rot[1], base_rot[2] );
	glTranslatef( - base_pos[0], - base_pos[1], - base_pos[2] );
	
	glTranslatef( target_pos[0], target_pos[1], target_pos[2] );
	glRotatef( target_rot[3], target_rot[0], target_rot[1], target_rot[2] );

	glGetFloatv( GL_MODELVIEW_MATRIX, mat );
	ezUtil_getRot( mat, out_rot );
	ezUtil_getPos( mat, out_pos );
	glPopMatrix();
}


void ezUtil_Mult( float *base_pos, float *base_rot,
	float *target_pos, float *target_rot,
	float *out_pos, float *out_rot )
{
	float mat[16];
	glPushMatrix();
	glLoadIdentity();
	ezUtil_transform( base_pos, base_rot );
	ezUtil_transform( target_pos, target_rot );
	glGetFloatv( GL_MODELVIEW_MATRIX, mat );
	ezUtil_getRot( mat, out_rot );
	ezUtil_getPos( mat, out_pos );
	glPopMatrix();
}


void ezUtil_Mult2( float *base_pos, float *base_ori,
	float *target_pos, float *target_rot,
	float *out_pos, float *out_rot )
{
	float mat[16];
	glPushMatrix();
	glLoadIdentity();
	glTranslatef( base_pos[0], base_pos[2], base_pos[3] );
	glRotatef( base_ori[0], base_ori[1], base_ori[2], base_ori[3] );
	ezUtil_transform( target_pos, target_rot );
	glGetFloatv( GL_MODELVIEW_MATRIX, mat );
	ezUtil_getRot( mat, out_rot );
	ezUtil_getPos( mat, out_pos );
	glPopMatrix();
}


//----------------------------------------------------------------------- getRot
/* getRot:
 */
void ezUtil_getRot( const float src[], float *rot )
{
	ezUtil_getRot( src, &rot[0], &rot[1], &rot[2] );
}
void ezUtil_getRot( const float src[],
                       float *roll, float *pitch, float *yaw )
{
    float x[4], y[4], z[4];
    float buf[4];
    float m[4][4];
    memcpy( (void*)m, (void*)src, sizeof( m ) );
    /*---- GET AXES ----*/
    x[0] = m[0][0]; x[1] = m[0][1]; x[2] = m[0][2]; x[3] = 1.0;
    y[0] = m[1][0]; y[1] = m[1][1]; y[2] = m[1][2]; y[3] = 1.0;
    z[0] = m[2][0]; z[1] = m[2][1]; z[2] = m[2][2]; z[3] = 1.0;
    /*---- PITCH ----*/
    *pitch = EZ_DEGREE * asinf( - z[1] ); // [-90,90]
    /*---- YAW & ROLL ----*/
    if( -90.0 < *pitch && *pitch < 90.0 ){
        *yaw = EZ_DEGREE * atan2f( z[0], z[2] ); // [-180,180]
        buf[0] =   z[2];
        buf[1] =    0.;
        buf[2] = - z[0];
        buf[3] =    1.;
        float dot = x[0]*buf[0] + x[1]*buf[1] + x[2]*buf[2];
        float norm = sqrtf( (x[0]*x[0]+x[1]*x[1]+x[2]*x[2])
                            *(buf[0]*buf[0]+buf[1]*buf[1]+buf[2]*buf[2]) );
        float c = dot / norm;
        if( c  < -1. ) c = -1.;
        if( 1. <  c  ) c =  1.;
        *roll = EZ_DEGREE * acosf( c );
        if( x[1] < 0. ) *roll = - *roll;
    }
    else{
        if( z[1] < 0. ) *yaw = EZ_DEGREE * atan2f(   y[0],   y[2] );
        else            *yaw = EZ_DEGREE * atan2f( - y[0], - y[2] );
        *roll = 0.;
    }
    if( *roll  == -0. ) *roll  = 0.;
    if( *pitch == -0. ) *pitch = 0.;
    if( *yaw   == -0. ) *yaw   = 0.;
    return;
}
//----------------------------------------------------------------------- getPos
/* getPos:
 */
void ezUtil_getPos( const float matrix[], float *pos )
{
	ezUtil_getPos( matrix, &pos[0], &pos[1], &pos[2] );
}
void ezUtil_getPos( const float matrix[], float *x, float *y, float *z )
{
    *x = matrix[12];//[3][0]; x座標値を抽出
    *y = matrix[13];//[3][1]; y座標値を抽出
    *z = matrix[14];//[3][2]; z座標値を抽出
    return;
}

//==============================================================================
