/******************************************************************************
* hmd.cpp
*/

#include "platform.h"

#include "common.h"
#include "sim.h"

#include "config.h"

#include "WarpTex.h"

 //グローバル変数
extern WarpTex g_DC, g_DC_right, g_DC_left;
extern float g_cx;
extern float g_cy;
extern float g_kR;
extern float g_kG;
extern float g_kB;

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
void HMD_View( void )
{
	float ipd = 0.06;//0.0625
	float left, right;

	//float h = 0.072;
	//float w = 0.065;
	float h = 0.12;
	float w = 0.1075; //テスト

	float bottom = - h/2.0;
	float top = h/2.0;
	float znear = 0.03;
	float zfar = 100.0;

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
	glViewport( 0, 0, window.width/2.0, window.height );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	if (distortion) { //★
		left = -w / 2.0;
		right = w / 2.0;
	}
	else {
		left = - w + ipd/2.0;
		right = ipd/2.0;
	}

	glFrustum( left, right, bottom, top, znear, zfar );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	glPushMatrix();
	glTranslatef( ipd/2.0, 0.0, 0.0 );

	Viewing();
	//-------- head light
	HeadLight();
	//-------- draw
	Lighting();
	DrawScene();
	glPopMatrix();

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	//★歪み補正実行
	if( distortion ){
		g_DC_left.Update();
		//g_DC_left.runDC();
	}

	//▼右眼画像
	glEnable(GL_DEPTH_TEST); // ---- begin: 
	glEnable(GL_LIGHTING);

	if (distortion) {

	}
	else {
		glViewport(window.width / 2.0 + 1, 0, window.width / 2.0, window.height);
	}
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	if (distortion) {//★
		left = -w / 2.0;
		right = w / 2.0;
	}
	else {
		left = - ipd/2.0;
		right = w - ipd/2.0;
	}

	glFrustum( left, right, bottom, top, znear, zfar );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	glPushMatrix();
	glTranslatef( -ipd/2.0, 0.0, 0.0 );

	Viewing();
	//-------- head light
	HeadLight();
	//-------- draw
	Lighting();
	DrawScene();
	glPopMatrix();

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	//PostDraw();

	//★歪み補正実行
	if( distortion ){
		g_DC_right.Update();
		//g_DC_right.runDC();
	}

	//★歪補正描画
	if (distortion) {
		glViewport(0, 0, window.width, window.height);
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glViewport(0, 0, window.width / 2, window.height);
		g_DC_left.Draw( (w / 2.0 - ipd / 2.0) / w, 0.0, 1.0 );

		glViewport(window.width / 2 + 1, 0, window.width / 2, window.height);
		g_DC_right.Draw( (-w / 2.0 + ipd / 2.0) / w, 0.0, 1.0 );
	}
	//-------- swapbuffers
	glutSwapBuffers();

	return;
}
/******** end of file ********/
