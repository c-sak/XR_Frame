//New Vector版
#include "platform.h"

#include <math.h>
#include <stdlib.h> //rand

#include "sim.h"
#include "calc.h"
#include "ezUtil.h"

extern float RADIAN;
extern float DEGREE;

//============================================= OBJ PARAMETERS
// 
//----------------------------------------------- setObjPos
void setObjPos( ObjDataT *obj, float *pos )
{
	vectorSet(&obj->pos, pos[0], pos[1], pos[2]);
}
//----------------------------------------------- setObjPos
void setObjPos(ObjDataT *obj, vector_t *pos)
{
	vectorCopy(&obj->pos, pos);
}
//----------------------------------------------- setObjPos
void setObjPos(ObjDataT* obj, float x, float y, float z)
{
	vectorSet(&obj->pos, x, y, z);
}
//----------------------------------------------- setObjRot
void setObjRot( ObjDataT *obj, float *rot )
{
	obj->rot.roll = rot[0];
	obj->rot.pitch= rot[1];
	obj->rot.yaw  = rot[2];
}
//----------------------------------------------- setObjRot
void setObjRot(ObjDataT* obj, euler_t *rot)
{
	eulerCopy( &obj->rot, rot );
}
//----------------------------------------------- getObjPos
void getObjPos( ObjDataT *obj, float *pos )
{
	pos[0] = obj->pos.x;
	pos[1] = obj->pos.y;
	pos[2] = obj->pos.z;
}
//--------------------------------------------- getObjRot
void getObjRot( ObjDataT *obj, float *rot )
{
	rot[0] = obj->rot.roll;
	rot[1] = obj->rot.pitch;
	rot[2] = obj->rot.yaw;
}
//----------------------------------------------------- setObjColor
void setObjColor( ObjDataT *obj, float red, float green, float blue )
{
	obj->color.red = red;
	obj->color.green = green;
	obj->color.blue = blue;
	obj->color.alpha = 1.0;
}
//------------------------------------------------------ setObjRot
void setObjRot( ObjDataT *obj, float roll, float pitch, float yaw )
{
	obj->rot.roll = roll;
	obj->rot.pitch= pitch;
	obj->rot.yaw  = yaw;
}
//----------------------------------------------------- setObjSize
void setObjSize(ObjDataT *obj, float x, float y, float z)
{
	obj->xsize = x;
	obj->ysize = y;
	obj->zsize = z;
}
//=========================================================================
//------------------------------------------------------- copyObj
void copyObj( ObjDataT *src, ObjDataT *dst )
{
	setObjPos( dst, &src->pos );
	setObjRot( dst, &src->rot );
	
	setObjColor( dst, src->color.red, src->color.green, src->color.blue );
	dst->base = src->base;
	dst->radius = src->radius;
	dst->state = src->state;
	dst->visible = src->visible;
	dst->move = src->move;
	dst->turn = src->turn;
	dst->xsize = src->xsize;
	dst->ysize = src->ysize;
	dst->zsize = src->zsize;
	return;
}
//-------- initObj
void initObj(ObjDataT *obj)
{
	setObjPos(obj, 0.0, 0.0, 0.0);
	setObjRot(obj, 0.0, 0.0, 0.0);
	setObjColor(obj, 0.0, 0.0, 0.0);
	setObjWorld(obj);
}
//-------- DirectionAtoB
//▼オブジェクトaからオブジェクトbへの方向ベクトルを求める関数
//オブジェクト間の方向ベクトル
void DirectionAtoB( ObjDataT *a, ObjDataT *b, vector_t *dir )
{
	vectorSub(dir, &b->pos, &a->pos);
}
//-------- DistanceAtoB
//▼距離を求める関数（「衝突判定」で作った関数の別バージョン）
//オブジェクト間距離
float DistanceAtoB( ObjDataT *a, ObjDataT *b )
{
	vector_t v;
 	DirectionAtoB( a, b, &v );
	return vectorNorm( &v );
}
//-------- EulerAtoB
//▼オブジェクト間の角度（aからbを見込む方位角と仰角）
float EulerAtoB( ObjDataT *a, ObjDataT *b, euler_t *angle )
{
	vector_t dir;
	DirectionAtoB( a, b, &dir );
	VectorToPolar( &dir, angle );
	return vectorNorm( &dir );
}

//▼オブジェクト間の角度（aからbを見込む方位角と仰角）
float EulerAtoB_Relative(ObjDataT *a, ObjDataT *b, euler_t *angle)
{
	vector_t dir;
	ObjDataT origin;
	ObjDataT child;

	copyObj(a, &origin);
	moveLocalToWorld(&origin);

	copyObj(b, &child);
	moveLocalToWorld(&child);

	moveWorldToLocal(&child, &origin);

	dir.x = child.pos.x;
	dir.y = child.pos.y;
	dir.z = child.pos.z;
	//DirectionAtoB( &origin, &child, &dir);

	VectorToPolar(&dir, angle);

	return vectorNorm(&dir);
}
//-------- MoveObject
//▼備考：構造体へのポインタを引数としている
void MoveObject( ObjDataT *obj )
{	
	obj->rot.yaw += obj->turn;
	obj->pos.x -= obj->move * sinf( obj->rot.yaw * RADIAN );
	obj->pos.z -= obj->move * cosf( obj->rot.yaw * RADIAN );
	obj->pos.y += obj->move * sinf( obj->rot.pitch * RADIAN ); ///// y方向の移動も考慮する場合
}
//====================================================================================
//--------------------------------------------------------- HitTest
// 
//▼物体同士の距離に基づいて衝突したかをチェックする関数
bool HitTest( ObjDataT *a, ObjDataT *b )
{
	if( DistanceAtoB( a, b ) < a->radius + b->radius ) return true;
	else return false;
}
//-------------------------------------------------------- HitTestBox
// 
//▼箱型オブジェクト（領域）と球状オブジェクト（領域）の基本衝突判定
//ballは、box座標系を基準としたローカルな位置情報とする。
bool HitTestBox(ObjDataT *box, ObjDataT *ball)
{
	//球体レベルの衝突判定でfalseなら衝突判定falseで終了
	float boxradius, x, y, z;
	x = box->xsize / 2.0;
	y = box->ysize / 2.0;
	z = box->zsize / 2.0;
	boxradius = sqrtf(x * x + y * y + z * z);
	
	if( DistanceAtoB( box, ball ) > boxradius + ball->radius ){
		return false;
	}
	
	//箱の座標範囲	
	float xmin = - box->xsize / 2.0;
	float xmax =   box->xsize / 2.0;
	float ymin = - box->ysize / 2.0;
	float ymax =   box->ysize / 2.0;
	float zmin = - box->zsize / 2.0;
	float zmax =   box->zsize / 2.0;
	//X座標の衝突判定
	if( xmax < ball->pos.x - ball->radius || xmin > ball->pos.x + ball->radius){		
		return false;
	}
	//Y座標の衝突判定
	if( ymax < ball->pos.y - ball->radius || ymin > ball->pos.y + ball->radius){		
		return false;
	}
	//Z座標の衝突判定
	if( zmax < ball->pos.z - ball->radius || zmin > ball->pos.z + ball->radius){
		return false;
	}
	return true;
}
//-------- isHit
bool isHit( ObjDataT *a, ObjDataT *b )
{
	ObjDataT a_world, b_world;

	copyObj( a, &a_world );
	copyObj( b, &b_world );
	moveLocalToWorld( &a_world );
	moveLocalToWorld( &b_world );

	return HitTest( &a_world, &b_world );
}
//-------- isHitBox
bool isHitBox( ObjDataT *box, ObjDataT *ball )
{
	ObjDataT ball_world;
	ObjDataT box_world;

	copyObj( box, &box_world );
	copyObj( ball, &ball_world );
	// 両者をワールドへ
	moveLocalToWorld( &box_world );
	moveLocalToWorld( &ball_world );
	// ballをboxのローカルへ
	moveWorldToLocal( &ball_world, &box_world );

	// ここで box_world を箱ローカル（中心が原点）にする
	setObjPos(&box_world, 0.0f, 0.0f, 0.0f);
	setObjRot(&box_world, 0.0f, 0.0f, 0.0f);

	return HitTestBox( &box_world, &ball_world );
}
//======================================================================================
//-------------座標系階層構造操作関数
//-------- setObjLocal
void setObjLocal( ObjDataT *target, ObjDataT *base )
{
	target->base = base;
	return;
}
//-------- setObjWorld
void setObjWorld( ObjDataT *target )
{
	target->base = nullptr;
	return;
}
//-------- moveLocalToWorld
//ローカルからワールドへ
void moveLocalToWorld( ObjDataT *target )
{
	ObjDataT *base;
	base = target->base;

	if (base == nullptr) return;

	while( base != nullptr ){
		TransformLocalToWorld( base, target, target );
		base = base->base;
		setObjLocal( target, base );
	}
	setObjWorld( target );
	return;
}
//-------- moveWorldToLocal
//ワールドからローカルへ
void moveWorldToLocal( ObjDataT *target, ObjDataT *base )
{
	ObjDataT *b, world;

	if (target->base != nullptr) {
		moveLocalToWorld(target); //targetがワールド下になければワールドへ
	}

	//if (base == nullptr) return; //baseがnullptrであればワールドなので処理終了

	b = base->base;

	if( b != nullptr && base != nullptr ){ //baseがワールドでなければワールドへ
		copyObj(base, &world);
		moveLocalToWorld( &world );
	}
	else {
		//good
		copyObj(base, &world);
		//bad
		//initObj(&world);
	}
	TransformWorldToLocal( &world, target, target );
	setObjLocal( target, base );
	return;
}
//-------- getWorld
void getWorld(ObjDataT *target, ObjDataT *world)
{
	copyObj(target, world);
	moveLocalToWorld(world);
}

//-------- TransformLocalToWorld
static void TransformLocalToWorld( ObjDataT *base, ObjDataT *local, ObjDataT *world )
{
#if 0
	//ObjDataT hand, target;
	float base_pos[3], base_rot[3], local_pos[3], local_rot[3];
	float world_pos[3], world_rot[3];

	getObjPos( base, base_pos );
	getObjRot( base, base_rot );
	
	getObjPos( local, local_pos );
	getObjRot( local, local_rot );
	
	ezUtil_Mult( base_pos, base_rot, local_pos, local_rot, world_pos, world_rot );

	setObjPos( world, world_pos ); ////////0627
	setObjRot( world, world_rot ); ////////0627
#else
	matrix_t m_base, m_local, m_world;
	matrixMake(m_base, &base->rot, &base->pos);
	matrixMake(m_local, &local->rot, &local->pos);
	matrixMult(m_world, m_local, m_base);
	//matrixPrint("test",m_world);
	matrixGetRot(m_world, &world->rot);
	matrixGetO(m_world, &world->pos);
#endif
}
//-------- TransformWorldToLocal
static void TransformWorldToLocal( ObjDataT *base, ObjDataT *target, ObjDataT *local )
{
#if 0
	float matrix[16];

	glPushMatrix();
	glLoadIdentity();
	
	applyObjInverse( base );
	applyObjTransform( target );
	
	glGetFloatv( GL_MODELVIEW_MATRIX, matrix );
	glPopMatrix();

	float rot[3],pos[3];
	ezUtil_getRot( matrix, rot );
	ezUtil_getPos( matrix, pos );

	setObjPos( local, pos );
	setObjRot( local, rot );
#else
	////
	matrix_t m_base, m_target, m_local;
	matrixMakeInv(m_base, &base->rot, &base->pos);
	matrixMake(m_target, &target->rot, &target->pos);
	matrixMult(m_local, m_target, m_base ); ////////
	//matrixPrint("test",m_world);
	matrixGetRot(m_local, &local->rot);
	matrixGetO(m_local, &local->pos);
#endif
	return;
}
//-----------------------------------------------
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
	/*
	dst->x = pos[0];
	dst->y = pos[1];
	dst->z = pos[2];
	*/
	setObjPos( dst, pos );
	/*
	dst->roll = rot[0];
	dst->pitch = rot[1];
	dst->yaw = rot[2];
	*/
	setObjRot( dst, rot );

	return;
}
//--------------------------------------------------------------- TransformLocalToWorld
//
static void TransformLocalToWorldX(  TargetT *base, ObjDataT *local, ObjDataT *world )
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

	/*
	glTranslatef( local->x, local->y, local->z );
	glRotatef( local->yaw, 0.0, 1.0, 0.0 );
	glRotatef( local->pitch, 1.0, 0.0, 0.0 );
	glRotatef( local->roll, 0.0, 0.0, 1.0 );
	*/
	applyObjTransform( local );
	
	glGetFloatv( GL_MODELVIEW_MATRIX, matrix );
	glPopMatrix();

	float rot[3],pos[3];
	ezUtil_getRot( matrix, rot );
	ezUtil_getPos( matrix, pos );
	/*
	world->x = pos[0];
	world->y = pos[1];
	world->z = pos[2];
	world->roll = rot[0];
	world->pitch = rot[1];
	world->yaw = rot[2];
	*/
	setObjPos( world, pos );
	setObjRot( world, rot );

	return;
}

#if 0
static void LocalForward(ObjDataT * target, vector_t * forward)
{
	vector_t forWardDir( 0,0,1);
	DirectionLocalToWorld(target, &forWardDir, forward);
}

static void LocalUP(ObjDataT * target, vector_t * up)
{
	vector_t upDir(0,1,0 );
	DirectionLocalToWorld(target, &upDir, up);
}

static void DirectionLocalToWorld(ObjDataT * target, vector_t * localDir, vector_t * worldDir) {
	
	//*
	float pos_origin[3];
	float pos_dir[3];

	matrix_t matrix[2];
	glPushMatrix(); {
		glLoadIdentity();

		applyObjTransform(target);
		glTranslatef(0,0,0);
		glGetFloatv(GL_MODELVIEW_MATRIX, matrix[0]);

		glTranslatef(localDir->x, localDir->y, localDir->z);
		glGetFloatv(GL_MODELVIEW_MATRIX, matrix[1]);
	}glPopMatrix();
	ezUtil_getPos(matrix[0], pos_origin);
	ezUtil_getPos(matrix[1], pos_dir);

	worldDir->x = pos_origin[0] - pos_dir[0];
	worldDir->y = pos_origin[1] - pos_dir[1];
	worldDir->z = pos_origin[2] - pos_dir[2];
	/*/
	ObjDataT dir;
	moveWorldToLocal(&dir, target);
	setObjPos(&dir,
		localDir->x,
		localDir->y,
		localDir->z);
	moveLocalToWorld(&dir);

	worldDir->x = dir.pos.x;
	worldDir->y = dir.pos.y;
	worldDir->z = dir.pos.z;
	//*/
	*worldDir = VectorNormalized(*worldDir);
}
#endif

//======================================= APPLY OPENGL TRANSFORMATION
//▼オブジェクトの位置と姿勢の幾何変換を行う関数
//プログラミング上のトピックス
//関数の引数として「構造体のポインタ」を与えるには？（アロー演算子）
//「構造体のポインタ」を使って構造体の中身「メンバー変数」を使うには？
//----------------------------------------- applyObjTransform
void applyObjTransform( ObjDataT *obj )
{
	if( obj->base != nullptr ){
		applyObjTransform( obj->base );
	}
	glTranslatef( obj->pos.x, obj->pos.y, obj->pos.z ); //位置
	glRotatef( obj->rot.yaw,  0.0, 1.0, 0.0 );//ヨー（方位角）ｙ軸
	glRotatef( obj->rot.pitch,1.0, 0.0, 0.0 );//ピッチ（仰角）ｘ軸
	glRotatef( obj->rot.roll, 0.0, 0.0, 1.0 );//ロール（傾き）ｚ軸
	return;
}
//----------------------------------------- applyObjInvrese
void applyObjInverse( ObjDataT *obj )
{
	glRotatef( - obj->rot.roll,	0.0, 0.0, 1.0 );//ロール（傾き）ｚ軸
	glRotatef( - obj->rot.pitch,1.0, 0.0, 0.0 );//ピッチ（仰角）ｘ軸
	glRotatef( - obj->rot.yaw,	0.0, 1.0, 0.0 );//ヨー（方位角）ｙ軸
	glTranslatef( - obj->pos.x, - obj->pos.y, - obj->pos.z ); //位置
	if( obj->base != nullptr ){
		applyObjInverse( obj->base );
	}	
	return;
}
//▼オブジェクトのカラーを適用する関数
void applyObjColor( ObjDataT *obj )
{
	applyMaterialColor( obj->color.red, obj->color.green, obj->color.blue );
}
//============================================= POINTING AND INTERSECTION
//------------------------------------------------------------ Pointing
void Pointing( ObjDataT *xy, ObjDataT *z, vector_t *o )
{
	ObjDataT xy_copy;
	ObjDataT z_copy;

	vector_t n;
	vector_t p;
	vector_t u;
	vector_t q;

	n.x = 0.0;
	n.y = 0.0;
	n.z = 1.0;
	p.x = 0.0;
	p.y = 0.0;
	p.z = 0.0;

	copyObj( xy, &xy_copy );
	copyObj( z,  &z_copy );
	moveWorldToLocal( &z_copy, &xy_copy );
	getZaxis( &z_copy, &u );
	getObjPos( &z_copy, &q );
	Intersection( &n, &p, &u, &q, o );
}
//----------------------------------------------------------- Intersection
void Intersection( vector_t *n, vector_t *p, vector_t *u, vector_t *q, vector_t *o )
{
	float t;
	float a, b, c, i, j, k;

	a = n->x;
	b = n->y;
	c = n->z;
	i = u->x;
	j = u->y;
	k = u->z;

	t = a*(p->x-q->x) + b*(p->y-q->y) + c*(p->z-q->z) / (a*i+b*j+c*k);
	o->x = q->x + i*t;
	o->y = q->y + j*t;
	o->z = q->z + k*t;
}
//======================================= GET OBJECT TRANSFORMATION
//---------------------------------------------- getMatrix
void getMatrix( ObjDataT *obj, matrix_t mat )
{
	matrixMake(mat, &obj->rot, &obj->pos);
}
//---------------------------------------------- getObjPos
void getObjPos( ObjDataT *obj, vector_t *pos )
{
	vectorCopy(pos, &obj->pos);
}
//---------------------------------------------- getObjAxes
void getObjAxes(ObjDataT* obj, vector_t *xaxis, vector_t *yaxis, vector_t *zaxis)
{
	matrix_t mat;
	matrixRot(mat, &obj->rot);
	matrixGetX(mat, xaxis);
	matrixGetY(mat, yaxis);
	matrixGetZ(mat, zaxis);
}
//---------------------------------------------- getXaxis
void getXaxis( ObjDataT *obj, vector_t *x )
{
	matrix_t mat;
	matrixRot(mat, &obj->rot);
	matrixGetX(mat, x);
}
//---------------------------------------------- getYaxis
void getYaxis( ObjDataT *obj, vector_t *y )
{
	matrix_t mat;
	matrixRot(mat, &obj->rot);
	matrixGetY(mat, y);
}
//---------------------------------------------- getZaxis
void getZaxis( ObjDataT *obj, vector_t *z )
{
	matrix_t mat;
	matrixRot(mat, &obj->rot);
	matrixGetZ(mat, z);
}
//--------------------------------------------- rotateBillboard
// targetが常にeyeの方向を向くようにyawを制御する
void rotateBillboard(
	ObjDataT* eye, //ワールド座標系であること
	ObjDataT* target) //ワールド座標系であること
{
	glTranslatef(target->pos.x, target->pos.y, target->pos.z);
	float dx = eye->pos.x - target->pos.x;
	float dy = eye->pos.y - target->pos.y;
	float dz = eye->pos.z - target->pos.z;
	float yaw = atan2f(dx, dz) / M_PI * 180.0;

	glRotatef( target->rot.yaw, 0, 1, 0);
	glRotatef( target->rot.pitch, 1, 0, 0);
	glRotatef( target->rot.roll, 0, 0, 1);
	glRotatef( yaw, 0, 1, 0);
	glRotatef(-target->rot.yaw, 0, 1, 0);
}
//#############################################################