#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <png.h>
#include <zlib.h>
//-----------------------------------------------------------------//
// PNGファイルを読込みます。                                       //
//-----------------------------------------------------------------//
int readPngImage( const char *fname,
                  int *width,
                  int *height,
                  int *depth,
                  int *bits,
                  unsigned char **image
                )
{
    FILE            *fp;                         // ファイルポインタ
    png_structp     png_ptr;                     // png_structp構造体
    png_infop       info_ptr;                    // png_infop構造体
    unsigned long   pngWidth;                    // 画像ピクセルの横幅
    unsigned long   pngHeight;                   // 画像ピクセルの高さ
    unsigned long   row;                         // 1ラインのサイズ
    int bit_depth;                               // ビット深度
    int color_type;                              // カラー/アルファ チャンネルの設定
    int interlace_type;                          // インターレースの設定
    unsigned char   **pngImage;                  // イメージデータポインタ
    unsigned long   i;                           // 汎用カウンタ

    // PNGファイル形式でない場合
    if(!strstr(fname, ".png") && !strstr(fname, ".PNG"))
    {
      return 1;
    }

    // ファイル読込み
    if((fp = fopen(fname, "rb")) < 0)
    {
      return 1;
    }

    // png_structp構造体を確保・初期化
    png_ptr = png_create_read_struct( PNG_LIBPNG_VER_STRING, 
                                      NULL, NULL, NULL );
    // png_infop構造体を確保・初期化
    info_ptr = png_create_info_struct( png_ptr );

    // ファイルポインタの設定
    png_init_io( png_ptr, fp );

    // PNGファイルのヘッダ読込み
    png_read_info( png_ptr, info_ptr );

    // IHDRチャンク情報を取得
    png_get_IHDR( png_ptr, info_ptr, (png_uint_32 *)&pngWidth, (png_uint_32 *)&pngHeight,
                  &bit_depth, &color_type, &interlace_type,
                  NULL, NULL );

    // 1ラインに必要なバッファのサイズを取得
    row = png_get_rowbytes( png_ptr, info_ptr );

    // PNG用画像データ領域確保
    pngImage = (png_bytepp)malloc( pngHeight * sizeof(png_bytep) ); 

    // 画像用データ領域確保
    *image = (unsigned char *)malloc( pngHeight * row );

    for( i=0; i<pngHeight; i++ ){
      pngImage[i] = (png_bytep)malloc( row );
    }
    // PNG画像データ読込み
    png_read_image( png_ptr, pngImage );
    fclose(fp);

    for( i=0; i<pngHeight; i++ ){
      // opengl上では、画像の始点が左上から始まる為、データの順番を入れ替え
      memcpy( *image+(pngHeight-1-i)*row, pngImage[i], row );
      free( pngImage[i] );
    }
    free( pngImage );
    // ２つの構造体のメモリを解放
    png_destroy_read_struct( &png_ptr, &info_ptr, (png_infopp)NULL );

    *width  = pngWidth;
    *height = pngHeight;
    *depth  = row/pngWidth;
    *bits   = bit_depth;

    return 0;
}

//-----------------------------------------------------------------//
// PNGイメージをファイルに出力します。                             //
//-----------------------------------------------------------------//
int writePngImage( int width
          , int height
          , int depth
          , unsigned char *image
          , const char *file_name
          , bool reverse
         ) 
{
    int             i, n;
    int             bits = 8;
    unsigned char   **pngImage;

    FILE            *fp;
    png_structp     png_ptr;
    png_infop       info_ptr;

    // 出力ファイルオープン
    if((fp = fopen(file_name, "wb")) < 0)
    {
      return 1;
    }

    png_ptr = png_create_write_struct(           // png_structp構造体の確保・初期化
                    PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if( png_ptr == NULL ){
      fclose( fp );
      return 2;
    }

    info_ptr = png_create_info_struct(png_ptr);  // png_infop構造体の確保・初期化
    if( info_ptr == NULL ){
        png_destroy_write_struct( &png_ptr,  (png_infopp)NULL );
        fclose( fp );
        return 3;
    }

    png_init_io(png_ptr, fp);                    // ファイルポインタの設定
    png_set_filter(png_ptr, 0, PNG_ALL_FILTERS); // フィルタの設定
    png_set_compression_level(png_ptr, Z_BEST_COMPRESSION); // 圧縮の設定

    if( depth == 3 ){       // RGB
      png_set_IHDR( png_ptr, info_ptr, width, height, // RGB用チャンク情報の設定
                    bits, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
                    PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT );
    }
    else if( depth == 4 ){  // RGBA
      png_set_IHDR( png_ptr, info_ptr, width, height, // RGBA用チャンク情報の設定
                    bits, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE,
                    PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT );
    }
    else{                   // 上記以外
      png_set_IHDR( png_ptr, info_ptr, width, height, // グレイスケール用チャンク情報の設定
                    bits, PNG_COLOR_TYPE_GRAY, PNG_INTERLACE_NONE,
                    PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT );
    }

    png_set_gAMA( png_ptr, info_ptr, 1.0 );      // ファイル・ガンマの設定
    png_write_info( png_ptr, info_ptr );         // PNGファイルのヘッダを出力

    // PNG用画像データ領域確保
    pngImage = (png_bytepp)malloc( height * sizeof(png_bytep) ); 
    n = ((depth*width + 3) / 4) * 4;             // 4バイトの整数倍とする

    // 高さ分ループ
    for( i=0; i<height; i++ ){
    // 降順描画の場合
    if(reverse == true)
    {
      pngImage[i] = image + ((height-1) - i) * n;
    }
    // 昇順描画の場合
    else
    {
      pngImage[i] = image + i * n;
    }
  }

    png_write_image( png_ptr, pngImage );        // PNGファイルの画像データを出力
    free( pngImage );

    png_write_end( png_ptr, info_ptr );          // 残りの情報を出力
    png_destroy_write_struct( &png_ptr, &info_ptr ); // ２つの構造体のメモリを解放

    fclose(fp);                                  // 出力ファイルクローズ
    return 0;
}
