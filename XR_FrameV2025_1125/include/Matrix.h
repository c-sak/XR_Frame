#pragma once

//-------- マトリクス src から dsc にコピー
void matrixCopy( matrix_t dst, const matrix_t src );

//-------- マトリクス m の値を表示
void matrixPrint( const char* text, const matrix_t m );

//-------- マトリクス m を正規化
void matrixIdent( matrix_t m );

//-------- マトリクス a と b を ab に合成
void matrixMult(matrix_t ab, const matrix_t a, const matrix_t b);

//-------- 平行移動行列を生成
void matrixTrans( matrix_t m, const vector_t *p );
void matrixTrans(matrix_t m, float x, float y, float z);

//-------- 平行移動の逆変換
void matrixTransInv(matrix_t m, const vector_t* t);

//-------- X 軸の回転行列を生成
void matrixRotX( matrix_t rx, float pitch );

//-------- Y 軸の回転行列を生成
void matrixRotY( matrix_t ry, float yaw );

//-------- Z 軸の回転行列を生成
void matrixRotZ( matrix_t rz, float roll );

//-------- 任意の軸の回転行列を生成
void matrixRot( matrix_t mr, float roll, float pitch, float yaw );
void matrixRot( matrix_t mr, const euler_t *angle );
void matrixRot( matrix_t m, float a, float x, float y, float z );
void matrixRot( matrix_t m, float a, vector_t *v );

void matrixRotInv(matrix_t mr, const euler_t* rot);
void matrixRotInv(matrix_t mr, float roll, float pitch, float yaw);

//-------- オイラー角rotと平行移動posからマトリクスmを生成
void matrixMake(matrix_t m, const euler_t* rot, const vector_t* pos);

//-------- 任意軸の回転 a, axis と平行移動 trans からマトリクス m を生成
void matrixMake(matrix_t m, float a, const vector_t* axis, const vector_t* trans);

//-------- 逆変換行列 inv を生成
void matrixMakeInv(matrix_t inv, const euler_t* rot, const vector_t* pos);

//-------- X軸を取得
void matrixGetX( const matrix_t m, vector_t *xaxis );

//-------- Y軸を取得
void matrixGetY( const matrix_t m, vector_t *yaxis );

//-------- Z軸を取得
void matrixGetZ( const matrix_t m, vector_t *zaxis );

//-------- 原点を取得
void matrixGetO( const matrix_t m, vector_t *pos );

//-------- 回転成分を抽出
void matrixGetRot( const matrix_t mr, float *roll, float *pitch, float *yaw );
void matrixGetRot( const matrix_t mr, euler_t *angle );
