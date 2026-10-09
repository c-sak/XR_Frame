#include "platform.h"
#include "depth.h"
#include <math.h>

static const int width = 640;
static const int height = 480;

//---------------------------------------------------------- depthToZ
//KINECTからの距離データ[mm]をOpenGLのZ値に変換する
float _depthToZ( unsigned short depth, float n, float f )
{
	float z;
	z = - 0.001 * ( depth >> 3 ); //KINECTフォーマットからZ座標値への変換
	z = (f+n) / (f-n) + (2.0*n*f) / (f-n) / z; //投影変換[-1.0,1.0]
	z = z / 2.0 + 0.5; //正規化[0.0,1.0]
	return z;
}
//---------------------------------------------------------------------- depthToZ
void depthToZ( float *zimage, unsigned short *depth, int width, int height, float n, float f )
{
//	const int width  = 640;
//	const int height = 480;

	unsigned short *d;
	float *z;

	const float k = 0.001; //[mm]から[m]単位へ変換

	///////////////////////////////////////////
	float a = ((f+n)/(f-n))       /2.0 + 0.5;
	float b = (2.0*n*f/(k*(n-f))) /2.0;
	///////////////////////////////////////////

	//z = zimage;
	d = depth;

	//上下反転しつつ距離画像からZ値イメージに変換
	/*
	for( int i = 0; i < height; i++ ){
	    z = &zimage[ i * width ];
	}
	*/
	for( int i = height; i > 0; i-- ){ //上下反転
		z = &zimage[ ( i - 1) * width ];
		for( int j = 0; j < width; j++ ){
			*z = (*d==0) ? 1.0 : a + b / (float)(*d>>3);
			//*z = _depthToZ( *d, n, f );
			z++;
			d++;
		}
	}
}
//------------------------------------------------------------- drawZ
void drawToZ( float *zimage, int width, int height, int win_w, int win_h, bool reverse )
{
//	const int width = 640;
//	const int height = 480;

	glEnable( GL_DEPTH_TEST );
	glDepthFunc( GL_ALWAYS );

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho( 0, width, height, 0, -1.0, 1.0 );
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadIdentity();

	glColorMask( false, false, false, false );

	//glRasterPos2i( 0, win_h - 1 );
	//
	if( reverse ){
		glRasterPos2i( width - 1, height - 1 );
		glPixelZoom( -(float)win_w / width, (float)win_h / height );
	}
	else{
		glRasterPos2i( 0, height - 1 );
		glPixelZoom( (float)win_w / width, (float)win_h / height );
	}
	glDrawPixels( width, height, GL_DEPTH_COMPONENT, GL_FLOAT, zimage );
	/*
	gluScaleImage( GL_DEPTH_COMPONENT, width, height, GL_FLOAT, zimage,
					win_w, win_h, GL_FLOAT, zimage_full );
	glDrawPixels( win_w, win_h, GL_DEPTH_COMPONENT, GL_FLOAT, zimage_full );
	*/
	glColorMask( true, true, true, true );
	glPopMatrix();

	glDepthFunc( GL_LESS );
	glDisable( GL_DEPTH_TEST );
}
//----------------------------------------------------------------------------------------
void drawPointCloud( unsigned short *depth, unsigned char *image, 
	int width, int height, float fovy, float clip_near, float clip_far, bool mirror )
{
	//const float fovy = 43.0;
	float rad = fovy * 3.141592/180.0;
    const float F = (float)(1.0 / tanf( rad / 2.0 ));
//	const int width = 640;
//	const int height = 480;

	float aspect = (float)width/(float)height;

	float x, y, z;
	const float k = 0.001;
	unsigned char *pix;
	unsigned short *d;
	float dy, dx;
	dy = 1.0/height;
	dx = 1.0/width;
	float xo = - 0.5, yo = - 0.5;
	if( mirror ){
		xo *= -1.0;
		dx *= -1.0;
	}

	d = depth;
	pix = image;
	glPointSize( 4.0 );
	glBegin( GL_POINTS );
	//y = - 0.5;
	y = yo;
	for( int i = 0; i < height; i++ ){
		//----
		//y = (float)i/height - 0.5;
		//pix = &image[ 4 * i * width ];
		//----
		//x = - 0.5;
		x = xo;
		for( int j = 0; j < width; j++ ){
			//z = - (float)depth[ i * width + j ] * k;
			//z = - (float)*d * k;
			z = *d > 0  ? ( - (float)(*d>>3) * k ) : ( - clip_far );
			////////////////////////デプス値は上位13ビット/////////////////
			////////////////////////下位3ビットは捨てる   /////////////////
			d++;
			//----
			//x = (float)j/width - 0.5;
			//pix = &image[ 4 * ( i * width + j )];
			//----
			//glColor3f( (float)*(pix+2)/255.0, (float)*(pix+1)/255.0, (float)*(pix)/255.0);
			//pix += 2;
			//glColor3ub( *(pix), *(--pix), *(--pix) );
			glColor3ub( *(pix+2), *(pix+1), *pix);
			pix += 4;
			glVertex3f( x * z, y * z / aspect, z );
			x += dx;
		}
		y += dy;
	}
	glEnd();
	glPointSize( 1.0 );
}
