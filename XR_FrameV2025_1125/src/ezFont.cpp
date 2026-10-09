//##############################################################################
/* ezUtil.cpp
 * 
 * SAMPLE CODE FOR INTERACTIVE CG PROGRAMMING COURSES
 * AT COLLEGE OF IMAGE ARTS AND SCIENCES, RITSUMEIKAN UNIVERSITY
 * COPYRIGHT(C)2008 TOSHIKAZU OHSHIMA, ALL RIGHTS RESERVED
 */
//##############################################################################

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "ezFont.h"
#include "font.h"

static GLuint _base;

//======================================================================= ezFont
//------------------------------------------------------------------- makeBitmap
/* makeBitmap: フォントオブジェクトを準備する
 */
GLuint ezFont_makeBitmap( void )
{
    int i, n;

    glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
    _base = glGenLists(0x80);//ディスプレイリスト領域を 128 個獲得
    //各文字のビットマップデータを各ディスプレイリストに割り当てる
    n = 0;
    for( i = 0x20; i < 0x80; i++ ){
        glNewList( _base + i, GL_COMPILE );
        glBitmap( 8, 16, 0.0, 0.0, 9.0, 0.0, font[n++] );
        glEndList();
    }
    return _base;
}
//------------------------------------------------------------------- drawBitmap
/* drawBitmap: ビットマップフォントで文字列を描画する
 */
void ezFont_drawBitmap( float x, float y, char *string )
{
    glPushAttrib( GL_LIST_BIT|GL_CURRENT_BIT );
    glRasterPos2f( x, y );//描画位置の設定
    //文字フォントディスプレイリストの開始位置 */
    glListBase( _base );
    //ディスプレイリスト (文字列) を描画
    glCallLists( strlen( string ), GL_UNSIGNED_BYTE, (unsigned char *)string );
    glPopAttrib();
    return;
}
//--------------------------------------------------------------- drawGlutBitmap
/* drawGlutBitmap: glutビットマップフォントで文字列を描画する
 */
void ezFont_drawGlutBitmap( float x, float y, char *string, void *font )
{
    glPushAttrib( GL_CURRENT_BIT );//現在のラスターポジションの保存
    glRasterPos2f( x, y );
    //ビットマップ文字列の描画
    while( *string ){
        glutBitmapCharacter( font, *string++ );
    }
    glPopAttrib();//保存したラスターポジションのロード
    return;
}
//--------------------------------------------------------------- drawGlutStroke
/* drawGlutStroke
 */
void ezFont_drawGlutStroke( float x, float y, float z,
                            char *string,
                            float xscl, float yscl,
                            void *font )
{
    glPushMatrix();
    {
        glTranslatef( x, y, z );
        glScalef( xscl * 0.005, yscl * 0.005, 1.0 );
        //ストロークフォント文字列の描画
        while( *string ){
            glutStrokeCharacter( font, *string++ );
        }
    }
    glPopMatrix();
    return;
}
//==============================================================================
