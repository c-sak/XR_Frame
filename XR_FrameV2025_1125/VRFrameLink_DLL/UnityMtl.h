#ifndef __UnityMtl_h__
#define __UnityMtl_h__

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <cstring>
#include <vector>
#include <fstream>

//#if defined _WIN64 
//#include <GL/freeglut.h>
//#else
//#include <GL/glut.h>
//#endif
//
//#include <GL/glext.h>

#include "UnityPngImage.h"

using namespace std;

//#undef _WIN32
/*
#ifdef _WIN32
//typedef void (APIENTRY * PFNGLACTIVETEXTUREPROC)    (GLenum target);
PFNGLACTIVETEXTUREPROC glActiveTexture;
#endif
*/

/**
 *  @struct  St_uo_mtl_data
 *  @brief  MTLデータ構造体
 *  @date  2008/12/1 ... 新規作成
 */
struct St_uo_mtl_data
{
  string mtlnm;                                  // マテリアル定義名
  string mtlfilenm;                              // マテリアルファイル名

  bool  isAmbient;                               // 環境光設定の有無(true=有, false=無)
  float  ambient[3];                             // 環境光

  bool  isDiffuse;                               // 拡散光設定の有無(true=有, false=無)
  float  diffuse[3];                             // 拡散光

  bool  isSpecular;                              // 鏡面設定の有無(true=有, false=無)
  float  specular[3];                            // 鏡面光

  bool  isTransparency;                          // アルファ値の有無(true=有, false=無)
  float  transparency;                           // アルファ値

  bool  isShininess;                             // 鏡面係数設定の有無(true=有, false=無)
  int    shininess;                              // 鏡面係数

  bool  isIllumination;                          // イルミネーション設定の有無(true=有, false=無)
  int    illumination;                           // イルミネーション(1=鏡面光無効、2=鏡面光有効)

  int    tNo;                                    // テクスチャNo

  bool  isTexture;                               // テクスチャ設定の有無(true=有, false=無)
  int    textureNo;                              // テクスチャマッピング用テクスチャNo

  bool  isSphereTexture;                         // スフィアマッピング用テクスチャ設定の有無(true=有, false=無)
  int    sphereTextureNo;                        // スフィアマッピング用テクスチャNo
  string  texturefnm;                            // テクスチャファイル名(テクスチャマッピング、スフィアマッピング用)
  int    textureCnt;                             // キューブマッピング用カウンタ
  int    textureIndex[6];                        // キューブマッピング用インデックス
  int    cubeTextureNo[6];                       // キューブマッピング用テクスチャNo

  vector <string> v_texturefnm;                  // テクスチャファイル名(キューブマッピング用)
  bool  isCubeTexture;                           // キューブマッピング用テクスチャ設定の有無(true=有, false=無)

  //*******************************************************************
  /*!
   *  @brief  内部データの初期化を行います。
   *  @date  2008/12/1 ... 新規作成
   */
  //********************************************************************
  void init(void)
  {
    mtlnm.clear();                               // マテリアル定義名
    mtlfilenm.clear();                           // マテリアルファイル名

    isAmbient = false;                           // 環境光設定の有無
    ambient[0] = 0.0f;                           // 環境光
    ambient[1] = 0.0f;
    ambient[2] = 0.0f;

    isDiffuse = false;                           // 拡散光設定の有無
    diffuse[0] = 0.0f;                           // 拡散光
    diffuse[1] = 0.0f;
    diffuse[2] = 0.0f;

    isSpecular = false;                          // 鏡面設定の有無
    specular[0] = 0.0f;                          // 鏡面光
    specular[1] = 0.0f;
    specular[2] = 0.0f;

    isTransparency = false;                      // アルファ値の有無
    transparency = 1.0f;                         // アルファ値の有無

    isShininess = false;                         // 鏡面係数設定の有無

    shininess = 0;                               // 鏡面係数

    isIllumination = false;                      // イルミネーション設定の有無
    illumination = 1;                            // イルミネーション

    tNo = 0;                                     // テクスチャNo

    isTexture = false;                           // テクスチャ設定の有無
    textureNo = -1;                              // テクスチャマッピング用テクスチャNo
    isSphereTexture = false;                     // スフィアマッピング用テクスチャ設定の有無
    sphereTextureNo = -1;                        // スフィアマッピング用テクスチャNo

    texturefnm.clear();                          // テクスチャファイル名(テクスチャマッピング、スフィアマッピング用)

    textureCnt = 0;                              // キューブマッピング用カウンタ

    textureIndex[0] = 1;                         // キューブマッピング用インデックス
    textureIndex[1] = 3;
    textureIndex[2] = 5;
    textureIndex[3] = 2;
    textureIndex[4] = 4;
    textureIndex[5] = 0;

    cubeTextureNo[0] = -1;
    cubeTextureNo[1] = -1;
    cubeTextureNo[2] = -1;
    cubeTextureNo[3] = -1;
    cubeTextureNo[4] = -1;
    cubeTextureNo[5] = -1;

    v_texturefnm.clear();                        // テクスチャファイル名(キューブマッピング用)
    isCubeTexture = false;                       // キューブマッピング用テクスチャ設定の有無
  }
};

struct St_uo_texture_pool_data
{
  string texturefnm;                             // テクスチャファイル名
  int width;                                     // 横幅
  int height;                                    // 縦幅
  int channels;                                     // チャンネル数(3/4)
  int bits;                                      // 1チャンネルあたりビット数(8/16)
  int imageSize;                                 // バッファサイズ(byte)
  unsigned char* image;                          // テクスチャ画像バッファ

  void init(void)
  {
    texturefnm.clear();
    width = 0;
    height = 0;
    channels = 0;
    bits = 0;
    imageSize = 0;
    image = NULL;
  }
};

/** @class  uMtl
  * @brief  OBJ用マテリアルクラス
  * @author  Masakazu Yoshida(yoshida@iltj.jp)
  * @version  1.0
  * @date  2008/12/1 ... 新規作成
  * @attention  Copyright (C) 2008, ILTJ Inc.
　*/
class uMtl
{
/*--------------*/
/* 公開メンバ   */
/*--------------*/
public:
  //*******************************************************************
  /*!
   *  @brief  デフォルトコンストラクタ
   *  @date  2008/12/1 ... 新規作成
   */
  //********************************************************************
  uMtl(void);

  //*******************************************************************
  /*!
   *  @brief  デストラクタ
   *  @date  2008/12/1 ... 新規作成
   */
  //********************************************************************
  ~uMtl(void);

  //*******************************************************************
  /*!
   *  @brief  マテリアルファイルのロードを行います。
   *  @param  mtlfnm ... [In] マテリアルファイル名
   *  @retval  true  ... 正常終了
   *  @retval  false ... 委譲終了
   *  @date  2008/12/1 ... 新規作成
   */
  //********************************************************************
  bool loadMtl(string mtlfnm);

  //*******************************************************************
  /*!
   *  @brief  内部データの初期化を行います。
   *  @date  2008/12/1 ... 新規作成
   */
  //********************************************************************
  void clear(void);

  int getCount();

  string getName(int index);

  bool getInfo(int index,
    int* hasAmbient, float* ambient,
    int* hasDiffuse, float* diffuse,
    int* hasSpecular, float* specular,
    int* hasTransparency, float* transparency,
    int* hasShininess, int* shininess,
    int* hasIllumination, int* illumination,
    int* hasTexture, int* textureID,
    int* hasSphereTexture, int* sphereTextureID,
    int* hasCubeTexture, int* cubeTextureIDs);

  bool getTextureBufferInfo(int textureID,
    int* width, int* height,
    int* channels, int* bufferSize);

  bool getTextureBufferImage(int textureID, unsigned char* image, int bufferSize);

/*--------------*/
/* 非公開メンバ */
/*--------------*/
private:
  bool texturebind_flg;                          // テクスチャマッピングバインドフラグ
  bool spherebind_flg;                           // スフィアマッピングバインドフラグ
  bool cubebind_flg;                             // キューブマッピングバインドフラグ

  // mtl定義保持領域
  vector <St_uo_mtl_data> v_mtl;

  // テクスチャ名(ID)保持領域
  vector <int> v_tNo;

  // テクスチャバッファ保持領域
  vector <St_uo_texture_pool_data> v_texturePool;

  //*******************************************************************
  /*!
   *  @brief  内部データの出力を行います。
   *  @param  val ... [In] mtlデータ構造体
   *  @param  os ... [In] アウトプットストリーム
   *  @date  2008/12/1 ... 新規作成
   */
  //********************************************************************
  void show(const St_uo_mtl_data &val, ostream &os);
  
  //*******************************************************************
  /*!
   *  @brief  テクスチャのロードを行います。
   *  @param  pval ... [In] mtlデータ構造体
   *  @retval  true  ... 正常終了
   *  @retval  false ... 異常終了
   *  @date  2008/12/1 ... 新規作成
   */
  //********************************************************************
  bool loadTexture(St_uo_mtl_data *pval);

  //*******************************************************************
  /*!
   *  @brief  スフィアマッピング用テクスチャのロードを行います。
   *  @param  pval ... [In] mtlデータ構造体
   *  @date  2008/12/1 ... 新規作成
   */
  //********************************************************************
  bool loadSphereTexture(St_uo_mtl_data *pval);

  //*******************************************************************
  /*!
   *  @brief  キューブマッピング用テクスチャのロードを行います。
   *  @param  pval ... [In] mtlデータ構造体
   *  @date  2008/12/1 ... 新規作成
   */
  //********************************************************************
  bool loadCubeTexture(St_uo_mtl_data *pval);

  int registerTextureBuffer(const string& texturefnm);

  void clearTexturePool(void);
};

#endif
