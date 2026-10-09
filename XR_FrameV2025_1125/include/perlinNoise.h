#pragma once

void perlinInit();
float perlinNoise(float x, float y);
float perlinNoiseCircle(float angle, float radius);
float perlinFractal(float x, float y, float freq, float fractal);
float perlinFractalCircle(float angle, float radius, float freq, float fractal);
void perlinDraw(void);

void perlinMake( float freq, float fractal );

//-------- ezGround_Draw: 地面オブジェクトを描画する
void ezGround_Draw(GLuint list);

//-------- ezGround_Make: 地面オブジェクトを生成する
GLuint ezGround_Make(float freq, float fractal);

//-------- ezGround_MakeSquare: 四角形の地面オブジェクトを生成する
GLuint ezGround_MakeSquare(float freq, float fractal, float height, float size );

//-------- ezGround_MakeCircle: 円形の地面オブジェクトを生成する
GLuint ezGround_MakeCircle(float freq, float fractal, float height, float radius );

/*
graph TD

%% 基本ノイズ関数
perlinNoise --> fade
perlinNoise --> lerp
perlinNoise --> grad

perlinNoiseCircle --> perlinNoise

perlinFractal --> perlinNoise
perlinFractalCircle --> perlinNoiseCircle

%% 地形生成・描画
perlinMake --> perlinInit
perlinMake --> perlinFractal

perlinDraw --> terrain

ezGround_Make --> perlinMake
ezGround_Make --> perlinDraw

ezGround_MakeSquare --> perlinMake
ezGround_MakeSquare --> perlinDraw

makeWeb --> perlinFractalCircle
makeWeb_old --> perlinFractal

ezGround_MakeCircle --> makeWeb
ezGround_MakeCircle --> drawWeb

ezGround_Draw --> glCallList

%% 補助関数
fade
lerp
grad
perlinInit
drawWeb
*/