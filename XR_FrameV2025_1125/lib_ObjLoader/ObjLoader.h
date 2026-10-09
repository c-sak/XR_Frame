#ifndef __CObjLoader_h__
#define __CObjLoader_h__

#define _CRT_SECURE_NO_WARNINGS

#define _USE_MATH_DEFINES

#include <iostream>
#include <cstring>
#include <vector>
#include <cmath>

#include "mtl.h"
#include "ThreedVector.h"
#include "TextureCoordinate.h"
#include "ObjData.h"

using namespace std;

namespace objloader
{

#define _USE_MATH_DEFINES                        // M_PI用
#define SMOOTHING_ANGLE 90.0f                    // スムージング角度

/**
 *  @class    CObjLoader
 *  @brief    OBJデータアクセスクラス@n
 <p>
        本クラスではOBJデータの読込み、描画を行うことが可能です。@n
        @n
        使用方法は、以下の通りです。@n
          1.本クラスのインスタンスを生成します。@n
          2.read関数を使用して、OBJファイルを読込みます。@n
          @n
            以下の関数を使用して、読込んだデータを描画します。@n
            2.1.draw関数では、OBJデータを描画します。@n
            2.2.drawNormal関数では、OBJデータの法線を描画します。@n
            2.3.drawLine関数では、OBJデータをライン描画します。@n
          @n
          3.write関数を使用して、法線情報のないOBJファイルを読込み@n
            法線情報を付加したOBJファイルを出力します。@n
            出力ファイルは、「out_ + OBJファイル名」で出力されます。@n
          ※テクスチャの形式はpngのみ対応しています。
 </p>
 *  @version  1.0
 *  @date    2008/12/1
 *  @author    Masakazu Yoshida (yoshida@iltj.jp)
 *  @attention  Copyright (C) 2008, ILTJ Inc.
 *
 *  @mainpage
 *  <center>
 *  @n
 *    本機能は、モデリングソフトである「Metasequoia」及び「Maya」からOBJ形式で@n
 *    エクスポートされたものを読込み、OpenGLにて描画します。@n
 *    OBJのタグは仕様が曖昧であるため、最低限必要であるタグのみに対応しています。@n
 *
 *    以下に、本機能で対応しているタグ情報の一覧を示します。@n
 *
 *  <table border=1 cellspacing=0 cellpadding=4 width=700 align=center>
 *  <tr bgcolor="#dddddd">
 *    <td colspan=2><b>[ MTLタグ ]</b></td><td><center><b>Metasequoia</b></center></td><td><center><b>MAYA</b></center></td>
 *  </tr>
 *  <tr>
 *    <td width=20%><b>newmtl</b></td><td width=60%>マテリアル定義名</td><td width=10%><center>○</center></td><td width=10%><center>○</center></td>
 *  </tr>
 *  <tr>
 *    <td><b>Ka</b></td><td>環境光(Ambient)カラー</td><td><center>○</center></td><td><center>○</center></td>
 *  </tr>
 *  <tr>
 *    <td><b>Kd</b></td><td>拡散光(Diffuse)カラー</td><td><center>○</center></td><td><center>○</center></td>
 *  </tr>
 *  <tr>
 *    <td><b>Ks</b></td><td>鏡面光(Specular)カラー</td><td><center>○</center></td><td><center>○</center></td>
 *  </tr>
 *  <tr>
 *    <td><b>Ns</b></td><td>鏡面指数(Shininness)</td><td><center>○</center></td><td><center>○</center></td>
 *  </tr>
 *  <tr>
 *    <td><b>Tf</b></td><td>不透明度</td><td><center>×</center></td><td><center>○</center></td>
 *  </tr>
 *  <tr>
 *    <td><b>d</b></td><td>透明度</td><td><center>×</center></td><td><center>×</center></td>
 *  </tr>
 *  <tr>
 *    <td><b>illum</b></td><td>イルミネーション設定(1:鏡面光あり, 2:鏡面光なし)</td><td><center>×</center></td><td><center>○</center></td>
 *  </tr>
 *  </tr>
 *  <tr>
 *    <td><b>map_Ka</b></td><td>環境マップ</td><td><center>×</center></td><td><center>○</center></td>
 *  </tr>
 *  <tr>
 *    <td><b>map_Kd</b></td><td>テクスチャマップ</td><td><center>○</center></td><td><center>○</center></td>
 *  </tr>
 *  <tr>
 *    <td><b>refl</b></td><td>リフレクションマップ</td><td><center>×</center></td><td><center>○</center></td>
 *  </tr>
 *  </table>
 *
 *  <table border=1 cellspacing=0 cellpadding=4 width=700 align=center>
 *  <tr bgcolor="#dddddd">
 *    <td colspan=2><b>[ OBJタグ ]</b></td><td><center><b>Metasequoia</b></center></td><td><center><b>MAYA</b></center></td>
 *  </tr>
 *  <tr>
 *    <td width=20%><b>mtlib</b></td><td width=60%>マテリアルファイル名</td><td width=10%><center>○</center></td><td width=10%><center>○</center></td>
 *  </tr>
 *  <tr>
 *    <td><b>usemtl</b></td><td>マテリアル定義名</td><td><center>○</center></td><td><center>○</center></td>
 *  </tr>
 *  <tr>
 *    <td><b>v</b></td><td>頂点座標</td><td><center>○</center></td><td><center>○</center></td>
 *  </tr>
 *  <tr>
 *    <td><b>vt</b></td><td>頂点のUV座標</td><td><center>○</center></td><td><center>○</center></td>
 *  </tr>
 *  <tr>
 *    <td><b>vn</b></td><td>頂点の法線ベクトル</td><td><center>○</center></td><td><center>○</center></td>
 *  </tr>
 *  <tr>
 *    <td><b>f</b></td><td>面のインデックス</td><td><center>○</center></td><td><center>○</center></td>
 *  </tr>
 *  <tr>
 *    <td><b>g</b></td><td>グループ名</td><td><center>○</center></td><td><center>○</center></td>
 *  </tr>
 *  </table>
 *  @n
 *  
 *  </center>
 *
 */
class CObjLoader
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
    CObjLoader(void);

    //*******************************************************************
    /*!
     *  @brief  デストラクタ
     *  @date  2008/12/1 ... 新規作成
     */
    //********************************************************************
    ~CObjLoader(void);

    //*******************************************************************
    /*!
     *  @brief  引数であるOBJファイル名を使用して、OBJファイルを読込みます。
     *  @param  objfnm ... [In] OBJファイル名
     *  @retval  true  ... 正常終了
     *  @retval  false ... 異常終了
     *  @date  2008/12/1 ... 新規作成
     */
    //********************************************************************
    bool read(string objfnm);

    //*******************************************************************
    /*!
     *  @brief  read()で読込み済みのOBJモデルを描画します。
     *  @attention  事前にread()を呼び出している必要があります。
     *  @param  enlarge ... [In] 拡大比率
     *  @date  2008/12/1 ... 新規作成
     */
    //********************************************************************
    void draw(double enlarge);

    //*******************************************************************
    /*!
     *  @brief  read()で読込み済みのOBJモデルの法線を描画します。
     *  @attention  事前にread()を呼び出している必要があります。
     *  @param  enlarge ... [In] 拡大比率
     *  @date  2008/12/1 ... 新規作成
     */
    //********************************************************************
    void drawNormal(double enlarge);

    //*******************************************************************
    /*!
     *  @brief  read()で読込み済みのOBJモデルをライン描画します。
     *  @attention  事前にread()を呼び出している必要があります。
     *  @param  enlarge ... [In] 拡大比率
     *  @date  2008/12/1 ... 新規作成
     */
    //********************************************************************
    void drawLine(double enlarge);

    //*******************************************************************
    /*!
     *  @brief  法線なしのOBJモデルを、法線を付加してOBJファイルに出力します。@n
     *          １．引数であるOBJファイル名を使用して、OBJファイルを読込みます。@n
     *          ２．読込んだOBJに法線情報を付加して、OBJファイルを出力します。
     *  @attention  マテリアルファイルの出力は行いません。
     *  @param  inFileNm  ... [In] 入力OBJファイル名
     *  @param  outFileNm  ... [Out] 出力OBJファイル名
     *  @retval true  ... 正常終了
     *  @retval false ... 異常終了
     *  @date 2008/12/1 ... 新規作成
     */
    //********************************************************************
    bool write(string inFileNm, string outFileNm);

  /*--------------*/
  /* 非公開メンバ */
  /*--------------*/
  private:
    CMtl cmtl;                                     // OBJ用マテリアルクラス
    bool logFlg;                                   // ログフラグ(true:debug出力, false:release出力)

    int  debug_cnt;                                // デバッグ用カウンタ
    long vf_cnt;                                   // 頂点法線用インデックスカウンタ
    string pre_MaterialName;                       // 前回のマテリアル名
    bool isMakeDispList;                           // OBJ読込み時にディスプレイリスト作成有無(true:あり, false:なし)

    vector< CThreedVector > v_v;                   // 頂点座標用 vector
    vector< CThreedVector > v_vn;                  // 法線座標 vector
    vector< St_TextureCoordinate > v_vt;           // テクスチャ座標 vector
    vector< St_face_data > v_f;                    // 面のデータ vector

    vector< CObjData > objData;                    // OBJ基本データ

    GLuint displaylistNo;                          // ディスプレイリストNO
    GLuint lineDisplaylistNo;                      // 法線描画用ディスプレイリストNO

    vector< string > v_mtlfn;                      // マテリアルファイル名 vector

    //*******************************************************************
    /*!
     *  @brief  モデル描画のディスプレイリストを作成します。
     *  @param  isDelete ... [In] true:メモリ領域開放あり / false:メモリ領域開放なし
     *  @date  2008/12/1 ... 新規作成
     */
    //********************************************************************
    void makeDisplayList(bool isDelete);

    //*******************************************************************
    /*!
     *  @brief  オブジェクトの面の描画(1つの面データを描画)を行います。
     *  @param  val ... [In] 面データ構造体
     *  @date  2008/12/1 ... 新規作成
     */
    //********************************************************************
    void draw_face(CObjData &val);

    //*******************************************************************
    /*!
     *  @brief  オブジェクトの面の法線描画(1つの面データを描画)を行います。
     *  @param  val ... [In] 面データ構造体
     *  @date  2008/12/1 ... 新規作成
     */
    //********************************************************************
    void draw_normal(CObjData &val);

    //*******************************************************************
    /*!
     *  @brief  面法線の計算を行います。
     *  @param  val ... [In,Out] 面データ構造体
     *  @date  2008/12/1 ... 新規作成
     */
    //********************************************************************
    void culcFaceNormal(CObjData &val);
    
    //*******************************************************************
    /*!
     *  @brief  頂点法線の計算を行います。
     *  @param  val ... [In, Out] OBJ基本データオブジェクト
     *  @param  cntval ... [In] 現在の頂点インデックス
     *  @date  2008/12/1 ... 新規作成
     */
    //********************************************************************
    void culcVertexNormal(CObjData &val, int cntval);

    //*******************************************************************
    /*!
     *  @brief  内部データ初期化処理を行います。
     *  @date  2008/12/1 ... 新規作成
     */
    //********************************************************************
    void clear(void);

    //*******************************************************************
    /*!
     *  @brief  引数であるOBJファイル名を使用して、OBJファイルを読込みます。
     *  @attention  publicであるread()はインターフェースであり、本関数で実処理を行っています。@n
     *              ・read()経由で処理を行う場合は、ディスプレイリストの作成を行います。@n
     *              ・read()を経由しないで処理を行う場合は、ディスプレイリストの作成を行いません。
     *  @param  objfnm ... [In] OBJファイル名
     *  @retval  true  ... 正常終了
     *  @retval  false ... 異常終了
     *  @date  2008/12/1 ... 新規作成
     */
    //********************************************************************
    bool readTrans(string objfnm);

    //*******************************************************************
    /*!
     *  @brief  read()で読込み済みのOBJモデルをOBJファイルに出力します。
     *  @attention  マテリアルファイルの出力は行いません。@n
     *              また、事前にread()を呼び出している必要があります。
     *  @param  filenm  ... [In] 出力OBJファイル名
     *  @retval true  ... 正常終了
     *  @retval false ... 異常終了
     *  @date 2008/12/1 ... 新規作成
     */
    //********************************************************************
    bool writeTrans(string filenm);

  };
}

#endif
