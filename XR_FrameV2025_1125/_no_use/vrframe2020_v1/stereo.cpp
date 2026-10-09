/******************************************************************************
* stereo.cpp 
*/

#include "platform.h"

#include "common.h"
#include "sim.h"
#include "config.h"
//設定フラグ参照＞quadbuffer, swapeyes, cylindrical, parallax

extern WindowDataT window;
extern SimDataT simdata;

// projview.cpp
void Viewing( void );
void Projection( void );

// cylindrical.cpp
void cylindricalView( float dx );

// main.cpp
void singleView( float d );

//-------- light.cpp
void MainLight(int light_id, float r, float g, float b);
void SubLight(int light_id, float r, float g, float b);
void HeadLight(int light_id, float r, float g, float b, float att);
void Fog(int fog_mode, float r, float g, float b, float d, float start, float end);
void SpotLight(int light_id, float r, float g, float b, float att_half,
	float cutoff, int exponent);

void HeadLight(void);
void Lighting(void);

inline float _blend(float k, float a, float b)
{
	return (k * a + (1.0 - k) * b);
}

/*------------------------------------------------------------------- display
* display - GLUT display callback function
*--------*/
void Stereo( void )
{
	float ipd = parallax;

	if( swapeyes ) ipd *= -1.0;

	//▼背景色とフォグカラーをブレンド
	glClearColor(
		_blend(simdata.sky_color[3], simdata.sky_color[0], simdata.air_color[0]),
		_blend(simdata.sky_color[3], simdata.sky_color[1], simdata.air_color[1]),
		_blend(simdata.sky_color[3], simdata.sky_color[2], simdata.air_color[2]),
		1.0);

	//-------- viewport
	glViewport( 0, 0, window.width, window.height );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	//PreDraw();
	glEnable(GL_DEPTH_TEST); // ---- begin: 
	glEnable(GL_LIGHTING);

	//▼左眼画像
	if( quadbuffer )
		glDrawBuffer( GL_BACK_LEFT );
	else
		glColorMask( GL_FALSE, GL_TRUE, GL_TRUE, GL_TRUE ); //カラーマスクG+B

	glViewport( 0, 0, window.width, window.height );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	if( cylindrical ) cylindricalView( -ipd/2.0 );
	else singleView( -ipd/2.0 );

	//▼右眼画像
	if( quadbuffer )
		glDrawBuffer( GL_BACK_RIGHT );
	else
		glColorMask( GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE ); //カラーマスクR

	//▼anaglyphの場合にはデプスバッファのみクリア
	if( ! quadbuffer ) glClear( GL_DEPTH_BUFFER_BIT );
	else{
		glViewport( 0, 0, window.width, window.height );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );			
	}

	if( cylindrical ) cylindricalView( ipd/2.0 );
	else singleView( ipd/2.0 );

	//▼ドローバッファ(quad)/カラーマスク(anaglyph)を元に戻す
	if( quadbuffer )
		glDrawBuffer( GL_BACK );
	else
		glColorMask( GL_TRUE,  GL_TRUE, GL_TRUE, GL_TRUE );//カラーマスクを戻す

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	//PostDraw();

	//-------- swapbuffers
	glutSwapBuffers();

	return;
}
/******** end of file ********/
