#pragma once
#define USE_LIBJPEG

#include "ezWebCam.h"

typedef struct{
	int width;
	int height;
	unsigned int pixformat; //GLenum
	unsigned int pixtype;   //GLenum
	int pixsize; //data size of each pixel
	int texid; //GLuint texture id
	void *pixdata; //
} image_t;

void initImage( image_t *image, int width = 0, int height = 0, 
	unsigned int pixformat = GL_RGB,
	unsigned int pixtype = GL_UNSIGNED_BYTE );

image_t *newImage( int width = 0, int height = 0, 
	unsigned int pixformat = GL_RGB,
	unsigned int pixtype = GL_UNSIGNED_BYTE );

void deleteImage( image_t *image );

void loadImage( image_t *image, const char *file );
int loadImageJpg( image_t *image, const char *file );
void drawImage( image_t *image, int x, int y );
image_t *newImageJpg( const char *file );
void propertyImage( image_t *image, const char *text );

void saveImageJpg( image_t *image, const char *file );

void allocTexture(image_t *image);
void updateTexture(image_t *image);
void applyTexture( image_t *image = nullptr );

int makeMesh( int texid = 0, bool reverse = false );
void drawMesh( int texid, int meshid );
void drawMesh( image_t *image, int meshid );

class ezImage {
private:

public:
	int meshid;
	image_t *image;
	ezImage( int width = 0, int height = 0, 
	unsigned int pixformat = GL_RGB,
	unsigned int pixtype = GL_UNSIGNED_BYTE, bool reverse = true ){
		/*
		image = newImage();
		initImage( image, width, height, pixformat, pixtype ); 
		*/
		image = newImage( width, height, pixformat, pixtype ); 
		meshid = makeMesh( 0, reverse );
	}
	ezImage( const char *file ){
		image = newImageJpg( file );
		meshid = makeMesh();
		allocTexture(image);
	}
	~ezImage(){
		if( image->pixdata != nullptr ) delete image->pixdata;
	}
	void update( void ){
		updateTexture( image );
	}
	void loadJpg( const char *file ){
		loadImageJpg( image, file );
		if( image->texid == 0 ) allocTexture(image);
		else updateTexture( image );
	}
	void saveJpg( const char *file ){
		saveImageJpg( image, file );
	}
	void drawPixels( int x, int y ){
		drawImage( image, x, y );
	}
	void draw( void ) {
		drawMesh( image, meshid );
	}
	void preperty( const char *text ){
		propertyImage( image, text );
	}
	void apply( void ){
		applyTexture( image );
	}
	void unbind( void ){
		applyTexture(); //nullptr
	}
};

//-------- IMAGE TEXTURE CLASS ----------------------------------
class ezCamera {
public:
	ezCamera() {
		webcam = new WebCam();
	}
	~ezCamera() {}
	void open(int camID, int w, int h, float fps, 
		GLuint pixformat = GL_BGRA_EXT, char *devicename = nullptr ) {

		//img = new ezImage(w, h, GL_BGR_EXT, GL_UNSIGNED_BYTE);
		img = new ezImage(w, h, pixformat, GL_UNSIGNED_BYTE);
		unsigned char *buffer = (unsigned char *)img->image->pixdata;

		webcam->config(w, h, fps);
		if (devicename) {
			webcam->open(camID, devicename, buffer );
		}
		else {
			webcam->open(camID, buffer );
		}
	}
	//----------------
	void close()
	{
		webcam->close();
		delete webcam;
		delete img;
	}
	//----------------
	void capture(void) {
		if (webcam->isLatest()) update();
	}
	void copy(unsigned char *srcimage) { //¦–¢ƒeƒXƒg
		memcpy( img->image->pixdata, srcimage, 
			img->image->width * img->image->height * img->image->pixsize );
	}
	//----------------
	void update(void) {
		if (webcam->isLatest()) img->update();
	}
	//--------------
	void draw(float px = 0.0, float py = 0.0, float pz = 0.0,
		float sx = 1.0, float sy = 1.0, float sz = 1.0,
		float roll = 0.0)
	{
		glPushMatrix();
		glScalef(sx, sy, sz);
		glTranslatef(px, py, pz);
		glRotatef(roll, 0.0, 0.0, 1.0);
		img->draw();
		glPopMatrix();
	}
	//--------
	ezImage *image() {
		return img;
	}
	//--------
private:
	ezImage *img;
	WebCam *webcam;
};
