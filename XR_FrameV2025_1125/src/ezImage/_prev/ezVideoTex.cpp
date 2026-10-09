//#############################################################################
//
//T. OHSHIMA
//#############################################################################

#include "platform.h"

#include "ezVideoTex.h"

//=============================================================================
//-----------------------------------------------------------------------------
GLuint ezVideoTex_GenTex( int width, int height )
{
    GLuint texID;

    glPixelStorei( GL_PACK_ALIGNMENT, 1 );
    glGenTextures( 1, &texID );
    glBindTexture( GL_TEXTURE_2D, texID );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, NULL );
    glBindTexture( GL_TEXTURE_2D, 0 );

    return texID;
}
//-----------------------------------------------------------------------------
GLuint ezVideoTex_GenMesh()
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
void ezVideoTex_Draw( GLuint texID, int width, int height, unsigned char *buffer, GLuint meshID )
{
	// テクスチャメモリに画像データをロード
	glBindTexture( GL_TEXTURE_2D, texID );
	glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, width, height, GL_BGR_EXT, GL_UNSIGNED_BYTE, buffer );
	glEnable( GL_TEXTURE_2D );

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho( 0.0, 1.0, 0, 1.0, -1.0, 1.0 );
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadIdentity();
	
	glCallList( meshID );
    glPopMatrix();

	glBindTexture( GL_TEXTURE_2D, 0 ); //■テクスチャを解除
	glDisable( GL_TEXTURE_2D ); //■テクスチャマッピングOFF
	return;
}
