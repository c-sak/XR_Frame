#define _USE_MATH_DEFINES

#include <math.h>
#include <stdlib.h>

#include "sim.h"
#include "calc.h"
#include "ezUtil.h"

const float radian = M_PI / 180.0;
const float degree = 180.0 / M_PI;

//▼ベクトルの大きさを計算する関数
//ベクトルの大きさ
float VectorNorm( vector_t *v )
{
	return sqrtf( v->x * v->x + v->y * v->y + v->z * v->z );
}

void PolarToVector( float r, float pitch, float yaw, float *v)
{
    float angle;
	float l, x, y, z;

	angle = pitch * radian;
	y = r * sinf( angle );
	l = r * cosf( angle );

	angle = yaw * radian;
	x = - l * sinf( angle );
	z = - l * cosf( angle );

	v[0] = x;
	v[1] = y;
	v[2] = z;
}

void DirectionAtoB( vector_t *a, vector_t *b, vector_t *dir )
{
	dir->x = b->x - a->x;
	dir->y = b->y - a->y;
	dir->z = b->z - a->z;
}

//▼方向ベクトルから方位角と仰角を求める関数
float VectorToPolar( vector_t *v,  euler_t *angle )
{
	float d = sqrtf( v->x * v->x + v->z * v->z );
	angle->pitch = degree * atan2f( v->y, d );
	angle->yaw = degree * atan2f( - v->x, - v->z );
	return VectorNorm( v );
}

float EulerAtoB( ObjDataT *a, ObjDataT *b, euler_t *angle )
{
	vector_t dir;
	DirectionAtoB( &a->pos, &b->pos, &dir );
	VectorToPolar( &dir, angle );
	return VectorNorm( &dir );
}

/*---------------------------------------------- Random Functions */
float uniformRandom( float rmin, float rmax ){
    double r;
    r = (double)rand()/(double)RAND_MAX;
    r = r * ( rmax - rmin ) + rmin;
    return r;
}


/*---------------------------------------------- Gaussian Random */
float gaussianRandom( float rmin, float rmax ){
    int i;
	float r;
	const int n = 12;
	for( i = 0; i < n; i++ ){
		r += uniformRandom( rmin, rmax );
	}
	return r / n;
}
/*--------------------------*/


float DistanceAtoB( vector_t *a, vector_t *b )
{
	float dx, dy = 0.0, dz;
	dx = b->x - a->x;
	dy = b->y - a->y;
	dz = b->z - a->z;
	return sqrtf( dx * dx + dy * dy + dz * dz );
}

bool HitTest( ObjDataT *a, ObjDataT *b )
{
	if( DistanceAtoB( &a->pos, &b->pos ) < a->radius + b->radius ) return true;
	else return false;
}

void MoveObject( ObjDataT *obj )
{
	//obj->yaw += obj->turn;
	obj->pos.x -= obj->move * sinf( obj->rot.yaw * radian );
	obj->pos.z -= obj->move * cosf( obj->rot.yaw * radian );
}

void TransformLocalToWorldX(  TargetT *base, ObjDataT *local, ObjDataT *world )
{
	float matrix[16];
	glPushMatrix();
	glLoadIdentity();
    glTranslatef( base->pos.x,
			base->pos.y,
			base->pos.z );

	glRotatef( base->ori.angle,
			base->ori.x,
			base->ori.y,
			base->ori.z );


	glTranslatef( local->pos.x, local->pos.y, local->pos.z );
	glRotatef( local->rot.yaw, 0.0, 1.0, 0.0 );
	glRotatef( local->rot.pitch, 1.0, 0.0, 0.0 );
	glRotatef( local->rot.roll, 0.0, 0.0, 1.0 );
	glGetFloatv( GL_MODELVIEW_MATRIX, matrix );
	glPopMatrix();

	float rot[3],pos[3];
	ezUtil_getRot( matrix, rot );
	ezUtil_getPos( matrix, pos );
	world->pos.x = pos[0];
	world->pos.y = pos[1];
	world->pos.z = pos[2];
	world->rot.roll = rot[0];
	world->rot.pitch = rot[1];
	world->rot.yaw = rot[2];

	return;
}

void TransformLocalToWorld( ObjDataT *base, ObjDataT *local, ObjDataT *world )
{
	//ObjDataT hand, target;
	float base_pos[3], base_rot[3], local_pos[3], local_rot[3];
	float world_pos[3], world_rot[3];

	base_pos[0] = base->pos.x;
	base_pos[1] = base->pos.y;
	base_pos[2] = base->pos.z;
	base_rot[0] = base->rot.roll;
	base_rot[1] = base->rot.pitch;
	base_rot[2] = base->rot.yaw;

	local_pos[0] = local->pos.x;
	local_pos[1] = local->pos.y;
	local_pos[2] = local->pos.z;
	local_rot[0] = local->rot.roll;
	local_rot[1] = local->rot.pitch;
	local_rot[2] = local->rot.yaw;

	ezUtil_Mult( base_pos, base_rot, local_pos, local_rot, world_pos, world_rot );

	world->pos.x = world_pos[0];
	world->pos.y = world_pos[1];
	world->pos.z = world_pos[2];
	world->rot.roll = world_rot[0];
	world->rot.pitch = world_rot[1];
	world->rot.yaw = world_rot[2];
}

void TransformLocalToWorld2( TargetT *base, ObjDataT *local, ObjDataT *world )
{
	//ObjDataT hand, target;
	float base_pos[3], base_ori[4], local_pos[3], local_rot[3];
	float world_pos[3], world_rot[3];

	base_pos[0] = base->pos.x;
	base_pos[1] = base->pos.y;
	base_pos[2] = base->pos.z;
	base_ori[0] = base->ori.angle;
	base_ori[1] = base->ori.x;
	base_ori[2] = base->ori.y;
	base_ori[3] = base->ori.z;

	local_pos[0] = local->pos.x;
	local_pos[1] = local->pos.y;
	local_pos[2] = local->pos.z;
	local_rot[0] = local->rot.roll;
	local_rot[1] = local->rot.pitch;
	local_rot[2] = local->rot.yaw;

	ezUtil_Mult2( base_pos, base_ori, local_pos, local_rot, world_pos, world_rot );

	world->pos.x = world_pos[0];
	world->pos.y = world_pos[1];
	world->pos.z = world_pos[2];
	world->rot.roll = world_rot[0];
	world->rot.pitch = world_rot[1];
	world->rot.yaw = world_rot[2];
}

void TransformWorldToLocal( ObjDataT *base, ObjDataT *target, ObjDataT *local )
{

	float matrix[16];

	glPushMatrix();
	glLoadIdentity();
	glRotatef( - base->rot.roll, 0.0, 0.0, 0.0 );
	glRotatef( - base->rot.pitch, 1.0, 0.0, 0.0 );
	glRotatef( - base->rot.yaw, 0.0, 1.0, 0.0 );
	glTranslatef( - base->pos.x,
			- base->pos.y,
			- base->pos.z );

	glTranslatef( target->pos.x, target->pos.y, target->pos.z );
	glRotatef( target->rot.yaw, 0.0, 1.0, 0.0 );
	glRotatef( target->rot.pitch, 1.0, 0.0, 0.0 );
	glRotatef( target->rot.roll, 0.0, 0.0, 1.0 );
	glGetFloatv( GL_MODELVIEW_MATRIX, matrix );
	glPopMatrix();

	float rot[3],pos[3];
	ezUtil_getRot( matrix, rot );
	ezUtil_getPos( matrix, pos );
	local->pos.x = pos[0];
	local->pos.y = pos[1];
	local->pos.z = pos[2];
	local->rot.roll = rot[0];
	local->rot.pitch = rot[1];
	local->rot.yaw = rot[2];

	return;
}

void TargetToObjData(TargetT *src, ObjDataT *dst)
{
	float matrix[16];
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(src->pos.x,
		src->pos.y,
		src->pos.z);

	glRotatef(src->ori.angle,
		src->ori.x,
		src->ori.y,
		src->ori.z);

	glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
	glPopMatrix();

	float rot[3], pos[3];
	ezUtil_getRot(matrix, rot);
	ezUtil_getPos(matrix, pos);
	dst->pos.x = pos[0];
	dst->pos.y = pos[1];
	dst->pos.z = pos[2];
	dst->rot.roll = rot[0];
	dst->rot.pitch = rot[1];
	dst->rot.yaw = rot[2];

	return;
}
