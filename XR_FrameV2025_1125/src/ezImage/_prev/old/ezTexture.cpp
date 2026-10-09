//##############################################################################
/* ezTexture.cpp
 * 
 * SAMPLE CODE FOR INTERACTIVE CG PROGRAMMING COURSES
 * AT COLLEGE OF IMAGE ARTS AND SCIENCES, RITSUMEIKAN UNIVERSITY
 * COPYRIGHT(C)2008 TOSHIKAZU OHSHIMA, ALL RIGHTS RESERVED
 */
//##############################################################################

#include "platform.h"

#include "ezTexture.h"

#define EZ_DEGREE (180.0/3.1415926)

//========================================================================
//---------------------------------------------------------------- setMode
void ezTexEnv_setMode( GLenum mode )
{ glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, mode ); }
//---------------------------------------------------------------- replace
void ezTexEnv_replace(){ ezTexEnv_setMode( GL_REPLACE ); }
//--------------------------------------------------------------- modulate
void ezTexEnv_modulate(){ ezTexEnv_setMode( GL_MODULATE ); }
//------------------------------------------------------------------ decal
void ezTexEnv_decal(){ ezTexEnv_setMode( GL_DECAL ); }
//------------------------------------------------------------------- copy
void ezTexEnv_copy(){ ezTexEnv_setMode( GL_COPY ); }
//------------------------------------------------------------------ blend
void ezTexEnv_blend( float *color )
{
    ezTexEnv_setMode( GL_DECAL );
    glTexEnvfv( GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, color );
}
//======================================================================== ezTex
//------------------------------------------------------------------- bind
void ezTex_bind( GLuint tex ){ glBindTexture( GL_TEXTURE_2D, tex );}
//----------------------------------------------------------------- unbind
void ezTex_unbind(){ glBindTexture( GL_TEXTURE_2D, 0 );}
//----------------------------------------------------------------- enable
void ezTex_enable(){ glEnable( GL_TEXTURE_2D ); }
//---------------------------------------------------------------- disable
void ezTex_disable(){ glDisable( GL_TEXTURE_2D ); }
//======================================================================== ezTex
//------------------------------------------------------------ loadTexture
static GLubyte *LoadImage( const char *file, int width, int height, int depth )
{
    GLubyte *rawimage; //テクスチャ用画像データの配列
  
    rawimage = (GLubyte *)malloc( width * height * depth );
    FILE *fptex;              //テクスチャファイル用ファイルポインタ
    fptex = fopen( file, "rb" ); //バイナリ読み込みモードでオープン
    if( fptex == NULL ){         //ファイルが読めなかったらエラー
        perror( file );
        exit( -1 );
    }
    //画像データを読み込む
    fread( rawimage, width * height * depth, 1, fptex );
    fclose( fptex );
    return rawimage;
}
//--------
GLuint ezTexture_Define( GLubyte *image, int width, int height, int depth )
{
    GLuint tex;               //テクスチャハンドル

    glGenTextures( 1, &tex );            //新しいテクスチャを生成し
    glBindTexture( GL_TEXTURE_2D, tex ); //カレントに設定する

    //▼画像データをテクスチャメモリに登録する
    switch( depth ){
      case 3: //▼RGBモード
        //メモリのテクセル境界の指定
        glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
        //テクスチャメモリに登録
        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
                      GL_RGB, GL_UNSIGNED_BYTE, image );
        break;
      case 4: //▼RGBAモード
        glPixelStorei( GL_UNPACK_ALIGNMENT, 4 );
        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
                      GL_RGBA, GL_UNSIGNED_BYTE, image );
        break;
      default:
        break;
    }
    //▼テクスチャフィルターの設定
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST ); //■
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST ); //■

    glBindTexture( GL_TEXTURE_2D, 0 ); //■テクスチャ登録完了

    return tex;
}
//--------
static GLuint ezTexture_LoadImage( const char *file, int width, int height, int depth )
{
    GLuint tex;               //テクスチャハンドル
    GLubyte *rawimage;         //テクスチャ用画像データの配列
  
    rawimage = LoadImage( file, width, height, depth );
    tex = ezTexture_Define( rawimage, width, height, depth );
	free( rawimage );
    return tex;
}

void ezTexture_SubImage( GLuint tex, GLubyte *subimage,
                         int width, int height )
{
    glBindTexture( GL_TEXTURE_2D, tex );
    glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, width, height,
                     GL_BGR_EXT, GL_UNSIGNED_BYTE, subimage );
}
//------------------------------------- Bind
void ezTexture_Bind( GLuint texid )
{
    ezTex_enable();
    ezTex_bind( texid );
    ezTexEnv_modulate();
}
//------------------------------------- Unbind
void ezTexture_Unbind( void )
{
    ezTex_unbind();
    ezTex_disable();
}
//========================================================================
//----------------------------------------------------- glTexGen_ObjLinear
/*テクスチャ座標自動生成の設定
 *頂点のオブジェクト空間における座標値を利用してテクスチャ座標を生成する
 */
void ezTexGen_setObjLinear( float roll, float pitch, float yaw,
                            float cx, float cy, float s )
{
    glTexGeni( GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR );
    glTexGeni( GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR );
    //テクスチャ座標基準軸の設定
    double s_axis[4] = { 1.0, 0.0, 0.0, 0.0 };
    double t_axis[4] = { 0.0, 1.0, 0.0, 0.0 };
    glTexGendv( GL_S, GL_OBJECT_PLANE, s_axis );
    glTexGendv( GL_T, GL_OBJECT_PLANE, t_axis );
    //テクスチャマトリクスの設定
    glMatrixMode( GL_TEXTURE );
    glLoadIdentity();
    glRotatef( yaw, 0.0, 1.0, 0.0 );
    glRotatef( pitch, 1.0, 0.0, 0.0 );
    glRotatef( roll, 0.0, 0.0, 1.0 );
    glScalef( s, - s, s );
    glTranslatef( cx, cy, 0.0 );
    glMatrixMode( GL_MODELVIEW );
    return;
}
//----------------------------------------------------------------- enable
void ezTexGen_enable()
{
    glEnable( GL_TEXTURE_GEN_S );
    glEnable( GL_TEXTURE_GEN_T );
}
//---------------------------------------------------------------- disable
void ezTexGen_disable()
{
    glDisable( GL_TEXTURE_GEN_S );
    glDisable( GL_TEXTURE_GEN_T );
}
//========================================================================
