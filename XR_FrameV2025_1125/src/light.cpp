#include "platform.h"
#include "sim.h"
/*------------------------------------------------------------ MaterialColor
 * MaterialColor:
 *--------*/
void MaterialColor( float r, float g, float b )
{
    float diffuse[4];
    float specular[] = { 0.8, 0.8, 0.8, 1.0 };

	diffuse[0] = r;
	diffuse[1] = g;
	diffuse[2] = b;
	diffuse[3] = 1.0;

    glMaterialfv( GL_FRONT, GL_DIFFUSE, diffuse );
    glMaterialfv( GL_FRONT, GL_SPECULAR, specular );
    glMaterialf( GL_FRONT, GL_SHININESS, 32.0 );

    return;
}
/*----------------------------------------------------------------- setLight
 * MainLight:
 *--------*/
void MainLight( int light_id, float r, float g, float b )
{
	//int light_id = GL_LIGHT0;
	float light_color[] = { 1.0, 1.0, 1.0, 1.0 };
    //float light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
    float light_pos[] = { 0.0, 10.0, 0.0, 0.0 };

	light_color[0] = r;
	light_color[1] = g;
	light_color[2] = b;
 	glEnable( light_id );
    glLightfv( light_id, GL_DIFFUSE, light_color );
    glLightfv( light_id, GL_POSITION, light_pos );
	//glLightfv( light_id, GL_AMBIENT, light_ambient );

    return;
}
/*----------------------------------------------------------------- setLight
 * SubLight:
 *--------*/
void SubLight( int light_id, float r, float g, float b )
{
	//int light_id = GL_LIGHT1;
    float light_color[]   = { 0.2, 0.2, 0.2, 1.0 };
    //float light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
    float light_pos[]     = { 0.0, -10.0, 0.0, 0.0 };

	light_color[0] = r;
	light_color[1] = g;
	light_color[2] = b;
	glEnable( light_id );  
    glLightfv( light_id, GL_DIFFUSE, light_color );
    glLightfv( light_id, GL_POSITION, light_pos );
	//glLightfv( light_id, GL_AMBIENT, light_ambient );

    return;
}
/*------------------------------------------------------------------- SpotLight
 * SpotLight
 * light_id; GL_LIGHT0-7
 * r, g, b: light color
 * att_half: attenuation range
 * cutoff: cutoff angle [0,180][degree]
 * exponent: distribution exponent 
 *--------*/
void SpotLight( int light_id, float r, float g, float b, float att_half, 
	float cutoff, int exponent )
{
	//int light_id = GL_LIGHT2;
    float light_color[]   = { 0.25, 0.25, 0.25, 1.0 };
    //float light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
    float light_pos[]     = { 0.0, 0.0, 0.0, 1.0 };
	float light_dir[]     = { 0.0, 0.0, -1.0 };

	light_color[0] = r;
	light_color[1] = g;
	light_color[2] = b;
	glEnable( light_id );  
    glLightfv( light_id, GL_DIFFUSE, light_color );
	//glLightfv( light_id, GL_AMBIENT, light_ambient );
    glLightfv( light_id, GL_POSITION, light_pos );
	glLightfv( light_id, GL_SPOT_DIRECTION, light_dir );

	glLightf( light_id, GL_SPOT_CUTOFF, cutoff ); // cutoff < 180
    glLighti( light_id, GL_SPOT_EXPONENT, exponent );

	// ATTENUATION
	float att_const, att_linear, att_quad;
	att_linear = 1.0 / att_half;  // linear attenuation factor
	att_const = 1.0 - att_linear; // constant attenuation factor
	att_quad = 0.0;               // quadratic attenuation factor
	glLightf( light_id, GL_CONSTANT_ATTENUATION, att_const );
	glLightf( light_id, GL_LINEAR_ATTENUATION, att_linear );
	glLightf( light_id, GL_QUADRATIC_ATTENUATION, att_quad );

	// 
    return;
}
/*----------------------------------------------------------------- PointLight
 * PointLight
 * light_id; GL_LIGHT0-7
 * r, g, b: light color
 * att_half: attenuation range
 *--------*/
void PointLight( int light_id, float r, float g, float b, float att_half )
{
	//int light_id = GL_LIGHT2;
    float light_color[]   = { 0.25, 0.25, 0.25, 1.0 };
    //float light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
    float light_pos[]     = { 0.0, 0.0, 0.0, 1.0 };

	light_color[0] = r;
	light_color[1] = g;
	light_color[2] = b;
	glEnable( light_id );  
    glLightfv( light_id, GL_DIFFUSE, light_color );
    glLightfv( light_id, GL_POSITION, light_pos );
	//glLightfv( light_id, GL_AMBIENT, light_ambient );

	// ATTENUATION
	float att_const, att_linear, att_quad;
	att_linear = 1.0 / att_half;  // linear attenuation factor
	att_const = 1.0 - att_linear; // constant attenuation factor
	att_quad = 0.0;               // quadratic attenuation factor
	glLightf( light_id, GL_CONSTANT_ATTENUATION, att_const );
	glLightf( light_id, GL_LINEAR_ATTENUATION, att_linear );
	glLightf( light_id, GL_QUADRATIC_ATTENUATION, att_quad );
    return;
}
/*------------------------------------------------------------- setHeadLight
 * HeadLight
 * light_id; GL_LIGHT0-7
 * r, g, b: light color
 * att_half: attenuation range
 *--------*/
void HeadLight( int light_id, float r, float g, float b, float att_half )
{
	//int light_id = GL_LIGHT2;
    float light_color[]   = { 0.25, 0.25, 0.25, 1.0 };
    //float light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
    float light_pos[]     = { 0.0, 0.0, 0.0, 1.0 };

	light_color[0] = r;
	light_color[1] = g;
	light_color[2] = b;
	glEnable( light_id );  
    glLightfv( light_id, GL_DIFFUSE, light_color );
    glLightfv( light_id, GL_POSITION, light_pos );
	//glLightfv( light_id, GL_AMBIENT, light_ambient );

	// ATTENUATION
	float att_const, att_linear, att_quad;
	att_linear = 1.0 / att_half;  // linear attenuation factor
	att_const = 1.0 - att_linear; // constant attenuation factor
	att_quad = 0.0;               // quadratic attenuation factor
	glLightf( light_id, GL_CONSTANT_ATTENUATION, att_const );
	glLightf( light_id, GL_LINEAR_ATTENUATION, att_linear );
	glLightf( light_id, GL_QUADRATIC_ATTENUATION, att_quad );
    return;
}
/*------------------------------------------------------------------- setFog */
/* Fog: 霧を設定する（簡易設定）
 */
void Fog( int fog_mode, float r, float g, float b, float d, float start, float end )
{
	float fog_color[4] = { 0.0, 0.0, 0.0, 1.0 };
    float density;

	fog_color[0] = r;
	fog_color[1] = g;
	fog_color[2] = b;

	if( d > 0.0 ) glEnable( GL_FOG );
	glFogi(GL_FOG_MODE, fog_mode); //種類 GL_LINEAR, GL_EXP, GL_EXP2
	glFogfv(GL_FOG_COLOR, fog_color); //色

	switch( fog_mode ){
	case GL_LINEAR:
		density = 1.0;
	    glFogf(GL_FOG_START, start );  //開始位置
	    glFogf(GL_FOG_END, end ); //終了位置
		break;
	case GL_EXP:
		density = 1.0 / end;
		break;
	case GL_EXP2:
		density = 1.0 / ( end * end );
		break;
	}
	glFogf(GL_FOG_DENSITY, density * d );  //密度
	//glHint(GL_FOG_HINT, GL_DONT_CARE); //ヒント
}
void setFog(SimDataT *simdata)
{
	//▼フォグON: R, G, B, 密度, 開始距離, 終了距離
	Fog(GL_EXP,
		simdata->fog.red,
		simdata->fog.green,
		simdata->fog.blue,
		simdata->fog.alpha,
		simdata->clip_near, simdata->clip_far * 0.5);
}
//-----------------------------------------------------------------
static inline float _blend(float k, float a, float b)
{
	return (k * a + (1.0 - k) * b);
}
void ezBackground(SimDataT* simdata)
{
	glClearColor(
		_blend(simdata->sky.alpha, simdata->sky.red, simdata->fog.red),
		_blend(simdata->sky.alpha, simdata->sky.green, simdata->fog.green),
		_blend(simdata->sky.alpha, simdata->sky.blue, simdata->fog.blue),
		1.0);
}