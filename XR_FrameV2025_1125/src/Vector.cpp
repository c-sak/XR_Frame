#include "platform.h"

#include "calc.h"
//#include "Vector.h"

#include <stdio.h>
#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>

//float PI = 3.14159;
float RADIAN = M_PI / 180.0;
float DEGREE = 180.0 / M_PI;

//-------- vectorDraw
void vectorDraw(vector_t* p, vector_t* v)
{
	float l = sqrtf(v->x * v->x + v->y * v->y + v->z * v->z);
	float pitch = asinf(v->y / l) * DEGREE;
	float yaw = atan2f(-v->x, -v->z) * DEGREE;
	glPushMatrix();
	glTranslatef(p->x, p->y, p->z);
	glRotatef(yaw, 0.0, 1.0, 0.0);
	glRotatef(pitch, 1.0, 0.0, 0.0);
	glRotatef(180.0, 0.0, 1.0, 0.0);
	glutSolidSphere(0.025, 4, 3);
	glutSolidCone(0.025, l, 4, 3);
	glTranslatef(0.0, 0.0, l);
	glutSolidSphere(0.0125, 4, 3);
	glPopMatrix();
	return;
}
//-------- vectorDrawAtoB
void vectorDrawAtoB(vector_t* a, vector_t* b)
{
	vector_t v;
	v.x = b->x - a->x;
	v.y = b->y - a->y;
	v.z = b->z - a->z;
	vectorDraw(a, &v);
	return;
}
//-------- vectorPrint
void vectorPrint( const char *head, const vector_t *v )
{
  printf( "%s: %f, %f, %f\n", head, v->x, v->y, v->z);
}
//-------- vectorSet
void vectorSet( vector_t *v, float x, float y, float z )
{
  v->x = x; v->y = y; v->z = z;
}
//-------- vectorCopy
void vectorCopy( vector_t *dst, const vector_t *src )
{
  dst->x = src->x; dst->y = src->y; dst->z = src->z;
}
//-------- vectorNorm
float vectorNorm( const vector_t *v )
{
  return sqrtf( v->x*v->x + v->y*v->y + v->z*v->z );
}
//-------- vectorNorm2
float vectorNorm2( const vector_t *v )
{
  return v->x*v->x + v->y*v->y + v->z*v->z;
}
//-------- vectorNormalize
float vectorNormalize( vector_t *v )
{
  float l = vectorNorm( v );
  v->x /= l; v->y /= l; v->z /= 1;
  return l;
}
//-------- vectorDot
float vectorDot( const vector_t *a, const vector_t *b )
{
  return a->x*b->x + a->y*b->y + a->z*b->z;
}
//-------- vectorCross
void vectorCross( vector_t *aXb, const vector_t *a, const vector_t *b )
{
  aXb->x = a->y*b->z - a->z*b->y;
  aXb->y = a->z*b->x - a->x*b->z;
  aXb->z = a->x*b->y - a->y*b->x;
}
//-------- vectorAdd
void vectorAdd( vector_t *v, const vector_t *a, const vector_t *b )
{
  v->x = a->x + b->x;
  v->y = a->y + b->y;
  v->z = a->z + b->z;
}
//-------- vectorSub
void vectorSub( vector_t *v, const vector_t *a, const vector_t *b )
{
  v->x = a->x - b->x;
  v->y = a->y - b->y;
  v->z = a->z - b->z;
}
//-------- vectorScale
void vectorScale(vector_t* sv, float s, const vector_t* v)
{
	sv->x = s * v->x;
	sv->y = s * v->y;
	sv->z = s * v->z;
}
//-------- vectorMult
void vectorMult(vector_t* ab, const vector_t* a, const vector_t* b) {
	ab->x = a->x * b->x;
	ab->y = a->y * b->y;
	ab->z = a->z * b->z;
}
//-------- vectorBlend
void vectorBlend( vector_t *v, float k, const vector_t *a, const vector_t *b )
{
  float _k = 1.0 - k;
  v->x = k* a->x + _k* b->x;
  v->y = k* a->y + _k* b->y;
  v->z = k* a->z + _k* b->z;
}
//-------- vectorAngle
float vectorAngle( const vector_t *a, const vector_t *b )
{
  return acosf( vectorDot( a, b ) / sqrtf( vectorNorm2( a ) * vectorNorm2( b ) ) ) * DEGREE;
}
//-------- vectorAngleRef
float vectorAngleRef( const vector_t *a, const vector_t *b, const vector_t *ref )
{
  float angle;
  vector_t aXb, refXa, refXb;

  vectorCross(&refXa, ref, a);
  vectorCross(&refXb, ref, b);
  vectorCross(&aXb, a, b);

  angle = vectorAngle( &refXa, &refXb);
  vectorCross( &aXb, &refXa, &refXb );

  if( vectorAngle( ref, &aXb ) < 90 ) return angle;
  else return - angle;
  return angle;
}
//-------- vectorXform
void vectorXform( vector_t *vo, const vector_t *vi, const matrix_t m ){
  vo->x = vi->x * m[0] + vi->y * m[4] + vi->z * m[8] + m[12];
  vo->y = vi->x * m[1] + vi->y * m[5] + vi->z * m[9] + m[13];
  vo->z = vi->x * m[2] + vi->y * m[6] + vi->z * m[10] + m[14];
}
//-------- eulerSet
void eulerSet( euler_t *rot, float roll, float pitch, float yaw ){
  rot->roll = roll; rot->pitch = pitch; rot->yaw = yaw;
}
//-------- eulerCopy
void eulerCopy(euler_t* dst, const euler_t* src) {
	dst->roll = src->roll;
	dst->pitch = src->pitch;
	dst->yaw = src->yaw;
}
//========
//▼方向ベクトルから方位角と仰角を求める関数．戻り値はベクトルの大きさ
float VectorToPolar(vector_t* v, euler_t* angle)
{
	float d = sqrtf(v->x * v->x + v->z * v->z);
	angle->pitch = DEGREE * atan2f(v->y, d);
	angle->yaw = DEGREE * atan2f(-v->x, -v->z);
	return vectorNorm(v);
}
//▼方位角と仰角から単位方向ベクトルを求める関数
void PolarToVector(euler_t* angle, vector_t* v)
{
	float rad, l;

	rad = angle->pitch * RADIAN;
	v->y = sinf(rad);
	l = cosf(rad);

	rad = angle->yaw * RADIAN;
	v->x = -l * sinf(rad);
	v->z = -l * cosf(rad);

	return;
}
//----------------
//====================================================================================================
//▼乱数生成関数
//最小値minと最大値maxの間の値をランダムに返す
float uniformRandom(float min, float max) {
	double r;
	r = (double)rand() / (double)RAND_MAX;
	r = r * (max - min) + min;
	return r;
}
//▼簡易ガウスノイズ関数
float gaussianRandom(float rmin, float rmax) {
	int i;
	float r = 0.0;
	const int n = 12;
	for (i = 0; i < n; i++) {
		r += uniformRandom(rmin, rmax);
	}
	return r / n;
}
//▼整数型ランダム関数
int diceRandom(int min, int max)
{
	int n;
	n = (int)uniformRandom((float)min, (float)max + 1.0);
	if (n > max) n = max;
	return n;
}
