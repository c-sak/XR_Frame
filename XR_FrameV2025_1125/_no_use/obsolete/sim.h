#define N_TARGET 15
#define N_MARKER 2048

#include "calc.h"
/*
vector_t
euler_t
OrientationT
ObjDataT
*/

// sim.cpp
void InitScene( void );
void UpdateScene( void );
void TermScene( void );

// draw.cpp
void PreDraw( void );
void PostDraw( void );
void DrawScene( void );

/*
typedef struct{
	vector_t pos;
	euler_t rot;
	int status;
} ObjDataT;
*/

typedef struct{
	TargetT TargetList[N_TARGET];
	MarkerT MarkerList[N_MARKER];
	////////▼ユーザデータ



	////////▲
} SimDataT;
