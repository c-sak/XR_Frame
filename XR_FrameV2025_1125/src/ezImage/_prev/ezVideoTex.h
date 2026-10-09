#ifndef __EZ_VIDEO_TEX_H__
#define __EZ_VIDEO_TEX_H__

/*#############################################################################
* VIDEO TEXTURE MODULE
*20170706ezIMAGE claaí«â¡
*
 *###########################################################################*/

#include "platform.h"

#include "image.h" //Åö

#include "ezWebCam.h"

//-------- VIDEO TEXTURE CLASS --------
class ezVideoTex {
  public:
	ezVideoTex(){}
	~ezVideoTex(){}
	void Open( int camID, int w, int h, double fps ){

		image = newImage( w, h, GL_BGR_EXT, GL_UNSIGNED_BYTE );

		_fps = fps;
		_camID = camID;
		ezWebCam_Open( camID, w, h, fps );

		allocTexture(image);

		_meshID = makeMesh( 0, true );

	}
	void Close(){

		ezWebCam_Close( _camID );

		deleteImage( image );
		free( image );
	}
	void Update(){

		ezWebCam_GetImage( _camID, (unsigned char *)image->pixdata );
		updateTexture( image );

	}
	void Draw(){
		drawMesh( image->texid, _meshID );
	}
  private:

	double _fps;
	image_t *image; //Åö
	int _camID;
	GLuint _meshID;
};
//-------- IMAGE TEXTURE CLASS ----------------------------------
class ezImageTex {
public:
	ezImageTex() {
		webcam = new WebCam();
	}
	~ezImageTex() {}
	void Open(int camID, int w, int h, float fps, 
		GLuint pixformat = GL_BGR_EXT, char *devicename = NULL) {
		//open( w, h, GL_BGR_EXT ); //Microsoft LifeCam Rear
		//open( w, h, GL_BGRA_EXT );  //Microsoft LifeCam Front

		image = newImage(w, h, GL_BGR_EXT, GL_UNSIGNED_BYTE);
		allocTexture(image);
		_meshID = makeMesh( 0, true );

		webcam->config(w, h, fps);
		if (devicename) {
			webcam->open(camID, devicename, (unsigned char *)image->pixdata );
		}
		else {
			webcam->open(camID, (unsigned char *)image->pixdata );
		}
	}
	//----------------
	void Close()
	{
		webcam->close();
		deleteImage(image);
		free(image);
	}
	//----------------
	void Capture(void) {
		if (webcam->isLatest()) Update();
	}
	void Copy(unsigned char *srcimage) { //Å¶ñ¢ÉeÉXÉg
		memcpy( image->pixdata, srcimage, image->width * image->height * image->pixsize );
	}
	//----------------
	void Update(void) {
		if (webcam->isLatest()) updateTexture( image );
	}
	//--------------
	void Draw(float px = 0.0, float py = 0.0, float pz = 0.0,
		float sx = 1.0, float sy = 1.0, float sz = 1.0,
		float roll = 0.0)
	{
		glPushMatrix();
		glScalef(sx, sy, sz);
		glTranslatef(px, py, pz);
		glRotatef(roll, 0.0, 0.0, 1.0);
		drawMesh( image->texid, _meshID);////////
		glPopMatrix();
	}
	//--------
	image_t *Image() {
		return image;
	}
	//--------
private:
	image_t *image; //Åö
	GLuint _meshID;
	WebCam *webcam;
};
#endif