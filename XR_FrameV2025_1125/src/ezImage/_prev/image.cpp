#include "platform.h"

#include "image.h"
#ifdef USE_LIBJPEG
#include "ez/ezJpeg.h"
#endif

//----------------------------------------------------------------------------
void initImage( image_t *image, int width, int height, 
	unsigned int pixformat, unsigned int pixtype )
{
	int n_comp;
	int comp_size;
	//--------
	switch (pixformat) {
	case GL_RGB: //DEFAULT
	case GL_BGR_EXT:
		n_comp = 3;
		break;
	case GL_RGBA:
	case GL_BGRA_EXT:
		n_comp = 4;
		break;
	case GL_LUMINANCE:
		n_comp = 1;
		break;
	case GL_DEPTH_COMPONENT:
		n_comp = 1;
		break;
	default: //GL_RGB,GL_BGR_EXT
		n_comp = 3;
		break;
	}
	//-------- 
	switch (pixtype){
	case GL_UNSIGNED_BYTE://DEFAULT
		comp_size = sizeof(GLubyte );
		break;
	case GL_UNSIGNED_SHORT:
		comp_size = sizeof(GLushort);
		break;
	case GL_FLOAT:
		comp_size = sizeof(GLfloat);
		break;
	default: //GL_UNSIGNED_BYTE
		comp_size = sizeof(GLubyte );
		break;
	}
	//--------
	image->width = width;
	image->height = height;
	image->pixtype = pixtype;
	image->pixformat = pixformat;
	image->pixsize = n_comp * comp_size;
	if( width == 0 || height == 0 ) image->pixdata = nullptr;
	else image->pixdata = (void *)calloc( image->width * image->height, image->pixsize );

	return;
}
//-----------------------------------------------------
image_t *newImage( int width, int height, 
	unsigned int pixformat, unsigned int pixtype )
{
	image_t *image = new image_t;
	//image_t *image = (image_t *)calloc( sizeof(image_t), 1 );

	initImage( image, width, height, pixformat, pixtype );
	printf("NEW IMAGE SIZE %d (%d x %d x %d)\n", image->width*image->height*image->pixsize, image->width, image->height, image->pixsize);
	return image;
}
//-----------------------------------------------------
void deleteImage( image_t *image )
{
	free( image->pixdata );
	delete image;
	return;
}
//-----------------------------------------------------------
static void load_image( const char *file,void *buffer, 
    int width, int height, int pixsize )
{
    FILE *fp;
    fp = fopen( file, "rb" );
    if( fp == NULL ){
        perror( file );
		getchar();
        exit( -1 );
    }
    fread( buffer, pixsize, width * height, fp );
    fclose( fp );
    return;
}
//-----------------------------------------------------------
void loadImage( image_t *image, const char *file )
{
    load_image( file, image->pixdata, image->width, image->height, image->pixsize );
    return;
}
//------------------------------------------------------------
void propertyImage( image_t *image, const char *text )
{
	printf( "[%s] %d x %d x %d\n", text, image->width, image->height, image->pixsize  );
}
//-----------------------------------------------------------
image_t *newImageJpg( const char *file )
{
	image_t *image = newImage( 0, 0 );
	image->pixdata = readImageJPEG( file, &image->width, &image->height );
	return image;
}
//-----------------------------------------------------------
int loadImageJpg( image_t *image, const char *file )
{
	int w, h;
	//前提として画像領域は確保済であること
	//もしまだ画像領域がnullptrであれば領域を確保する
	//また、確保済みの領域とサイズが異なる場合には、
	//潜在的にエラーとなる。
	//エラーステータスを返すこととするが、
	//例外処理は行っていないので落ちる可能性あり
	image->pixdata = readImageJPEG( file, &w, &h, (unsigned char*)image->pixdata );
	image->width = w;
	image->height = h;
	if( w != image->width || h != image->height ){
		return -1;
	}
	return 0;
}
//------------------------------------------------------------
void saveImageJpg( image_t *image, const char *file )
{
	writeImageJPEG( file, (unsigned char *)image->pixdata,
		image->width, image->height );
}
//------------------------------------------------------------
void drawImage( image_t *image, int x, int y )
{
    if( image->pixdata != nullptr ){
        glRasterPos2i( x, y );
        glDrawPixels( image->width, image->height,
                      image->pixformat, image->pixtype,
                      (GLvoid *)( image->pixdata ) );
    }
	return;
}
//-------------------------------------------------------------
void allocTexture(image_t *image)
{
	GLuint texid;
	glGenTextures(1, &texid);
	image->texid = texid;
	glBindTexture(GL_TEXTURE_2D, image->texid);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	/*
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	*/
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	switch (image->pixformat) {
	case GL_RGB:
	case GL_BGR_EXT:
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->width, image->height, 0, image->pixformat,
			GL_UNSIGNED_BYTE, image->pixdata );
		break;
	case GL_RGBA:
	case GL_BGRA_EXT:
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->width, image->height, 0, image->pixformat,
			GL_UNSIGNED_BYTE, image->pixdata );
		break;
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	return;
}
//-------------------------------------------------------------
void applyTexture(image_t *image)
{
	glBindTexture(GL_TEXTURE_2D, image->texid);
}
//-------------------------------------------------------------
void updateTexture( image_t *image )
{
	glBindTexture(GL_TEXTURE_2D, image->texid);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 
		image->width, image->height, 
		image->pixformat,
		image->pixtype,
		image->pixdata );
	glBindTexture( GL_TEXTURE_2D, NULL );
}
//------------------------------------------------------------
int makeMesh( int texid )
{
	GLuint texmesh = glGenLists(1);
	glNewList(texmesh, GL_COMPILE);
	//glBindTexture(GL_TEXTURE_2D, texid);
	glColor4f(1.0, 1.0, 1.0, 1.0);
	glBegin(GL_POLYGON);
	{
		glTexCoord2d(0, 0);
		glVertex2d(0.0, 0.0);
		glTexCoord2d(1, 0);
		glVertex2d(1.0, 0.0);
		glTexCoord2d(1, 1);
		glVertex2d(1.0, 1.0);
		glTexCoord2d(0, 1);
		glVertex2d(0.0, 1.0);
	}
	glEnd();
	glEndList();
	return texmesh;
}
//-----------------------------------------------------------------------------
void drawMesh( int texid, int meshid )
{
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glBindTexture(GL_TEXTURE_2D, texid);
	glEnable(GL_TEXTURE_2D);
	glCallList(meshid);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	return;
}
void drawMesh( image_t *image, int meshid )
{
	drawMesh( image->texid, meshid );
}
//=============================================================