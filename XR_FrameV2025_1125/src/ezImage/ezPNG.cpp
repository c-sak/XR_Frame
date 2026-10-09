//#define _CRT_SECURE_NO_WARNINGS
#include "ezPNG.h"
#include "libpng/png.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned char* readImagePNG(const char *file, int *w, int *h, unsigned char *image) {

	png_structp m_png_ptr;
	png_infop m_info_ptr;
	png_bytep row;
	png_bytep px;
	png_bytep *row_pointers;
	unsigned char *rawimage;

	//ファイルを開く
	FILE *fp = fopen(file, "rb");
	if (!fp) {
		printf("Failed to open PNG file %s\n", file);
		png_destroy_read_struct(&m_png_ptr, &m_info_ptr, NULL);  // Cleanup
		return NULL;
	}

	//PNG構造体と情報構造体の作成
	m_png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!m_png_ptr) {
		printf("Failed to create PNG read struct\n");
		fclose(fp);
		return NULL;
	}

	m_info_ptr = png_create_info_struct(m_png_ptr);
	if (!m_info_ptr) {
		printf("Failed to create PNG info struct\n");
		png_destroy_read_struct(&m_png_ptr, (png_infopp)NULL, (png_infopp)NULL);  // Cleanup
		fclose(fp);
		return NULL;
	}

	//エラーハンドリング
	if (setjmp(png_jmpbuf(m_png_ptr))) {
		png_destroy_read_struct(&m_png_ptr, &m_info_ptr, (png_infopp)NULL);  // Cleanup
		fclose(fp);
		return NULL;
	}

	png_init_io(m_png_ptr, fp);
	png_read_info(m_png_ptr, m_info_ptr);
	
	// 画像の幅と高さを取得
	*w = png_get_image_width(m_png_ptr, m_info_ptr);
	*h = png_get_image_height(m_png_ptr, m_info_ptr);

	// PNGの色タイプとビット深度を取得
	png_byte color_type = png_get_color_type(m_png_ptr, m_info_ptr);
	png_byte bit_depth = png_get_bit_depth(m_png_ptr, m_info_ptr);

	// 16ビットの色を8ビットに変換
	if (png_get_bit_depth(m_png_ptr, m_info_ptr) == 16) {
		png_set_strip_16(m_png_ptr);
	}

	// RGBAで8ビット深度でない場合はエラー
	if (color_type != PNG_COLOR_TYPE_RGBA || bit_depth != 8) {
		printf("This PNG file is not 8bit!\n");
		return NULL;
	}

	// アルファチャンネルの有無を確認して拡張
	if (color_type == PNG_COLOR_TYPE_PALETTE) {
		png_set_palette_to_rgb(m_png_ptr);
	}
	if (color_type == PNG_COLOR_TYPE_GRAY&&bit_depth < 8) {
		png_set_expand_gray_1_2_4_to_8(m_png_ptr);
	}
	if (png_get_valid(m_png_ptr, m_info_ptr, PNG_INFO_tRNS)) {
		png_set_tRNS_to_alpha(m_png_ptr);
	}

	// RGBAまたはGRAY + ALPHAでない場合は、アルファチャンネルを追加
	if (color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_PALETTE) {
		png_set_add_alpha(m_png_ptr, 0xFF, PNG_FILLER_AFTER);
	}

	// 上記の設定に基づき、画像を読み込む前に行う更新
	png_read_update_info(m_png_ptr, m_info_ptr);

	//ピクセルデータの取得
	image = (unsigned char *)malloc(png_get_rowbytes(m_png_ptr, m_info_ptr) * (*h));
	row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * (*h));
	for (int y = 0; y < *h; y++) {
		row_pointers[y] = image + y * png_get_rowbytes(m_png_ptr, m_info_ptr);
	}

	//PNG画像データ読み込み
	png_read_image(m_png_ptr, row_pointers);

	/*
	// 画像データをimageバッファにコピー
	int row_bytes = png_get_rowbytes(m_png_ptr, m_info_ptr);
	unsigned char* pImage = image;

	for (int y = 0; y < *h; y++) {
		memcpy(pImage, row_pointers[y], row_bytes);
		pImage += row_bytes;
	}

	//PNGの終了処理
	for (int y = 0; y < *h; y++) {
		free(row_pointers[y]);
	}
	*/
	free(row_pointers);

	png_destroy_read_struct(&m_png_ptr, &m_info_ptr, (png_infopp)NULL);
	fclose(fp);

	return image;
}

void writeImagePNG(const char *file, unsigned char *image, int w, int h) {

	png_structp m_png_ptr;
	png_infop m_info_ptr;
	png_bytep row;

	//書き込みの初期化
	m_png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	m_info_ptr = png_create_info_struct(m_png_ptr);

	//イメージデータの書き込み
	FILE *fp = fopen(file, "wb");
	if (!fp) {
		printf("Failed to open PNG file %s for writing\n", file);
		return;
	}

	png_init_io(m_png_ptr, fp);
	png_set_IHDR(m_png_ptr, m_info_ptr, w, h, 8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
	png_write_info(m_png_ptr, m_info_ptr);

	row = (png_bytep)malloc(4 * w * sizeof(png_byte));
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			row[x * 4] = image[(y*w + x) * 4];
			row[x * 4 + 1] = image[(y*w + x) * 4 + 1];
			row[x * 4 + 2] = image[(y*w + x) * 4 + 2];
			row[x * 4 + 3] = image[(y*w + x) * 4 + 3];
		}
		png_write_row(m_png_ptr, row);
	}

	//PNG書き込みの終了処理
	png_write_end(m_png_ptr, NULL);
	png_destroy_write_struct(&m_png_ptr, &m_info_ptr);
	fclose(fp);
	free(row);
}
