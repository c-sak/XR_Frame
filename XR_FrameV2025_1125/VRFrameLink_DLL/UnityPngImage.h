/** @file  PngImage.h
 *  @brief  PNG入出力関数
 *  @author  Masakazu Yoshida(yoshida@iltj.jp)
 *  @version  1.0
 *  @date  2008/12/1 ... 新規作成
 *  @attention  Copyright (C) 2008, ILTJ Inc.
 */

#ifndef __UnityPngImage_h__
#define __UnityPngImage_h__

#define _CRT_SECURE_NO_WARNINGS

//*******************************************************************
/*!
 *  @brief  PNGファイルを読込みます。
 *  @param  fname ... [In] PNGファイル名
 *  @param  width ... [In/Out] イメージの幅
 *  @param  height ... [In/Out] イメージの高さ
 *  @param  depth ... [In/Out] イメージの深度
 *  @param  bits ... [In/Out] イメージのビット[8/16]
 *  @param  image ... [In/Out] 読込んだイメージデータ
 *  @return  int ... リターンコード@n
 *                  [0:正常終了 / 1:ファイルオープンエラー / 2:ファイル読込みエラー]
 *  @date  2008/12/1 ... 新規作成
 */
//********************************************************************
int readPngImage( const char *fname,
                  int *width,
                  int *height,
                  int *depth,
                  int *bits,
                  unsigned char **image
                 );

//*******************************************************************
/*!
 *  @brief  イメージデータをPNGファイルに出力します。
 *  @param  width ... [In] イメージの幅
 *  @param  height ... [In] イメージの高さ
 *  @param  depth ... [In] 深度[3:RGB / 4:RGBA]
 *  @param  image ... [In] イメージデータ
 *  @param  file_name ... [In] 出力ファイル名
 *  @param  reverse ... [In] 出力順序(true:降順 / false:昇順)
 *  @return  int ... リターンコード@n
 *                   [1:ファイルオープンエラー / 2:png_structp構造体確保エラー / 3:png_infop構造体確保エラー]
 *  @date  2008/12/1 ... 新規作成
 */
//********************************************************************
int writePngImage( int width, int height, int depth, 
                   unsigned char *image, const char *fname, bool reverse );

#endif

