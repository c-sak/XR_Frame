#pragma once
//ezJpeg.h

//JPG画像をファイルから読み込む
//w, h: 画像サイズ
//image: 画像バッファを指定しない場合には、新しく領域を確保し返す
unsigned char* readImageJPEG( const char *file, int *w, int *h, 
	unsigned char *image = nullptr, bool reverse = true );
void writeImageJPEG( const char *file, unsigned char *image, int w, int h );

void nameRAW( char *rawfile, const char *infile, int w, int h, int d );
void saveRAW( char *rawfile, unsigned char *img, int x, int y );
