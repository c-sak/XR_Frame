#include "platform.h"
#include "calc.h"

#include "Vector.h"
#include "Matrix.h"
#include "string.h"
#include "stdio.h"
#define _USE_MATH_DEFINES
#include "math.h"

extern float DEGREE;
extern float RADIAN;

//-------- matrixCopy
void matrixCopy(matrix_t dst, const matrix_t src) 
{
	memcpy(dst, src, sizeof(matrix_t));
}
//-------- matrixIdent
void matrixIdent(matrix_t m) 
{
#if 1
	const matrix_t e = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
	memcpy(m, e, sizeof(matrix_t));
	/*
	glPushMatrix();
	glLoadIdentity();
	glGetFloatv(GL_MODELVIEW_MATRIX, m);
	glPopMatrix();
	*/
#else
	float* p = m;
	int i, j;
	for (i = 15; i > 0; i--) {
		*p++ = 0.0;
	}
	p = m;
	*p = 1.0;
	*(p + 5) = 1.0;
	*(p + 10) = 1.0;
	*(p + 15) = 1.0;
	/*
	  for( i = 4; i >= 0; i-- ){
	   p = 1.0;
	  p+= 5;
	  }
	*/
	/*
	  for ( i = 0; i < 4; i++ ) {
	  for ( j = 0; j < 4; j++ ) {
		if ( i == j ) *p = 1.0;
		else *p = 0.0;
		p++;
	  }
	  }
	*/
#endif
}
//-------- matrixMult
void matrixMult(matrix_t ab, const matrix_t a, const matrix_t b)
{
	/*
	glPushMatrix();
	glLoadMatrixf(b);
	glMultMatrixf(a);
	glGetFloatv( GL_MODELVIEW_MATRIX, ab);
	glPopMatrix();
	return;
	*/
	float* mab;
	const float* ma, * mb;
	mab = ab;
	int i, j, k;
	for (i = 0; i < 16; i += 4) {
		ma = &a[ i ]; //faster ////////
		for (j = 0; j < 4; j++) {
			//mab = &ab[ i + j ];
			//ma = &a[i]; //slower
			mb = &b[j];
#if 1 //faster than latter
			
			* mab = *(ma) * *(mb)
				+ *(ma+1) * *(mb + 4)
				+ *(ma+2) * *(mb + 8)
				+ *(ma+3) * *(mb + 12);
				
			/*
			* mab = *(ma) * *(mb)
				+*( ++ma) * *(mb + 4)
				+ *( ++ma) * *(mb + 8)
				+ *( ++ma) * *(mb + 12);
			*/
#else //slower
			* mab = 0.0;
			for (k = 0; k < 4; k++) {
				*mab += *ma * *mb;
				ma++;
				mb += 4;
			}
#endif
			mab++;
		}
	}
}
//-------- matrixPrint
void matrixPrint( const char *text, const matrix_t m) 
{
	const float* p = m;
	printf("%s: \n", text);
	for (int i = 0; i < 4; i++) {
		printf("%8.3f %8.3f %8.3f %8.3f\n",*p,*(p+1),*(p+2),*(p+3));
		p+=4;
	}
	printf("\n");
}
//-------- matrixTrans
void matrixTrans(matrix_t mt, const vector_t* t) 
{
	matrixTrans(mt, t->x, t->y, t->z);
}
//-------- matrixTrans
void matrixTrans(matrix_t mt, float x, float y, float z) 
{
	matrixIdent(mt);
	mt[12] = x;
	mt[13] = y;
	mt[14] = z;
}
//-------- matrixTransInv
void matrixTransInv( matrix_t mt, const vector_t* t)
{
	matrixTrans(mt, -t->x, -t->y, -t->z);
}
//-------- matrixRotX
void matrixRotX(matrix_t rx, float pitch)
{
	float c, s;
	matrixIdent(rx);
	pitch *= RADIAN;
	c = cosf(pitch);
	s = sinf(pitch);
	rx[5] = c; rx[6] = s;
	rx[9] = -s; rx[10] = c;
}
//-------- matrixRotY
void matrixRotY(matrix_t ry, float yaw)
{
	float c, s;
	matrixIdent(ry);
	yaw *= RADIAN;
	c = cosf(yaw);
	s = sinf(yaw);
	ry[10] = c; ry[8] = s;
	ry[2] = -s; ry[0] = c;
}
//-------- matrixRotZ
void matrixRotZ(matrix_t rz, float roll)
{
	float c, s;
	matrixIdent(rz);
	roll *= RADIAN;
	c = cosf(roll);
	s = sinf(roll);
	//s = 1.0 - c * c;
	rz[0] = c; rz[1] = s;
	rz[4] = -s; rz[5] = c;
}
//-------- matrixRot
void matrixRot(matrix_t mr, float roll, float pitch, float yaw)
{
#if 0
	matrix_t m0, m1;
	matrixRotZ(mr, roll); //roll
	matrixRotX(m0, pitch); //pitch
	matrixMult(m1, mr, m0); //roll*pitch
	matrixRotY(m0, yaw); //yaw
	matrixMult(mr, m1, m0); //roll*pitch*yaw
#else
	float cx, sx, cy, sy, cz, sz;
	cx = cosf(pitch * RADIAN);
	sx = sinf(pitch * RADIAN);
	cy = cosf(yaw * RADIAN);
	sy = sinf(yaw * RADIAN);
	cz = cosf(roll * RADIAN);
	sz = sinf(roll * RADIAN);

	mr[0] = cz * cy + sy * sx * sz;
	mr[1] = cx * sz;
	mr[2] = -sy * cz + cy * sx * sz;
	mr[3] = 0.0;

	mr[4] = -cy * sz + sy * sx * cz;
	mr[5] = cx * cz;
	mr[6] = sy * sz + cy * sx * cz;
	mr[7] = 0.0;

	mr[8] = sy * cx;
	mr[9] = -sx;
	mr[10] = cy * cx;
	mr[11] = 0.0;

	mr[12] = 0.0;
	mr[13] = 0.0;
	mr[14] = 0.0;
	mr[15] = 1.0;
#endif
}
//-------- matrixRot
void matrixRot(matrix_t mr, const euler_t* rot)
{
	matrixRot(mr, rot->roll, rot->pitch, rot->yaw);
}
//-------- matrixRot
void matrixRot(matrix_t m, float a, float x, float y, float z)
{
	float c = cosf(a * RADIAN);
	float c1 = 1.0 - c;
	float s = sinf(a * RADIAN);
	float l = sqrtf(x * x + y * y + z * z);//★
	x /= l; //★
	y /= l; //★回転軸を単位ベクトル化する必要がある
	z /= l; //★
	//----1st row
	m[0] = x * x * c1 + c;
	m[1] = x * y * c1 + z * s;
	m[2] = x * z * c1 - y * s;
	m[3] = 0.0;
	//----2nd row
	m[4] = y * x * c1 - z * s;
	m[5] = y * y * c1 + c;
	m[6] = y * z * c1 + x * s;
	m[7] = 0.0;
	//----3rd row
	m[8] = z * x * c1 + y * s;
	m[9] = z * y * c1 - x * s;
	m[10] = z * z * c1 + c;
	m[11] = 0.0;
	//----4th row
	m[12] = 0.0;
	m[13] = 0.0;
	m[14] = 0.0;
	m[15] = 1.0;
}
//-------- matrixRot
void matrixRot(matrix_t m, float a, const vector_t* v)
{
	matrixRot(m, a, v->x, v->y, v->z );
}
//------------------------
//-------- matrixRotInv
void matrixRotInv(matrix_t mr, float roll, float pitch, float yaw)
{
#if 0
	matrix_t m0, m1;
	matrixRotY(mr, -yaw); // -yaw
	matrixRotX(m0, -pitch); // -pitch
	matrixMult(m1, mr, m0); // -yaw * -pitch
	matrixRotZ(m0, -roll); // -roll
	matrixMult(mr, m1, m0); // -yaw * -pitch * -roll
#else
	float cx, sx, cy, sy, cz, sz;
	cx = cosf( - pitch * RADIAN);
	sx = sinf( - pitch * RADIAN);
	cy = cosf( - yaw * RADIAN);
	sy = sinf( - yaw * RADIAN);
	cz = cosf( - roll * RADIAN);
	sz = sinf( - roll * RADIAN);

	mr[0] = cz * cy - sy * sx * sz;
	mr[4] = - cx * sz;
	mr[8] = sy * cz + cy * sx * sz;
	mr[12] = 0.0;

	mr[1] = cy * sz + sy * sx * cz;
	mr[5] = cx * cz;
	mr[9] = sy * sz - cy * sx * cz;
	mr[13] = 0.0;

	mr[2] = - sy * cx;
	mr[6] = sx;
	mr[10] = cy * cx;
	mr[14] = 0.0;

	mr[3] = 0.0;
	mr[7] = 0.0;
	mr[11] = 0.0;
	mr[15] = 1.0;
#endif
}
//-------- matrixRotInv
void matrixRotInv(matrix_t mr, const euler_t* rot)
{
	matrixRotInv(mr, rot->roll, rot->pitch, rot->yaw);
}
//-------- matrixMake
void matrixMake(matrix_t m, const euler_t* rot, const vector_t* pos)
{
	matrix_t mr, mt;
	matrixRot(mr, rot);
	matrixTrans(mt, pos);
	matrixMult(m, mr, mt);
}
//-------- matrixMake
void matrixMake(matrix_t m, float a, const vector_t* axis, const vector_t* trans)
{
	matrix_t mr, mt;
	matrixRot(mr, a, axis);
	matrixTrans(mt, trans);
	matrixMult(m, mr, mt);
}
//-------- matrixMakeInv
void matrixMakeInv(matrix_t inv, const euler_t* rot, const vector_t* pos)
{
	matrix_t mr, mt;
	matrixTransInv(mt, pos);
	matrixRotInv(mr, rot);
	matrixMult(inv, mt, mr);
}
//==========================================================
//--------
void matrixGetX(const matrix_t m, vector_t* xaxis)
{
	xaxis->x = m[0];
	xaxis->y = m[1];
	xaxis->z = m[2];
}
//--------
void matrixGetY(const matrix_t m, vector_t* yaxis)
{
	yaxis->x = m[4];
	yaxis->y = m[5];
	yaxis->z = m[6];
}
//--------
void matrixGetZ(const matrix_t m, vector_t* zaxis)
{
	zaxis->x = m[8];
	zaxis->y = m[9];
	zaxis->z = m[10];
}
//--------
void matrixGetO(const matrix_t m, vector_t* pos)
{
	pos->x = m[12];
	pos->y = m[13];
	pos->z = m[14];
}
void matrixSetO(matrix_t m, const vector_t* pos)
{
	m[12] = pos->x;
	m[13] = pos->y;
	m[14] = pos->z;
}
//--------
void matrixGetRot(const matrix_t mr, float* roll, float* pitch, float* yaw)
{
	vector_t dir, right, up;
	vector_t xaxis, yaxis, zaxis;

	matrixGetX(mr, &xaxis);
	matrixGetY(mr, &yaxis);
	matrixGetZ(mr, &zaxis);
	// dir vector
	vectorScale(&dir, -1.0, &zaxis);
	
	/*
	float l;
	//l = sqrtf( dir.x*dir.x + dir.y*dir.y + dir.z*dir.z );
	l = vectorNorm(&dir);
	if (l <= 0) {
		vectorSet(&dir, 0.0, 0.0, -1.0);
		l = 1.0;
	}
	*/
	//---- YAW
	*yaw = atan2f(-dir.x, -dir.z) * DEGREE;
	//---- PITCH
	*pitch = asinf(dir.y ) * DEGREE;
	//---- ROLL
	// right vector on XZ plane
	vectorSet(&up, 0.0, 1.0, 0.0);
	vectorCross(&right, &dir, &up);
	// up vector
	vectorCross(&up, &right, &dir);
	// roll
	vectorCross(&dir, &up, &yaxis);
	*roll = vectorAngle(&up, &yaxis);
	if (vectorDot(&dir, &zaxis) < 0) {
		*roll *= -1;
	}
}
//-------- matrixGetRot
void matrixGetRot(const matrix_t mr, euler_t* angle)
{
	matrixGetRot(mr, &angle->roll, &angle->pitch, &angle->yaw);
}
//--------
