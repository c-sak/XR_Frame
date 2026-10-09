#ifndef __OBJECT_H__
#define __OBJECT_H__

#include "vector.h"
#include "euler.h"
#include "color.h"

//-------- object data
typedef struct _objdata_t {

	//float x, y, z; ///// ◆レイアウト位置
	vector_t pos;
	vector_t prePos;
	//float roll, pitch, yaw; /////◆姿勢
	euler_t rot;
	int state; /////◆状態
	bool visible; ///◆可視属性

	color_t color; ///////◆色

	float move; //◆速度〔m/フレーム〕
	float turn; //◆旋回速度（各速度）〔度/フレーム〕

	float radius; //◆衝突判定用の半径
	float xsize, ysize, zsize; //ボックスサイズ

	struct _objdata_t *base; //★親ノード

	_objdata_t() {
		pos = vector_t(0, 0, 0);
		prePos = vector_t(0,0,0);
		rot = euler_t();
		state = 0;
		visible = true;
		
		color.red = 1;
		color.green = 1;
		color.blue = 1;
		
		move = 0;
		turn = 0;

		radius = 1;
		xsize = 1;
		ysize = 1;
		zsize = 1;

		base = nullptr;
	}

} ObjDataT;

#endif
