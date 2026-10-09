/*###########################################################################*/
/* OpenGL Texture Utility Functions
 * This code is provided for non-profit and personal learning use
 * without any guarantee for results of the use.
 * Any comercial reuse of this material is prohibited.
 * Under the condition, this code can be redistributed as is.
 * Copyright (C) 2014 Toshikazu Ohshima, All Rights Reserved.
 */

#include <stdlib.h>
#include <stdio.h>

#include "ezTexture.h"

//---------------------------------------------------------------------------
static int pixformatToDepth( GLubyte pixformat )
{
	int depth = 0;
	switch( pixformat ){
	case GL_LUMINANCE:
	case GL_DEPTH_COMPONENT:
		depth = 1;
		break;
	case GL_RGB:
	case GL_BGR_EXT:
		depth = 3;
		break;
	case GL_RGBA:
	case GL_BGRA_EXT:
		depth = 4;
		break;
	}
	return depth;
}
//----------------------------------------------------------------------------- bind
void ezTex_bind( GLuint texid )
{
	glBindTexture( GL_TEXTURE_2D, texid );
}
void ezTex_unbind( void )
{
	glBindTexture( GL_TEXTURE_2D, 0 );
}
//----------------------------------------------------------------------------- create
GLuint ezTex_create( int width, int height, GLuint pixformat, GLubyte *image )
{
	GLuint texID;

	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	/*
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	*/
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	switch (pixformat) {

	case GL_RGB:
	case GL_BGR_EXT:
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, pixformat,
			GL_UNSIGNED_BYTE, image);
		break;

	case GL_RGBA:
	case GL_BGRA_EXT:
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, pixformat,
			GL_UNSIGNED_BYTE, image);
		break;

	case GL_LUMINANCE:
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, pixformat, width, height, 0, pixformat,
			GL_UNSIGNED_SHORT, image);
		break;

	case GL_DEPTH_COMPONENT:
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, pixformat, width, height, 0, pixformat,
			GL_FLOAT, image);
		break;

	default:
		break;
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	return texID;
}
//------------------------------------------------------------------------------- load
GLubyte *ezTex_load( int width, int height, GLuint pixformat, GLubyte *image, const char *file )
{
	int depth, datasize;

	depth = pixformatToDepth( pixformat );
	datasize = width * height * depth;

	if( image == NULL ){ //引数のimageがNULLであれば新たにメモリを確保
		image = (GLubyte *)malloc( datasize );
	}
	if( file != NULL ){
	    FILE *fptex; //テクスチャファイル用ファイルポインタ
		fptex = fopen( file, "rb" ); //バイナリ読み込みモードでオープン
		if( fptex == NULL ){         //ファイルが読めなかったらエラー
			perror( file ); //エラーメッセージを出して
			exit( -1 ); //強制終了
		}
		fread( image, datasize, 1, fptex );//テクスチャデータを読み込む
		fclose( fptex );
	}
	return image;
}
//--------------------------------------------------------------------------------- subimage
void ezTex_subimage( int width, int height, GLuint pixformat, GLubyte *buffer, GLuint texid )
{
	glBindTexture( GL_TEXTURE_2D, texid );
	switch( pixformat ){

	case GL_RGB:
	case GL_BGR_EXT:
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, pixformat, GL_UNSIGNED_BYTE, buffer );			
		break;
	case GL_RGBA:
	case GL_BGRA_EXT:
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, pixformat, GL_UNSIGNED_BYTE, buffer );			
		break;

	case GL_LUMINANCE: //************
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, pixformat, GL_UNSIGNED_SHORT, buffer );				
		break;
	case GL_DEPTH_COMPONENT: //******************
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, pixformat, GL_FLOAT, buffer );
		break;
	}
	glBindTexture( GL_TEXTURE_2D, 0 );
}
//------------------------------------------------------------------- mesh
GLuint ezTex_makeMesh( void )
{
	GLuint texmesh = glGenLists(1);
    glNewList( texmesh , GL_COMPILE );
	glColor4f( 1.0, 1.0, 1.0, 1.0 );
	glBegin(GL_POLYGON);
	{
		glTexCoord2d( 0,1 );
		glVertex2d( 0.0, 0.0 );
		glTexCoord2d( 1,1 );
		glVertex2d( 1.0, 0.0 );
		glTexCoord2d( 1,0 );
		glVertex2d( 1.0, 1.0 );
		glTexCoord2d( 0,0 );
		glVertex2d( 0.0, 1.0 );
	}
	glEnd();
	glEndList();
	return texmesh;
}

//-----------------------------------------------------------------------------
void ezTex_drawMesh( GLuint texID, GLuint meshID )
{
	// テクスチャメモリに画像データをロード
	ezTexEnv_Replace(); //テクスチャ画像で置き換え

	glBindTexture( GL_TEXTURE_2D, texID );
	glEnable( GL_TEXTURE_2D );
	glCallList( meshID );
	glBindTexture( GL_TEXTURE_2D, 0 );
	glDisable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, 0 );

	return;
}
//========================================================= TexEnv
//------------------------------------------------------------------- setReplace
void ezTexEnv_Replace( void )
{ glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE ); return; }
//------------------------------------------------------------------ setModulate
void ezTexEnv_Modulate( void )
{ glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE ); return; }
//--------------------------------------------------------------------- setDecal
void ezTexEnv_Decal( void )
{ glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL ); return; }
//--------------------------------------------------------------------- setCopy
void ezTexEnv_Copy( void )
{ glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COPY ); return; }
//--------------------------------------------------------------------- setBlend
void ezTexEnv_Blend( float *color )
{
    glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );
    glTexEnvfv( GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, color );
    return;
}
//================================================================================