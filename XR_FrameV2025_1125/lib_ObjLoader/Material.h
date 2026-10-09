#ifndef __CMaterial_h__
#define __CMaterial_h__

#if defined _WIN64 
#include <GL/freeglut.h>
#else
#include <GL/glut.h>
#endif

/** @class  CMaterial
 *  @brief  マテリアルクラス
 *  @author  Masakazu Yoshida(yoshida@iltj.jp)
 *  @version  1.0
 *  @date  2008/12/1 ... 新規作成
 *  @attention  Copyright (C) 2008, ILTJ Inc.
 */
class CMaterial
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
  CMaterial(void);

//*******************************************************************
/*!
 *  @brief  デストラクタ
 *  @date  2008/12/1 ... 新規作成
 */
//********************************************************************
  ~CMaterial(void);

//*******************************************************************
/*!
 *  @brief  Diffuse(拡散光)設定を行います。
 *  @param  val ... [In] 表裏[GL_FRONT:表, GL_BACK:裏]
 *  @param  red ... [In] 赤色成分
 *  @param  green ... [In] 緑色成分
 *  @param  blue ... [In] 青色成分
 *  @param  alpha ... [In] アルファ値
 *  @date  2008/12/1 ... 新規作成
 */
//********************************************************************
  void setDiffuse(int val, GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);

//*******************************************************************
/*!
 *  @brief  Ambient(環境光)設定を行います。
 *  @param  val ... [In] 表裏[GL_FRONT:表, GL_BACK:裏]
 *  @param  red ... [In] 赤色成分
 *  @param  green ... [In] 緑色成分
 *  @param  blue ... [In] 青色成分
 *  @param  alpha ... [In] アルファ値
 *  @date  2008/12/1 ... 新規作成
 */
//********************************************************************
  void setAmbient(int val, GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);

//*******************************************************************
/*!
 *  @brief  Specular(鏡面光)設定を行います。
 *  @param  val ... [In] 表裏[GL_FRONT:表, GL_BACK:裏]
 *  @param  red ... [In] 赤色成分
 *  @param  green ... [In] 緑色成分
 *  @param  blue ... [In] 青色成分
 *  @param  alpha ... [In] アルファ値
 *  @date  2008/12/1 ... 新規作成
 */
//********************************************************************
  void setSpecular(int val, GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);

//*******************************************************************
/*!
 *  @brief  Emission(放射光)設定を行います。
 *  @param  val ... [In] 表裏[GL_FRONT:表, GL_BACK:裏]
 *  @param  red ... [In] 赤色成分
 *  @param  green ... [In] 緑色成分
 *  @param  blue ... [In] 青色成分
 *  @param  alpha ... [In] アルファ値
 *  @date  2008/12/1 ... 新規作成
 */
//********************************************************************
  void setEmission(int val, GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);

//*******************************************************************
/*!
 *  @brief  Shininess(鏡面指数)設定を行います。
 *  @param  val1 ... [In] 表裏 [GL_FRONT=表, GL_BACK=裏]
 *  @param  val2 ... [In] 鏡面指数
 *  @date  2008/12/1 ... 新規作成
 */
//********************************************************************
  void setShininess(int val1, GLfloat val2);

//*******************************************************************
/*!
 *  @brief  内部データの初期化を行います。
 *  @date  2008/12/1 ... 新規作成
 */
//********************************************************************
  void clear(void);

/*--------------*/
/* 非公開メンバ */
/*--------------*/
private:
  GLfloat diffuse[5];                            // DIFFUSE設定値
  GLfloat ambient[5];                            // AMBIENT設定値
  GLfloat specular[5];                           // SPECULAR設定値
  GLfloat emission[5];                           // EMISSION設定値
  GLfloat shininess[2];                          // SHAININESS設定値
};

#endif
