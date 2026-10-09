#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
//#include <memory.h>
#include "libjpeg/jpeglib.h"
#include <string.h>

#include "ezJpeg.h"

unsigned char* readImageJPEG( const char *file, int *w, int *h, unsigned char *image )
{
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;

	unsigned char *rawimage, *p;
	JSAMPARRAY img;
	int i, j;
	int width;
	int height;

	FILE *infile;

	// JPEGオブジェクトの初期化
	cinfo.err = jpeg_std_error( &jerr );
	jpeg_create_decompress( &cinfo );

	// ファイルを開く
	infile = fopen( file, "rb" );
	jpeg_stdio_src( &cinfo, infile );

	// ヘッダの読み込み
	jpeg_read_header( &cinfo, TRUE );

	// 展開の開始
	jpeg_start_decompress( &cinfo );

	// 幅と高さの取得
	width = cinfo.output_width;
	height = cinfo.output_height;

	// イメージを保持するメモリ領域の確保と初期化
	img = (JSAMPARRAY)malloc( sizeof( JSAMPROW ) * height );
	for ( i = 0; i < height; i++ ) {
		img[i] = (JSAMPROW)calloc( sizeof( JSAMPLE ), 3 * width );
	}

	// 全イメージデータを取得	
	while( cinfo.output_scanline < cinfo.output_height ) {
		jpeg_read_scanlines( &cinfo,
			img + cinfo.output_scanline,
			cinfo.output_height - cinfo.output_scanline
		);
	}

	// 展開の終了
	jpeg_finish_decompress( &cinfo );

	// JPEGオブジェクトの破棄
	jpeg_destroy_decompress( &cinfo );

	// ファイルを閉じる
	fclose( infile );
	
	//イメージデータをコピーし保持するメモリ領域を開放
	if( image == NULL ){
		p = rawimage = (unsigned char*)malloc( width * height * 3 );
	}
	else{
		p = rawimage = image;
	}
	for ( i = height-1; i >= 0; i-- ){
	//for (i = 0; i < height; i++ ) {
		for( j= 0; j < width; j++ ){
			*p = img[i][j * 3 + 0 ]; p++;
			*p = img[i][j * 3 + 1 ]; p++;
			*p = img[i][j * 3 + 2 ]; p++;
		}
		free( img[i] );
	}
	free( img );
	
	*w = width;
	*h = height;
	return rawimage;
}

void writeImageJPEG( const char *file, unsigned char *image, int w, int h )
{
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	FILE *outfile;

	// JPEGオブジェクトの初期化
	cinfo.err = jpeg_std_error( &jerr );
	jpeg_create_compress( &cinfo );

	// ファイルを開く
	outfile = fopen( file, "wb" ); //★
	jpeg_stdio_dest( &cinfo, outfile );

	// パラメータの設定
	cinfo.image_width = w; //★
	cinfo.image_height = h; //★
	cinfo.input_components = 3;
	cinfo.in_color_space = JCS_RGB;

	// デフォルト値の設定
	jpeg_set_defaults( &cinfo );

	// 圧縮の開始
	jpeg_start_compress( &cinfo, TRUE );

	// イメージを保持するメモリ領域の確保と初期化
	JSAMPARRAY img;
	unsigned char *p = image;
	img = (JSAMPARRAY)malloc( sizeof( JSAMPROW ) * h );
	for ( int i = 0; i < h; i++ ) {
		img[i] = (JSAMPROW)calloc( sizeof( JSAMPLE ), 3 * w );
		for( int j = 0; j < w; j++ ){
			img[i][j * 3 + 2 ] = *p; p++;
			img[i][j * 3 + 1 ] = *p; p++;
			img[i][j * 3 + 0 ] = *p; p++;
			p++;
		}
	}
	// 全イメージデータを出力
	//jpeg_write_scanlines( &cinfo, (JSAMPARRAY)image, cinfo.image_height ); //★
	jpeg_write_scanlines( &cinfo, img, cinfo.image_height ); //★

	// 圧縮の終了
	jpeg_finish_compress( &cinfo );

	// JPEGオブジェクトの破棄
	jpeg_destroy_compress( &cinfo );

	// ファイルを閉じる
	fclose( outfile );	
}

void saveRAW( char *rawfile, unsigned char *img, int x, int y )
{
	FILE *fp;

	fp = fopen( rawfile, "wb" );
	fwrite( img, x * 3, y, fp );
	fclose( fp );
}

void nameRAW( char *rawfile, const char *infile, int w, int h, int d )
{
	char width[6], height[6];

	strcpy( rawfile, infile );
	*strchr( rawfile, '.' ) = '\0';
	sprintf( width, "%d", w );
	sprintf( height, "%d", h );

	strcat( rawfile, "_" );
	strcat( rawfile, width );
	strcat( rawfile, "x" );
	strcat( rawfile, height );
	switch( d ){
	case 1: //intensity
	strcat( rawfile, "xI.raw" );
		break;
	case 2: //intensity and alpha
		strcat( rawfile, "xIA.raw" );
		break;
	case 3: //rgb
		strcat( rawfile, "xRGB.raw" );
		break;
	case 4: //rgba
		strcat( rawfile, "xRGBA.raw" );
		break;
	}
}
