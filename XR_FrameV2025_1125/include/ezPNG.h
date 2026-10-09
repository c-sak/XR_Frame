#pragma once

//PNG画像をファイルから読み込む
//w, h: 画像サイズ
//image: 画像バッファを指定しない場合には、新しく領域を確保し返す
unsigned char* readImagePNG(const char *file, int *w, int *h, unsigned char *image = nullptr);
void writeImagePNG(const char *file, unsigned char *image, int w, int h);

void nameRAW_PNG(char *rawfile, const char *infile, int w, int h, int d);
void saveRAW_PNG(char *rawfile, unsigned char *img, int x, int y);