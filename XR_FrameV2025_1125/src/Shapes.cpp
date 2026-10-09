#include "platform.h"

#include <math.h>
#include "Shapes.h"

const int _stacks = _STACKS;  //緯度方向の分割
const int _slices = _SLICES;  //経度方向の分割

static GLUquadric* pFillObj = nullptr;
static GLUquadric* pLineObj = nullptr;

//-------- ezInitShape
void ezInitShape(void) {
	pFillObj = gluNewQuadric();
	pLineObj = gluNewQuadric();
	gluQuadricDrawStyle(pFillObj, GLU_FILL);
	gluQuadricOrientation(pFillObj, GLU_OUTSIDE);
	gluQuadricTexture(pFillObj, GLU_TRUE);
	gluQuadricDrawStyle(pLineObj, GLU_LINE);
	gluQuadricOrientation(pLineObj, GLU_OUTSIDE);
	gluQuadricTexture(pLineObj, GLU_FALSE);
}
//-------- ezSolidCone
void ezSolidCone(float radius, float height, int n_slices, int n_stacks, bool lid)
{
	gluCylinder(pFillObj, radius, 0.0, height, n_slices, n_stacks);
	if (!lid) return;
	glPushMatrix();
	glRotatef(180.0 / n_slices, 0.0, 0.0, 1.0);
	glRotatef(180.0, 1.0, 0.0, 0.0);
	ezSolidCircle(radius, n_slices);
	glPopMatrix();
}
//-------- ezWireCone
void ezWireCone(float radius, float height, int n_slices, int n_stacks, bool lid)
{
	gluCylinder(pLineObj, radius, 0.0, height, n_slices, n_stacks);
	if (!lid) return;
	glPushMatrix();
	glRotatef(180.0 / n_slices, 0.0, 0.0, 1.0);
	glRotatef(180.0, 1.0, 0.0, 0.0);
	ezWireCircle(radius, n_slices);
	glPopMatrix();
}
//-------- ezSolidCylinder
void ezSolidCylinder(float bottom, float top,  float height, int n_slices, int n_stacks, bool lid)
{
	gluCylinder(pFillObj, bottom, top, height, n_slices, n_stacks);
	if (!lid)return;
	glPushMatrix();
	glRotatef(180.0 / n_slices, 0.0, 0.0, 1.0);
	glRotatef(180.0, 1.0, 0.0, 0.0);
	ezSolidCircle(bottom, n_slices);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0.0, 0.0, height);
	ezSolidCircle(top, n_slices);
	glPopMatrix();
}
//-------- ezWireCylinder
void ezWireCylinder(float bottom, float top, float height, int n_slices, int n_stacks, bool lid)
{
	gluCylinder(pLineObj, bottom, top, height, n_slices, n_stacks);
	if (!lid)return;
	glPushMatrix();
	glRotatef(180.0 / n_slices, 0.0, 0.0, 1.0);
	glRotatef(180.0, 1.0, 0.0, 0.0);
	ezWireCircle(bottom, n_slices);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0.0, 0.0, height);
	ezWireCircle(top, n_slices);
	glPopMatrix();
}
//-------- ezSolidDisk
void ezSolidDisk(float inner, float outer, int n_slices, int n_loops){
	gluDisk(pFillObj, inner, outer, n_slices, n_loops); ////////★////////
}
//-------- ezWireDisk
void ezWireDisk(float inner, float outer, int n_slices, int n_loops) {
	gluDisk(pLineObj, inner, outer, n_slices, n_loops);
}
//-------- ezSolidCircle
void ezSolidCircle(float radius, int n_slices)
{
	ezSolidDisk( 0.0, radius, n_slices, 1);
}
//-------- ezWireCircle
void ezWireCircle(float radius, int n_slices)
{
	ezWireDisk(0.0, radius, n_slices, 1);
}
//-------- ezSolidTorus
void ezSolidTorus(float inner, float outer, int n_slices, int n_loops)
{
	ezSolidDisk(outer-inner, outer, n_slices, n_loops);
}
//-------- ezWireTorus
void ezWireTorus(float inner, float outer, int n_slices, int n_loops)
{
	ezWireTorus(outer-inner, outer, n_slices, n_loops);
}
//===========================================================================//
// Function for displaying the cube (colored in)
//===========================================================================//
static void dispCube(GLfloat sideLen, bool fill) {
	GLint mode;
	if (fill) mode = GL_POLYGON;
	else mode = GL_LINE_LOOP;

	glBegin( mode );
	glNormal3f(0.0, 0.0, -1.0);
	glVertex3f(-sideLen / 2, -sideLen / 2, -sideLen / 2);
	glVertex3f(sideLen / 2, -sideLen / 2, -sideLen / 2);
	glVertex3f(sideLen / 2, sideLen / 2, -sideLen / 2);
	glVertex3f(-sideLen / 2, sideLen / 2, -sideLen / 2);
	glEnd();
	glBegin( mode );
	glNormal3f(-1.0, 0.0, 0.0);
	glVertex3f(-sideLen / 2, -sideLen / 2, -sideLen / 2);
	glVertex3f(-sideLen / 2, sideLen / 2, -sideLen / 2);
	glVertex3f(-sideLen / 2, sideLen / 2, sideLen / 2);
	glVertex3f(-sideLen / 2, -sideLen / 2, sideLen / 2);
	glEnd();
	glBegin( mode );
	glNormal3f(0.0, -1.0, 0.0);
	glVertex3f(sideLen / 2, -sideLen / 2, -sideLen / 2);
	glVertex3f(sideLen / 2, -sideLen / 2, sideLen / 2);
	glVertex3f(-sideLen / 2, -sideLen / 2, sideLen / 2);
	glVertex3f(-sideLen / 2, -sideLen / 2, -sideLen / 2);
	glEnd();
	glBegin( mode );
	glNormal3f(1.0, 0.0, 0.0);
	glVertex3f(sideLen / 2, -sideLen / 2, sideLen / 2);
	glVertex3f(sideLen / 2, -sideLen / 2, -sideLen / 2);
	glVertex3f(sideLen / 2, sideLen / 2, -sideLen / 2);
	glVertex3f(sideLen / 2, sideLen / 2, sideLen / 2);
	glEnd();
	glBegin( mode );
	glNormal3f(0.0, 1.0, 0.0);
	glVertex3f(-sideLen / 2, sideLen / 2, sideLen / 2);
	glVertex3f(-sideLen / 2, sideLen / 2, -sideLen / 2);
	glVertex3f(sideLen / 2, sideLen / 2, -sideLen / 2);
	glVertex3f(sideLen / 2, sideLen / 2, sideLen / 2);
	glEnd();
	glBegin( mode );
	glNormal3f(0.0, 0.0, 1.0);
	glVertex3f(-sideLen / 2, sideLen / 2, sideLen / 2);
	glVertex3f(-sideLen / 2, -sideLen / 2, sideLen / 2);
	glVertex3f(sideLen / 2, -sideLen / 2, sideLen / 2);
	glVertex3f(sideLen / 2, sideLen / 2, sideLen / 2);
	glEnd();
}
//-------- ezWireCube
void ezWireCube(float size)
{
	dispCube(size, false);
}
//-------- ezSolidCube
void ezSolidCube(float size)
{
	dispCube(size, true);
}
//--------------------------------------------------------------------------------
VertexT globev[_stacks + 1][_slices];  //_stacks+1は要素数だから一つ多く箱を作る
/*-------------------------------------------------------------- ezSmoothSphere
 * ezSmoothSphere:
 *--------*/
void ezSmoothSphere(float radius, int n_slices, int n_stacks)
{
	float x = 0.0;
	float y = 0.0;
	float z = 0.0;
	float l = 0;
	static float x0, y0, z0;
	static float x1, y1, z1, l1;

	int i, j;
	float ds, dt; //テクスチャ座標増分値
	float s = 0.0, t = 1.0; //テクスチャ座標

	ds = 1.0 / n_slices; //経度方向＝S座標
	dt = 1.0 / n_stacks; //緯度方向＝T座標

	float rot = 0.0, elev = 0.0;
	float sin_rot, cos_rot;

	static float s_rot[360];
	static float c_rot[360];
	static float s_elev[360];
	static float c_elev[360];

	glShadeModel(GL_SMOOTH);
	//glEnable(GL_AUTO_NORMAL);
	//glEnable(GL_NORMALIZE);

	for (j = 0; j <= n_slices; j++) { //経度
		rot = 2.0 * M_PI * ds * j;
		s_rot[j] = sinf(rot);
		c_rot[j] = cosf(rot);
	}
	for (i = 0; i <= n_stacks; i++) { //緯度
		elev = M_PI * dt * i - M_PI_2;
		s_elev[i] = sinf(elev);
		c_elev[i] = cosf(elev);
	}
	for (j = 0; j < n_slices; j++) { //経度
		sin_rot = s_rot[j];
		cos_rot = c_rot[j];
		glBegin(GL_TRIANGLE_STRIP);
		x0 = 0.0;
		y0 = 0.0;
		z = -radius;
		l = 0;
		for (i = 0; i <= n_stacks; i++) { //緯度
			z1 = z;
			l1 = l;
			z = radius * s_elev[i];
			l = radius * c_elev[i];
			x = l * cos_rot;
			y = l * sin_rot;
			x1 = x0;
			y1 = y0;
			x0 = l * c_rot[j + 1];
			y0 = l * s_rot[j + 1];

			glTexCoord2f(s + ds, t);
			glNormal3f(x0, y0, z);
			glVertex3f(x0, y0, z);
			glTexCoord2f(s, t);
			glNormal3f(x, y, z);
			glVertex3f(x, y, z);
			s += ds;
		}
		t -= dt;
		glEnd();
	}
	glShadeModel(GL_FLAT);
	return;
}
/*---------------------------------------------------------------- ezSolidSphere
 * ezSolidSphere:
 *--------*/
void ezSolidSphere(float radius, int n_slices, int n_stacks)
{
	float x = 0.0;
	float y = 0.0;
	float z = 0.0;
	float l = 0;
	static float x0, y0, z0;
	static float x1, y1, z1, l1;

	int i, j;
	float ds, dt; //テクスチャ座標増分値
	float s = 0.0, t = 1.0; //テクスチャ座標

	ds = 1.0 / n_slices; //経度方向＝S座標
	dt = 1.0 / n_stacks; //緯度方向＝T座標

	float rot = 0.0, elev = 0.0;
	float sin_rot, cos_rot;

	static float s_rot[360];
	static float c_rot[360];
	static float s_elev[360];
	static float c_elev[360];

	glShadeModel(GL_FLAT);
	//glShadeModel(GL_SMOOTH);
	//glEnable(GL_AUTO_NORMAL);
	glEnable(GL_NORMALIZE);

	for (j = 0; j <= n_slices; j++) { //経度
		rot = 2.0 * M_PI * ds * j;
		s_rot[j] = sinf(rot);
		c_rot[j] = cosf(rot);
	}
	for (i = 0; i <= n_stacks; i++) { //緯度
		elev = M_PI * dt * i - M_PI_2;
		s_elev[i] = sinf(elev);
		c_elev[i] = cosf(elev);
	}
	for (j = 0; j < n_slices; j++) { //経度
		sin_rot = s_rot[j];
		cos_rot = c_rot[j];
		glBegin(GL_TRIANGLE_STRIP);
		x0 = 0.0;
		y0 = 0.0;
		z = -radius;
		l = 0;
		for (i = 0; i <= n_stacks; i++) { //緯度
			z1 = z;
			l1 = l;
			z = radius * s_elev[i];
			l = radius * c_elev[i];
			x = l * cos_rot;
			y = l * sin_rot;
			x1 = x0;
			y1 = y0;
			x0 = l * c_rot[j + 1];
			y0 = l * s_rot[j + 1];

			if (i > 0) {
				//z1 = radius * s_elev[i - 1];
				//l1 = radius * c_elev[i - 1];
				//x1 = l1 * c_rot[j + 1];
				//y1 = l1 * s_rot[j + 1];
				glNormal3f((x1 + x) / 2.0, (y1 + y) / 2.0, (z1 + z) / 2.0);
			}
			glTexCoord2f(s+ds, t);
			glVertex3f(x0, y0, z);
			glTexCoord2f(s, t);
			glVertex3f(x, y, z);
			s += ds;
		}
		t -= dt;
		glEnd();
	}
	return;
}
/*---------------------------------------------------------------- ezWireSphere
 * ezWireSphere:
 *--------*/
void ezWireSphere(float radius, int n_slices, int n_stacks)
{
	float x = 0.0;
	float y = 0.0;
	float z = 0.0;
	float l = 0;
	static float x0, y0, z0;
	static float x1, y1, z1, l1;

	int i, j;
	float ds, dt; //テクスチャ座標増分値
	float s = 0.0, t = 1.0; //テクスチャ座標

	ds = 1.0 / n_slices; //経度方向＝S座標
	dt = 1.0 / n_stacks; //緯度方向＝T座標

	float rot = 0.0, elev = 0.0;
	float sin_rot, cos_rot;

	static float s_rot[360];
	static float c_rot[360];
	static float s_elev[360];
	static float c_elev[360];

	//glShadeModel(GL_SMOOTH);
	//glEnable(GL_AUTO_NORMAL);
	glEnable(GL_NORMALIZE);

	for (j = 0; j <= n_slices; j++) { //経度
		rot = 2.0 * M_PI * ds * j;
		s_rot[j] = sinf(rot);
		c_rot[j] = cosf(rot);
	}
	for (i = 0; i <= n_stacks; i++) { //緯度
		elev = M_PI * dt * i - M_PI_2;
		s_elev[i] = sinf(elev);
		c_elev[i] = cosf(elev);
	}
	for (j = 0; j < n_slices; j++) { //経度
		sin_rot = s_rot[j];
		cos_rot = c_rot[j];
		glBegin(GL_LINE_STRIP);
		x0 = 0.0;
		y0 = 0.0;
		z = -radius;
		l = 0;
		bool up = false;
		for (i = 0; i <= n_stacks; i++) { //緯度
			z1 = z;
			l1 = l;
			z = radius * s_elev[i];
			l = radius * c_elev[i];
			x = l * cos_rot;
			y = l * sin_rot;
			x1 = x0;
			y1 = y0;
			x0 = l * c_rot[j + 1];
			y0 = l * s_rot[j + 1];

			if ( up ) {
				glTexCoord2f(s + ds, t);
				glNormal3f(x0, y0, z);
				glVertex3f(x0, y0, z);
				glTexCoord2f(s, t);
				glNormal3f(x, y, z);
				glVertex3f(x, y, z);
				up = false;
			}
			else {
				glTexCoord2f(s, t);
				glNormal3f(x, y, z);
				glVertex3f(x, y, z);
				glTexCoord2f(s + ds, t);
				glNormal3f(x0, y0, z);
				glVertex3f(x0, y0, z);
				up = true;
			}
			s += ds;
		}
		t -= dt;
		glEnd();
	}
	return;
}
/*---------------------------------------------------------------- ezSolidSphere
 * ezSolidSphere:
 *--------*/
void ezSphere(float radius, int n_slices, int n_stacks)
{
	VertexT *globev = (VertexT *)calloc((n_stacks + 1) * (n_slices + 1), sizeof(VertexT));

	float x = 0.0;
	float y = 0.0;
	float z = 0.0;
	float l = 0;
	static float x0, y0, z0;
	static float x1, y1, z1, l1;

	int i, j;
	float ds, dt; //テクスチャ座標増分値
	float s = 0.0, t = 0.0; //テクスチャ座標f

	ds = 1.0 / n_slices; //経度方向＝S座標
	dt = 1.0 / n_stacks; //緯度方向＝T座標

	float rot = 0.0, elev = 0.0;
	float sin_rot, cos_rot;

	VertexT *v = globev;

	static float s_rot[360];
	static float c_rot[360];
	static float s_elev[360];
	static float c_elev[360];

	//glShadeModel(GL_SMOOTH);
	//glEnable(GL_AUTO_NORMAL);
	glEnable(GL_NORMALIZE);

	for (j = 0; j <= n_slices; j++) { //経度
		rot = 2.0 * M_PI * ds * j;
		s_rot[j] = sinf(rot);
		c_rot[j] = cosf(rot);
	}
	for (i = 0; i <= n_stacks; i++) { //緯度
		elev = M_PI * dt * i - M_PI_2;
		s_elev[i] = sinf(elev);
		c_elev[i] = cosf(elev);
	}
	for (j = 0; j <= n_slices; j++) { //経度
		sin_rot = s_rot[j];
		cos_rot = c_rot[j];
		glBegin(GL_TRIANGLE_STRIP);
		x0 = 0.0;
		y0 = 0.0;
		z = -radius;
		l = 0;
		for (i = 0; i <= n_stacks; i++) { //緯度
			z1 = z;
			l1 = l;
			z = radius * s_elev[i];
			l = radius * c_elev[i];
			x = l * cos_rot;
			y = l * sin_rot;
			x1 = x0;
			y1 = y0;
			x0 = l * c_rot[j + 1];
			y0 = l * s_rot[j + 1];

			if (i > 0) {
				//z1 = radius * s_elev[i - 1];
				//l1 = radius * c_elev[i - 1];
				//x1 = l1 * c_rot[j + 1];
				//y1 = l1 * s_rot[j + 1];
				glNormal3f((x1 + x) / 2.0, (y1 + y) / 2.0, (z1 + z) / 2.0);
			}

			glVertex3f(x0, y0, z);
			glVertex3f(x, y, z);

			v->x = x; v->y = y; v->z = z;
			v->nx = x; v->ny = y; v->nz = z;
			v->s = s; v->t = t;
			s += ds;
			v++;
		}
		t += dt;
		glEnd();
	}

	VertexT *v0 = globev;
	VertexT *v1 = globev + n_stacks + 1;

	/*
	for (j = 0; j < n_slices; j++) {//緯度
		//glBegin(GL_POINTS);
		//glBegin(GL_LINE);
		//glBegin(GL_POLYGON);
		glBegin(GL_TRIANGLE_STRIP);
		//glBegin(GL_LINE_STRIP);
		for (i = 0; i <= n_stacks; i++) { //経度
			//glNormal3f(v1->x, v1->y, v1->z);
			//if (i == n_stacks) {
			if( i > 0 ){
				glNormal3f((v1->x + (v0 - 1)->x) / 2.0,
				(v1->y + (v0 - 1)->y) / 2.0, (v1->z + (v0 - 1)->z) / 2.0);
			}
			else {
				//glNormal3f(v0->x, v0->y, v0->z);
			}
			glVertex3f(v1->x, v1->y, v1->z);
			//glNormal3f(v0->x, v0->y, v0->z);
			glVertex3f(v0->x, v0->y, v0->z);
			v0++;
			v1++;
		}
		glEnd();
	}
	*/

	return;
}

/*------------------------------------------------------------------ drawObj
 * makeGlobe:
 *--------*/
GlobeT *makeGlobe( float r, float eq_d, float pl_d )
{
	GlobeT *globe = (GlobeT *)malloc( sizeof(GlobeT) ); //動的メモリ領域確保

	globe->eq_d = eq_d;
	if( pl_d > 0.0 ) globe->pl_d = pl_d;
	else             globe->pl_d = eq_d;

	float x = 0.0;
	float y = 0.0;
	float z = 0.0;

	float l = 0;
	//int slices = 16; //経度方向の分割
	//int stacks = 8;  //緯度方向の分割

	int i, j;
	float a = 360.0 / _slices; //経度方向の増分値
	float b = 180.0 / _stacks; //緯度方向の増分値

	for( i = 0; i <= _stacks; i++ ){              //緯度
		z = r * sinf( M_PI * ((float)i * b - 90.0) / 180 );
		l = r * cosf( M_PI * ((float)i * b - 90.0) / 180 );
		for( j = 0; j < _slices; j++ ){           //経度
			x = l * cosf( M_PI * (float)j * a / 180 );
			y = l * sinf( M_PI * (float)j * a / 180 );

			globe->globev[i][j].x = x * globe->eq_d;
			globe->globev[i][j].y = y * globe->eq_d;
			globe->globev[i][j].z = z * globe->pl_d;

		}
	}

    return globe;
}

/*-------------------------------------------------------------- drawGlobe
 * drawGlobe:
 *--------*/
void drawGlobe( GlobeT *globe )
{
	int i, j;
	float ds, dt; //テクスチャ座標増分値
	float s, t; //テクスチャ座標f

	ds = 1.0/_SLICES; //経度方向＝S座標
	dt = 1.0/(_STACKS+1);//緯度方向＝T座標

	//glEnable( GL_AUTO_NORMAL );
	glEnable( GL_NORMALIZE );
	/*南極*/
	/*glBegin(GL_TRIANGLE_FAN);
	s = 0.0;
	t = 0.0;
	glTexCoord2f( s, 1.0 - t );
	glNormal3f( globe->globev[0][0].x, globe->globev[0][0].y, globe->globev[0][0].z );
	glVertex3f( globe->globev[0][0].x, globe->globev[0][0].y, globe->globev[0][0].z );
	t += dt;
	for( j = _slices - 1; j >= 0; j-- ){//経度
		glTexCoord2f( s, 1.0 - t );
		s += ds;
		glNormal3f( globe->globev[1][j].x, globe->globev[1][j].y, globe->globev[1][j].z ); //法線ベクトル
		glVertex3f( globe->globev[1][j].x, globe->globev[1][j].y, globe->globev[1][j].z );
	}
	glTexCoord2f( s, 1.0 -  t );
	glNormal3f( globe->globev[1][_slices - 1].x, globe->globev[1][_slices - 1].y, globe->globev[1][_slices - 1].z );
	glVertex3f( globe->globev[1][_slices - 1].x, globe->globev[1][_slices - 1].y, globe->globev[1][_slices - 1].z );
	glEnd();*/

	/*ベルト*/
	glBegin(GL_TRIANGLE_STRIP);
	s = 0.0;
	t = 0.0;
	for( i = 0; i < _stacks; i++ ){ 
		for( j = 0; j < _slices; j++ ){
			glTexCoord2f( s, 1.0 - (t+dt) ); //北極と南極が逆になっていたため、1.0からt+dtを引いて逆にした
			glNormal3f( globe->globev[i + 1][j].x, globe->globev[i + 1][j].y, globe->globev[i + 1][j].z );
			glVertex3f( globe->globev[i + 1][j].x, globe->globev[i + 1][j].y, globe->globev[i + 1][j].z );
			glTexCoord2f( s, 1.0 - t );
			glNormal3f( globe->globev[i][j].x, globe->globev[i][j].y, globe->globev[i][j].z );
			glVertex3f( globe->globev[i][j].x, globe->globev[i][j].y, globe->globev[i][j].z );
			s += ds;
		}
		glTexCoord2f( s, 1.0 - (t+dt) );
		glNormal3f( globe->globev[i + 1][0].x, globe->globev[i + 1][0].y, globe->globev[i + 1][0].z );
		glVertex3f( globe->globev[i + 1][0].x, globe->globev[i + 1][0].y, globe->globev[i + 1][0].z );
		glTexCoord2f( s, 1.0 - t );
		glNormal3f( globe->globev[i][0].x, globe->globev[i][0].y, globe->globev[i][0].z );
		glVertex3f( globe->globev[i][0].x, globe->globev[i][0].y, globe->globev[i][0].z );
		t += dt;
	}
	glEnd();

	/*北極*/
	/*s = 0.0;
	glBegin(GL_TRIANGLE_FAN);
	glTexCoord2f( s, 1.0 - t );
	glNormal3f( globe->globev[_stacks][0].x, globe->globev[_stacks][0].y, globe->globev[_stacks][0].z );
	glVertex3f( globe->globev[_stacks][0].x, globe->globev[_stacks][0].y, globe->globev[_stacks][0].z );
	for( j = 0; j < _slices; j++ ){//経度
		glTexCoord2f( s, 1.0 - t );
		s += ds;
		glNormal3f( globe->globev[_stacks - 1][j].x, globe->globev[_stacks - 1][j].y, globe->globev[_stacks - 1][j].z );
		glVertex3f( globe->globev[_stacks - 1][j].x, globe->globev[_stacks - 1][j].y, globe->globev[_stacks - 1][j].z );
	}
	t += dt;
	glTexCoord2f( s, 1.0 - t );
	glNormal3f( globe->globev[_stacks - 1][0].x, globe->globev[_stacks - 1][0].y, globe->globev[_stacks - 1][0].z );
	glVertex3f( globe->globev[_stacks - 1][0].x, globe->globev[_stacks - 1][0].y, globe->globev[_stacks - 1][0].z );
	glEnd();*/

	//glutSolidSphere( 1.1, 16, 8 );

}

/*-------------------------------------------------------------- drawGlobeZoom
 * drawGlobeZoom:
 *--------*/
void drawGlobeZoom( GlobeT *globe, float lt, float lg, float range, float zoom )
{ // lt:緯度, lg:経度
	int i, j;
	float ds, dt; //テクスチャ座標増分値
	float s, t;   //テクスチャ座標

	float dlong, dlat; //1つのSLICE、STACKあたりの角度
	int   nlong, nlat; //SLICEとSTACKの番号

	/*緯度*/
	dlat = 180.0 / _STACKS;     //1つのSTACK（緯度方向の分割）あたりの角度
	nlat = (int)( lt / dlat );  //南極から数えたSTACKの番号（南極から数えて何番目のSTACKか）
	nlat = _STACKS / 2 + nlat;  //-90度分補正した、南極から数えたSTACKの番号（南極から数えて何番目のSTACKか）

	/*経度*/
	dlong = 360.0 / _SLICES;    //1つのSLICE（経度方向の分割）あたりの角度
	nlong = (int)( lg /dlong ); //SLICEの番号（何番目のSLICEか）

	/*
	float n; //rangeで指定される範囲に入るスタックの数
    n = range / 180.0 * _STACKS;
	*/

	ds = 1.0/_SLICES;     //テクスチャ座標増分値（経度方向＝S座標）
	dt = 1.0/(_STACKS+1); //テクスチャ座標増分値（緯度方向＝T座標）

	//glEnable( GL_AUTO_NORMAL );
	glEnable( GL_NORMALIZE );
	
	/*ベルト*/
	glBegin(GL_TRIANGLE_STRIP);
	s = ds * nlong; //テクスチャ座標（経度方向＝S座標）
	t = dt * nlat;  //テクスチャ座標（緯度方向＝T座標）

	i = nlat; 
	for( j = nlong; j <= nlong+1; j++ ){  
		glTexCoord2f( s, 1.0 - (t+dt) ); //北極と南極が逆になっていたため、1.0からt+dtを引いて逆にした
		if( lt >= 0.0 ){   //北極のとき
			glNormal3f( globe->globev[i+1][j].x, globe->globev[i+1][j].y, globe->globev[i+1][j].z );
			glVertex3f( globe->globev[i+1][j].x, globe->globev[i+1][j].y, globe->globev[i+1][j].z );
			glTexCoord2f( s, 1.0 - t );
			glNormal3f( globe->globev[i][j].x, globe->globev[i][j].y, globe->globev[i][j].z );
			glVertex3f( globe->globev[i][j].x, globe->globev[i][j].y, globe->globev[i][j].z );
		}
		else{              //南極のとき
			glNormal3f( globe->globev[i][j].x, globe->globev[i][j].y, globe->globev[i][j].z );
			glVertex3f( globe->globev[i][j].x, globe->globev[i][j].y, globe->globev[i][j].z );
			glTexCoord2f( s, 1.0 - t );
			glNormal3f( globe->globev[i-1][j].x, globe->globev[i-1][j].y, globe->globev[i-1][j].z );
			glVertex3f( globe->globev[i-1][j].x, globe->globev[i-1][j].y, globe->globev[i-1][j].z );
		}
		s += ds;
	}
	glEnd();
}
#if 0
/*-------------------------------------------------------------- drawPartialGlobe
 * drawPartialGlobe:ポリゴンの頂点を指定して描画する
 *--------*/
void drawPartialGlobe( float lt, float lg, float zoom )
{
	float x, y, z;
	float t = 0; //テクスチャの緯度方向の座標
	float s = 0; //テクスチャの経度方向の座標
	float dlt = 0; //パッチの中心を基準とした、緯度方向のパッチの頂点までの角度
	float dlg = 0; //パッチの中心を基準とした、経度方向のパッチの頂点までの角度
	float dt = 0; //テクスチャの緯度方向の座標の増分値
	float ds = 0; //テクスチャの経度方向の座標の増分値

	//緯度
	t = ( lt + 90 ) / 180; //テクスチャの緯度方向の座標
	dlt = 180.0 / _STACKS; //パッチの中心を基準とした、緯度方向のパッチの頂点までの角度
	dt = dlt / 180 * zoom;        //テクスチャの緯度方向の座標の増分値
	//経度
	s = lg / 360;          //テクスチャの経度方向の座標
	dlg = 360.0 / _SLICES; //パッチの中心を基準とした、経度方向のパッチの頂点までの角度
	ds = dlg / 360 * zoom;        //テクスチャの経度方向の座標の増分値
	

	glEnable( GL_NORMALIZE );

	glBegin( GL_TRIANGLE_FAN );
	PolarToXYZ( 0.066, lt, lg, &x, &y, &z );
	glTexCoord2f( s, 1.0 - t );
	glNormal3f( x, y, z );
	glVertex3f( x, y, z );
	PolarToXYZ( 0.066, lt+dlt, lg-dlg, &x, &y, &z );
	glTexCoord2f( s-ds, 1.0 - (t+dt) );
	glNormal3f( x, y, z );
	glVertex3f( x, y, z );
	PolarToXYZ( 0.066, lt-dlt, lg-dlg, &x, &y, &z );
	glTexCoord2f( s-ds, 1.0 - (t-dt) );
	glNormal3f( x, y, z );
	glVertex3f( x, y, z );
	PolarToXYZ( 0.066, lt-dlt, lg+dlg, &x, &y, &z );
	glTexCoord2f( s+ds, 1.0 - (t-dt) );
	glNormal3f( x, y, z );
	glVertex3f( x, y, z );
	PolarToXYZ( 0.066, lt+dlt, lg+dlg, &x, &y, &z );
	glTexCoord2f( s+ds, 1.0 - (t+dt) );
	glNormal3f( x, y, z );
	glVertex3f( x, y, z );
	PolarToXYZ( 0.066, lt+dlt, lg-dlg, &x, &y, &z );
	glTexCoord2f( s-ds, 1.0 - (t+dt) );
	glNormal3f( x, y, z );
	glVertex3f( x, y, z );
	glEnd();

	glBindTexture( GL_TEXTURE_2D, 0 );
	glBegin( GL_LINE_STRIP );
	glDisable(GL_LIGHTING );
	//glLineWidth( 1.0 );
	glColor3f( 1.0, 1.0, 1.0 );
	PolarToXYZ( 0.067, lt+dlt, lg-dlg, &x, &y, &z );
	glVertex3f( x, y, z );
	PolarToXYZ( 0.067, lt-dlt, lg-dlg, &x, &y, &z );
	glVertex3f( x, y, z );
	PolarToXYZ( 0.067, lt-dlt, lg+dlg, &x, &y, &z );
	glVertex3f( x, y, z );
	PolarToXYZ( 0.067, lt+dlt, lg+dlg, &x, &y, &z );
	glVertex3f( x, y, z );
	PolarToXYZ( 0.067, lt+dlt, lg-dlg, &x, &y, &z );
	glVertex3f( x, y, z );
	glEnd();
	glEnable(GL_LIGHTING);

    return;
}
#endif
/*------------------------------------------------------------------ drawObj
 * drawGlobe0:
 *--------*/

void drawGlobe0( void )
{
	float x = 0.0;
	float y = 0.0;
	float z = 0.0;
	float r = 0.5;

	float l = 0;
	//int slices = 16; //経度方向の分割
	//int stacks = 8;  //緯度方向の分割
	int i, j;
	float a = 360.0 / _slices; //経度方向の増分値
	float b = 180.0 / _stacks; //緯度方向の増分値

    glColor3f( 1.0, 1.0, 1.0);
    glPointSize( 4.0 );
	glBegin(GL_POINTS);
	for( i = 0; i <= _stacks; i++ ){              //緯度
		z = r * sinf( M_PI * ((float)i * b - 90.0) / 180 );
		l = r * cosf( M_PI * ((float)i * b - 90.0) / 180 );
		for( j = 0; j < _slices; j++ ){          //経度
			x = l * cosf( M_PI * (float)j * a / 180 );
			y = l * sinf( M_PI * (float)j * a / 180 );

			globev[i][j].x = x;
			globev[i][j].y = y;
			globev[i][j].z = z;
		
			glVertex3f( globev[i][j].x, globev[i][j].y, globev[i][j].z );
			glNormal3f(globev[i][j].x, globev[i][j].y, globev[i][j].z);
		}
	}
	glEnd();
    return;
}

/*------------------------------------------------------------------ drawObj
 * makeGlobe:
 *--------*/
/*
void makeGlobe( void )
{
	float x = 0.0;
	float y = 0.0;
	float z = 0.0;
	float r = 0.5;

	float l = 0;
	//int slices = 16; //経度方向の分割
	//int stacks = 8;  //緯度方向の分割
	int i, j;
	float a = 360.0 / _slices; //経度方向の増分値
	float b = 180.0 / _stacks; //緯度方向の増分値

	for( i = 0; i <= _stacks; i++ ){              //緯度
		z = r * sinf( M_PI * ((float)i * b - 90.0) / 180 );
		l = r * cosf( M_PI * ((float)i * b - 90.0) / 180 );
		for( j = 0; j < _slices; j++ ){          //経度
			x = l * cosf( M_PI * (float)j * a / 180 );
			y = l * sinf( M_PI * (float)j * a / 180 );

			globev[i][j].x = x;
			globev[i][j].y = y;
			globev[i][j].z = z;		
		}
	}
    return;
}
*/

/*頂点を指定して球体を表示*/
/*void drawGlobePoints()
{
	int i, j;
    glColor3f( 1.0, 1.0, 1.0);
    glPointSize( 4.0 );
	glBegin(GL_POINTS);
	for( i = 0; i <= _stacks; i++ ){             //緯度
		for( j = 0; j < _slices; j++ ){          //経度
			glVertex3f( globev[i][j].x, globev[i][j].y, globev[i][j].z );
		}
	}
	glEnd();
}
*/

/*------------------------------------------------------------------ drawObj
 * drawGlobe:使っていない関数
 *--------*/
/*
void drawGlobe()
{
	int i, j;

    setMaterial();

	//glEnable( GL_AUTO_NORMAL );
	glEnable( GL_NORMALIZE );
	//南極
	glBegin(GL_TRIANGLE_FAN);
	glVertex3f( globev[0][0].x, globev[0][0].y, globev[0][0].z );
	for( j = _slices - 1; j >= 0; j-- ){               //経度
		glNormal3f( globev[1][j].x, globev[1][j].y, globev[1][j].z ); //法線ベクトル
		glVertex3f( globev[1][j].x, globev[1][j].y, globev[1][j].z );
	}
	glNormal3f( globev[1][_slices - 1].x, globev[1][_slices - 1].y, globev[1][_slices - 1].z );
	glVertex3f( globev[1][_slices - 1].x, globev[1][_slices - 1].y, globev[1][_slices - 1].z );
	glEnd();

	//ベルト
	glBegin(GL_TRIANGLE_STRIP);
	for( i = 1; i < _stacks-1; i++ ){ 
		for( j = 0; j < _slices; j++ ){
			glNormal3f( globev[i + 1][j].x, globev[i + 1][j].y, globev[i + 1][j].z );
			glVertex3f( globev[i + 1][j].x, globev[i + 1][j].y, globev[i + 1][j].z );
			glNormal3f( globev[i][j].x, globev[i][j].y, globev[i][j].z );
			glVertex3f( globev[i][j].x, globev[i][j].y, globev[i][j].z );
		}
		glNormal3f( globev[i + 1][0].x, globev[i + 1][0].y, globev[i + 1][0].z );
		glVertex3f( globev[i + 1][0].x, globev[i + 1][0].y, globev[i + 1][0].z );
		glNormal3f( globev[i][0].x, globev[i][0].y, globev[i][0].z );
		glVertex3f( globev[i][0].x, globev[i][0].y, globev[i][0].z );

	}
	glEnd();

	//北極
	glBegin(GL_TRIANGLE_FAN);
	glVertex3f( globev[_stacks][0].x, globev[_stacks][0].y, globev[_stacks][0].z );
	for( j = 0; j < _slices; j++ ){              //経度
		glNormal3f( globev[_stacks - 1][j].x, globev[_stacks - 1][j].y, globev[_stacks - 1][j].z );
		glVertex3f( globev[_stacks - 1][j].x, globev[_stacks - 1][j].y, globev[_stacks - 1][j].z );
	}
	glNormal3f( globev[_stacks - 1][0].x, globev[_stacks - 1][0].y, globev[_stacks - 1][0].z );
	glVertex3f( globev[_stacks - 1][0].x, globev[_stacks - 1][0].y, globev[_stacks - 1][0].z );
	glEnd();	

	//glutSolidSphere( 1.1, 16, 8 );

}
*/