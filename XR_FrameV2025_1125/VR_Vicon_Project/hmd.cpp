/******************************************************************************
* hmd.cpp
*/

#include "platform.h"

#include "common.h"
#include "sim.h"
#include "light.h"
#include "config.h"

#include "WarpTex.h"

 //グローバル変数
WarpTex g_DC_right, g_DC_left;
float g_cx = 0;
float g_cy = 0;
float g_kR = -0.0005;
float g_kG = -0.0005;
float g_kB = -0.0005;

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
/*-----------------------------------------------------------------HMD_Config
* Config HMD
*/
void HMD_Config(void)
{
	g_DC_left.init();
	//g_cx = window.width/4;
	//g_cy = window.height/2;
	g_DC_left.setSize(window.width / 2, window.height);
	g_DC_left.setCenter(g_cx, g_cy);
	g_DC_right.init();
	g_DC_right.setSize(window.width / 2, window.height);
	g_DC_right.setCenter(g_cx, g_cy);
	g_DC_left.setParams(g_kR, g_kG, g_kB);
	g_DC_left.setParams(g_kR, g_kG, g_kB);
	g_DC_left.setParams(g_kR, g_kG, g_kB);
	g_DC_right.setParams(g_kR, g_kG, g_kB);
	g_DC_right.setParams(g_kR, g_kG, g_kB);
	g_DC_right.setParams(g_kR, g_kG, g_kB);
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
	ezBackground(&simdata);

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
