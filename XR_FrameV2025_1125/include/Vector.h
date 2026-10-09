#pragma once

/* ベクトル関数
* OpenGL非依存
*/

//-------- ベクトルを描画する
void vectorDraw(vector_t* p, vector_t* v);
void vectorDrawAtoB(vector_t* a, vector_t* b);

//-------- ベクトルvの成分表示
void vectorPrint(const char *head, const vector_t* v);

//-------- ベクトルvへの値(x,y,z)の設定
void vectorSet( vector_t *v, float x, float y, float z );

//-------- ベクトルsrcをdstにコピー
void vectorCopy( vector_t *dst, const vector_t *src );

//-------- ベクトルvの大きさ
float vectorNorm( const vector_t *v );

//-------- ベクトルvの大きさの2乗
float vectorNorm2( const vector_t *v );

//-------- ベクトルvを単位ベクトル化
float vectorNormalize( const vector_t *v );

//-------- ベクトルaとbの内積
float vectorDot( const vector_t *a, const vector_t *b );

//-------- ベクトルaとbの外積aXb
void vectorCross( vector_t *aXb, const vector_t *a, const vector_t *b );

//-------- ベクトル合成 v = a + b
void vectorAdd( vector_t *v, const vector_t *a, const vector_t *b );

//-------- 差分ベクトル v = a - b
void vectorSub( vector_t *v, const vector_t *a, const vector_t *b );

//-------- ベクトルvの定数倍のベクトルsv
void vectorScale(vector_t* sv, float s, const vector_t* v);

//-------- ベクトル a と b の各成分同士を掛けたベクトル ab（内積ではなく）
void vectorMult(vector_t* ab, const vector_t* a, const vector_t* b);

//-------- ベクトル合成 v = k * a + ( 1 - k ) * b
void vectorBlend( vector_t *v, float k, const vector_t *a, const vector_t *b );

//-------- ベクトルaとbの間の角度(0,180)
float vectorAngle( const vector_t *a, const vector_t *b );

//-------- 参照ベクトル ref を軸としたベクトル a とベクトル b の間の角度(-180,180)
float vectorAngleRef(const vector_t* a, const vector_t* b, const vector_t* ref);

//-------- ベクトルviをマトリクスmでベクトルvoに変換
void vectorXform(vector_t * vo, const vector_t * vi, const matrix_t m);

//-------- オイラー角 rot に (roll, pitch, yaw) を設定
void eulerSet(euler_t * rot, float roll, float pitch, float yaw);

void eulerCopy(euler_t* dst, const euler_t* src);

//-------- ベクトル v からオイラー角 angle へ変換(0,pitch,yaw)
float VectorToPolar(vector_t* v, euler_t* angle);
//-------- オイラー角 angle からベクトル v へ変換
void PolarToVector(euler_t* angle, vector_t* v);

//-------- 一様ランダム関数
float uniformRandom(float min, float max);
//-------- 簡易ガウシアンランダム関数
float gaussianRandom(float rmin, float rmax);
//-------- 整数型ランダム関数
int diceRandom(int min, int max);
