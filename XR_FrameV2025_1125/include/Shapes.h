//void makeGlobe(float radius, int slices, int stacks, char *texfile);
/*
typedef struct {
	float x, y, z;
} vector_t;
*/
typedef struct{
	float x, y, z;
	float s, t, u;
	float nx, ny, nz;
} VertexT;

#define _STACKS 8   //緯度方向の分割
#define _SLICES 16  //経度方向の分割

typedef struct{

	float eq_d; //赤道直径
	float pl_d; //極直径

    VertexT globev[ _STACKS + 1][ _SLICES ];  //_stacks+1は要素数だから一つ多く箱を作る

	VertexT *v;

} GlobeT;


void ezSmoothSphere(float radius, int n_slices, int n_stacks);

void ezSolidSphere(float radius, int n_slices, int n_stacks );

void ezWireSphere(float radius, int n_slices, int n_stacks);

void ezSphere(float radius, int n_slices, int n_stacks);

void ezInitShape(void);
void ezSolidCone(float radius, float height, int n_slices, int n_stacks, bool lid = false);
void ezWireCone(float radius, float height, int n_slices, int n_stacks, bool lid = false);
void ezSolidCylinder(float bottom, float top, float height, int n_slices, int n_stacks, bool lid = false);
void ezWireCylinder(float bottom, float top, float height, int n_slices, int n_stacks, bool lid = false);

void ezSolidDisk(float inner, float outer, int n_slices, int n_loops);
void ezWireDisk(float inner, float outer, int n_slices, int n_loops);

void ezSolidTorus(float inner, float outer, int n_slices, int n_loops);
void ezWireTorus(float inner, float outer, int n_slices, int n_loops);

void ezSolidCircle(float radius, int n_slices);
void ezWireCircle(float radius, int n_slices);

void ezSolidCube(float size);
void ezWireCube(float size);

//------------------------
void makeGlobe();
void drawGlobe();

void drawGlobe0();

GlobeT *makeGlobe( float eq_d, float pl_d = 0.0 );
void drawGlobe( GlobeT *globe );

void drawGlobeZoom( GlobeT *globe, float lt, float lg, float range, float zoom );
// 緯度経度lat/longを中心に、緯度方向range度の範囲をzoom倍拡大表示する

void drawPartialGlobe( float lt, float lg, float zoom );