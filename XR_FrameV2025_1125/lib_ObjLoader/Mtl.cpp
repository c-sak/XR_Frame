#include ".\mtl.h"

/////////////////////////////
#ifdef _WIN32
PFNGLACTIVETEXTUREPROC glActiveTexture;
#endif
/////////////////////////////

//-----------------------------------------------------------------//
// デフォルトコンストラクタ                                        //
//-----------------------------------------------------------------//
CMtl::CMtl(void)
{
	//////////////////////////
#ifdef _WIN32
  glActiveTexture =
		(PFNGLACTIVETEXTUREPROC)wglGetProcAddress("glActiveTexture");
#endif
	//////////////////////////
  texturebind_flg = false;                       // テクスチャバインドフラグ初期化
  spherebind_flg = false;                        // スフィアテクスチャバインドフラグ初期化
  cubebind_flg = false;                          // キューブテクスチャバインドフラグ初期化
}

//-----------------------------------------------------------------//
// デストラクタ                                                    //
//-----------------------------------------------------------------//
CMtl::~CMtl(void)
{
}

//-----------------------------------------------------------------//
// マテリアルファイルのロードを行います。                          //
//-----------------------------------------------------------------//
bool CMtl::loadMtl(string mtlfnm)
{
  St_mtl_data *pst;                              // mtlデータ構造体一時領域
  int token_cnt = 0;                             // トークン用カウンタ
  GLuint tNo_temp;                               // テクスチャ名(ID)一時領域
  char line_buff[255];                           // レコード読込み用バッファ
  char bufftemp[255];                            // 一時領域バッファ
  char *psrc;                                    // 文字列検索用ポインタ
  char *token;                                   // 文字列分割用ポインタ
  ifstream ifs;                                  // 読込みファイル用ストリーム
  bool newflg = false;                           // 初回判定フラグ

  // 読込みファイルオープン
  ifs.open(mtlfnm.c_str(), ios::in);

  // オープンエラーチェック
  if(!ifs.is_open())
  {
    cout << "読込みファイルのオープンに失敗しました。 file: " << mtlfnm.c_str() << endl;
    return false;
  }

  // OBJ格納ディレクトリパスを取得
  string::size_type copysize = mtlfnm.rfind("/");
  string mtldir = mtlfnm.substr(0, copysize+1);

  ifs.unsetf(ios::skipws);                       // ブランクをスキップさせない

  string texpath;                                // テクスチャ格納ディレクトリパス

  // mtlファイル読込み
  while(!ifs.eof())
  {
    // 1レコード読込み
    ifs.getline(line_buff, 255 - 1);
    token_cnt = 0;

    // 先頭が「#」である場合
    if(line_buff[0] == '#')
    {
      ;                // スキップ
    }
    // マテリアル定義名
    else if(psrc = strstr(line_buff, "newmtl"))
    {
      // 1回目の処理である場合
      if(!newflg)
      {
        newflg = true;
      }
      // 上記以外である場合
      else
      {
        // mtl定義に追加
        v_mtl.push_back(*pst);
        // 領域開放
        delete pst;
      }
      pst = new St_mtl_data;
      pst->init();

      strcpy(bufftemp, (psrc + 7));
      pst->mtlnm = bufftemp;                     // マテリアル定義名設定
      pst->mtlfilenm = mtlfnm;                   // マテリアルファイル名を設定
    }
    // テクスチャマッピング
    else if(psrc = strstr(line_buff, "map_Kd"))
    {
      pst->isTexture = true;                     // テクスチャマッピングON

      texpath = mtldir;                          // テクスチャパスを設定
      texpath.append(psrc + 7);                  // テクスチャ名を追加
      pst->texturefnm = texpath;                 // テクスチャファイル名設定

      glGenTextures(1, &tNo_temp);               // テクスチャ名(ID)取得
      pst->tNo = tNo_temp;                       // テクスチャ名(ID)設定

      // テクスチャファイルロード
      if(!loadTexture(pst))
      {
        return false;
      }
    }

    // 環境(スフィア)マッピング
    else if(psrc = strstr(line_buff, "map_Ka"))
    {
      pst->isSphereTexture = true;

      texpath = mtldir;                          // テクスチャパスを設定
      texpath.append(psrc + 7);                  // テクスチャ名を追加
      pst->texturefnm = texpath;                 // テクスチャファイル名設定

      glGenTextures(1, &tNo_temp);               // テクスチャ名(ID)取得
      pst->tNo = tNo_temp;                       // テクスチャ名(ID)設定

      // テクスチャファイルロード
      loadSphereTexture(pst);
    }
    // リフレクション(キューブ)マッピング
    else if(psrc = strstr(line_buff, "refl"))
    {
      texpath.clear();                           // テクスチャ格納ディレクトリパスの初期化
      // 6つのテクスチャファイル名を設定
      if(psrc = strstr(line_buff, "cube_top"))
      {
        texpath = mtldir;                        // テクスチャパスを設定
        texpath.append(psrc + 9);                // テクスチャ名を追加
        pst->v_texturefnm.push_back(texpath);    // テクスチャファイル名(top)を設定
        pst->textureCnt++;                       // 読込み済みテクスチャ数+1
      }
      else if(psrc = strstr(line_buff, "cube_bottom"))
      {
        texpath = mtldir;                        // テクスチャパスを設定
        texpath.append(psrc + 12);               // テクスチャ名を追加
        pst->v_texturefnm.push_back(texpath);    // テクスチャファイル名(bottom)を設定
        pst->textureCnt++;                       // 読込み済みテクスチャ数+1
      }
      else if(psrc = strstr(line_buff, "cube_front"))
      {
        texpath = mtldir;                        // テクスチャパスを設定
        texpath.append(psrc + 11);               // テクスチャ名を追加
        pst->v_texturefnm.push_back(texpath);    // テクスチャファイル名(front)を設定
        pst->textureCnt++;                       // 読込み済みテクスチャ数+1
      }
      else if(psrc = strstr(line_buff, "cube_back"))
      {
        texpath = mtldir;                        // テクスチャパスを設定
        texpath.append(psrc + 10);               // テクスチャ名を追加
        pst->v_texturefnm.push_back(texpath);    // テクスチャファイル名(back)を設定
        pst->textureCnt++;                       // 読込み済みテクスチャ数+1
      }
      else if(psrc = strstr(line_buff, "cube_left"))
      {
        texpath = mtldir;                        // テクスチャパスを設定
        texpath.append(psrc + 10);               // テクスチャ名を追加
        pst->v_texturefnm.push_back(texpath);    // テクスチャファイル名(left)を設定
        pst->textureCnt++;                       // 読込み済みテクスチャ数+1
      }
      else if(psrc = strstr(line_buff, "cube_right"))
      {
        texpath = mtldir;                        // テクスチャパスを設定
        texpath.append(psrc + 11);               // テクスチャ名を追加
        pst->v_texturefnm.push_back(texpath);    // テクスチャファイル名(right)を設定
        pst->textureCnt++;                       // 読込み済みテクスチャ数+1
      }
      else
      {
        cout << "未知の refl が定義されました。 rec:" << line_buff << endl;
        return false;
      }

      // 6つのテクスチャ情報が揃った場合
      if(pst->textureCnt == 6)
      {
        pst->isCubeTexture = true;
        // テクスチャを読み、glTexImage2Dに設定
        loadCubeTexture(pst);
      }
    }

    // イルミネーション
    else if(psrc = strstr(line_buff, "illum"))
    {
      strcpy(bufftemp, (psrc + 6));
      pst->isIllumination = true;
      pst->illumination = atoi(bufftemp);        // イルミネーション値設定
    }
    // 環境光
    else if(psrc = strstr(line_buff, "Ka"))
    {
      pst->isAmbient = true;
      token = strtok((psrc + 3), " ");
      while(token != NULL)
      {
        // 環境光値設定(x, y, z)
        pst->ambient[token_cnt] = atof(token);
        token = strtok(NULL, " ");
        token_cnt++;
      }
    }
    // 拡散光
    else if(psrc = strstr(line_buff, "Kd"))
    {
      pst->isDiffuse = true;
      token = strtok((psrc + 3), " ");
      while(token != NULL)
      {
        // 拡散光値設定(x, y, z)
        pst->diffuse[token_cnt] = atof(token);
        token = strtok(NULL, " ");
        token_cnt++;
      }
    }
    // 鏡面光
    else if(psrc = strstr(line_buff, "Ks"))
    {
      pst->isSpecular = true;                    // 鏡面光指定ありに設定
      token = strtok((psrc + 3), " ");

      // トークンがなくなるまでループ
      while(token != NULL)
      {
        // 鏡面光値設定(x, y, z)
        pst->specular[token_cnt] = atof(token);
        token = strtok(NULL, " ");
        token_cnt++;
      }
    }
    // 鏡面係数
    else if(psrc = strstr(line_buff, "Ns"))
    {
      pst->isShininess = true;
      strcpy(bufftemp, (psrc + 3));
      pst->shininess = atoi(bufftemp);           // 鏡面係数設定
    }
    // アルファ値(透明度単一指定)
    else if(line_buff[0] == 'd')
    {
      // アルファ値のコピー
      strcpy(bufftemp, &line_buff[2]);

      // 指定値が1.0fより大きい場合
      if(1.0f < atof(bufftemp))
      {
        pst->isTransparency = true;
        pst->transparency = 1.0f;                // アルファ値最大値を設定
      }
      // 指定値が0.0fより小さい場合
      else if(0.0f > atof(bufftemp))
      {
        pst->isTransparency = true;
        pst->transparency = 0.0f;                // アルファ値最小値を設定
      }
      // 上記以外の場合
      else
      {
        pst->isTransparency = true;
        pst->transparency = atof(bufftemp);      // アルファ値設定
      }
    }

    // アルファ値(透明度RGB指定)
    else if(psrc = strstr(line_buff, "Tf"))
    {
      float tmpTransparency = 0.0f;
      int tmpTokenCnt = 0;
      char *tmpToken;

      tmpToken = strtok((psrc + 3), " ");

      // トークンがなくなるまでループ
      while(tmpToken != NULL)
      {
        // アルファ値を足し込む
        tmpTransparency += atof(tmpToken);

        tmpToken = strtok(NULL, " ");
        tmpTokenCnt++;
      }
      pst->isTransparency = true;

      // 取得した値の平均値を設定
      tmpTransparency = tmpTransparency / tmpTokenCnt;
      
      // 値が1より大きい場合
      if(1.0f < tmpTransparency)
      {
        tmpTransparency = 1.0f;
      }
      // 値が0より小さい場合
      else if(1.0 > tmpTransparency)
      {
        tmpTransparency = 0.0f;
      }

      // 値の設定
      pst->transparency = tmpTransparency;
    }

    // 上記以外
    else
    {
      ;  // スキップ
    }
  }
  ifs.close();

  // mtl定義に追加
  v_mtl.push_back(*pst);

  // 領域開放
  delete pst;

  return true;
}

//-----------------------------------------------------------------//
// マテリアル設定の適用を行います。                                //
//-----------------------------------------------------------------//
void CMtl::applyMtl(string mtlnm)
{
  // マテリアル定義の個数分ループ
  for(unsigned int i = 0; i < v_mtl.size(); i++)
  {
    // 指定したマテリアル定義である場合
    if(!v_mtl[i].mtlnm.compare(mtlnm))
    {
      if(v_mtl[i].isAmbient)
      {
        // 環境光設定
        material.setAmbient(GL_FRONT
                  , v_mtl[i].ambient[0]
                  , v_mtl[i].ambient[1]
                  , v_mtl[i].ambient[2]
                  , v_mtl[i].transparency);
      }
      if(v_mtl[i].isDiffuse)
      {
        // 拡散光設定
        material.setDiffuse(GL_FRONT
                  , v_mtl[i].diffuse[0]
                  , v_mtl[i].diffuse[1]
                  , v_mtl[i].diffuse[2]
                  , v_mtl[i].transparency);
      }
      if(v_mtl[i].isSpecular)
      {
        // 鏡面光設定
        material.setSpecular(GL_FRONT
                  , v_mtl[i].specular[0]
                  , v_mtl[i].specular[1]
                  , v_mtl[i].specular[2]
                  , v_mtl[i].transparency);
      }
      if(v_mtl[i].isShininess)
      {
        // 鏡面係数設定
        material.setShininess(GL_FRONT
                  , (float)v_mtl[i].shininess);
      }

      if(v_mtl[i].isIllumination)
      {
        // イルミネーション設定OFFである場合
        if(v_mtl[i].illumination == 1)
        {
          material.setSpecular(GL_FRONT
                    , 0.0f
                    , 0.0f
                    , 0.0f
                    , 0.0f);
        }
        // イルミネーション設定ONである場合
        else if(v_mtl[i].illumination == 2)
        {
          ; // 上記の鏡面光設定で、既に設定済みの為、スキップ
        }
      }

      // 透明度設定がある場合
      if(v_mtl[i].isTransparency)
      {
        // 透明度設定
        glColor4d(1.0, 1.0, 1.0, v_mtl[i].transparency);
      }
      // 透明度設定がない場合
      else
      {
        // 不透明に設定
        glColor4d(1.0, 1.0, 1.0, 1.0);
      }

      // テクスチャマッピング設定がある場合
      if(v_mtl[i].isTexture)
      {
        // テクスチャのバインド
        glBindTexture(GL_TEXTURE_2D, v_mtl[i].tNo);

        glEnable(GL_TEXTURE_2D);

        // 透明度設定があり、1.0より小さい指定である場合
        if(v_mtl[i].transparency < 1.0)
        {
          // テクスチャ環境
          glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
        }
        // 透明度設定がない場合
        else
        {
          // テクスチャ環境
          glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        }

        texturebind_flg = true;
      }
      // テクスチャ設定がない場合
      else
      {
        if(texturebind_flg)
        {
          texturebind_flg = false;
        }
      }

      // スフィアマッピング設定がある場合
      if(v_mtl[i].isSphereTexture)
      {
        // スフィアマッピングバインド
        glBindTexture(GL_TEXTURE_2D, v_mtl[i].tNo);

#ifdef _WIN32
        glActiveTexture(GL_TEXTURE1);
#endif

        glEnable(GL_TEXTURE_2D);

        /* テクスチャを拡大・縮小する方法の指定 */
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        /* テクスチャの繰り返し方法の指定 */
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

        /* テクスチャ環境 */
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

        // スフィアマッピング用テクスチャ座標を生成
        glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
        glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

        // テクスチャ座標の自動生成を有効にする
        glEnable(GL_TEXTURE_GEN_S);
        glEnable(GL_TEXTURE_GEN_T);

        spherebind_flg = true;
      }

      // キューブマッピング設定がある場合
      if(v_mtl[i].isCubeTexture)
      {
        // キューブマッピングバインド
        glBindTexture(GL_TEXTURE_CUBE_MAP, v_mtl[i].tNo);

#ifdef _WIN32
        glActiveTexture(GL_TEXTURE2);
#endif

        glEnable(GL_TEXTURE_CUBE_MAP);

        /* テクスチャを拡大・縮小する方法の指定 */
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        /* テクスチャの繰り返し方法の指定 */
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP);

        /* テクスチャ環境 */
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

        // スフィアマッピング用テクスチャ座標を生成
        glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
        glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
        glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);

        // テクスチャ座標の自動生成を有効にする
        glEnable(GL_TEXTURE_GEN_S);
        glEnable(GL_TEXTURE_GEN_T);
        glEnable(GL_TEXTURE_GEN_R);

        cubebind_flg = true;
      }

      break;
    }
  }
}

//-----------------------------------------------------------------//
// マテリアル設定の初期化を行います。                              //
//-----------------------------------------------------------------//
void CMtl::clearMtl(void)
{

#ifdef _WIN32
  // テクスチャユニット1に切り替え
  glActiveTexture(GL_TEXTURE1);
  glDisable(GL_TEXTURE_GEN_T);                   // テクスチャT座標の自動生成を無効にする
  glDisable(GL_TEXTURE_GEN_S);                   // テクスチャS座標の自動生成を無効にする
  glDisable(GL_TEXTURE_2D);                      // テクスチャマッピングOFF

  // テクスチャユニット2に切り替え
  glActiveTexture(GL_TEXTURE2);
#endif
  glDisable(GL_TEXTURE_GEN_T);                   // テクスチャT座標の自動生成を無効にする
  glDisable(GL_TEXTURE_GEN_S);                   // テクスチャS座標の自動生成を無効にする
  glDisable(GL_TEXTURE_GEN_R);                   // テクスチャR座標の自動生成を無効にする
  glDisable(GL_TEXTURE_CUBE_MAP);                // キューブマッピングOFF
  glDisable(GL_TEXTURE_2D);                      // スフィアマッピングOFF

#ifdef _WIN32
  // テクスチャユニット0に切り替え
  glActiveTexture(GL_TEXTURE0);
#endif
  glDisable(GL_TEXTURE_2D);                      // テクスチャマッピングマッピングOFF

  material.clear();
}

//-----------------------------------------------------------------//
// 内部データの初期化を行います。                                  //
//-----------------------------------------------------------------//
void CMtl::clear(void)
{
  texturebind_flg = false;                       // テクスチャマッピングバインドフラグ初期化
  spherebind_flg = false;                        // スフィアマッピングバインドフラグ初期化
  cubebind_flg = false;                          // キューブマッピングバインドフラグ初期化

  // mtl定義保持領域クリア
  v_mtl.clear();

  // テクスチャ名(ID)保持領域クリア
  v_tNo.clear();

  // マテリアル情報クリア
  material.clear();
}

//-----------------------------------------------------------------//
// 内部データの出力を行います。                                    //
//-----------------------------------------------------------------//
void CMtl::show(const St_mtl_data &val, ostream &os)
{
  os << "MTL名：" << val.mtlnm.c_str() << endl;
  os << "環境光R：" << val.ambient[0] << endl;
  os << "環境光G：" << val.ambient[1] << endl;
  os << "環境光B：" << val.ambient[2] << endl;

  os << "拡散光R：" << val.diffuse[0] << endl;
  os << "拡散光G：" << val.diffuse[1] << endl;
  os << "拡散光B：" << val.diffuse[2] << endl;

  os << "鏡面光R：" << val.specular[0] << endl;
  os << "鏡面光G：" << val.specular[1] << endl;
  os << "鏡面光B：" << val.specular[2] << endl;

  os << "アルファ値：" << val.transparency << endl;
  os << "鏡面係数：" << val.shininess << endl;
  os << "イルミネーション：" << val.illumination << endl;
  os << "テクスチャNo：" << val.tNo << endl;
  os << "テクスチャファイル名：" << val.texturefnm.c_str() << endl;

  os << "スフィアマッピング:";
  if(val.isSphereTexture)
  {
    os << "あり";
  }
  else
  {
    os << "なし";
  }
  os << endl;

  os << "キューブマッピング:";
  if(val.isCubeTexture)
  {
    os << "あり";
  }
  else
  {
    os << "なし";
  }
  os << endl;
}

//-----------------------------------------------------------------//
// テクスチャのロードを行います。                                  //
//-----------------------------------------------------------------//
bool CMtl::loadTexture(const St_mtl_data *pval)
{
  int ret;                                       // 戻り値
  int width;                                     // イメージの横幅
  int height;                                    // イメージの高さ
  int depth;                                     // イメージの深度
  int bits;                                      // イメージのビット値(8/16)
  unsigned char *fimage;                         // イメージデータ

  // テクスチャファイルの読込み
  ret = readPngImage(pval->texturefnm.c_str(), &width, &height, &depth, &bits, &fimage);
  if(ret == 1)
  {
    cout << "テクスチャファイルのオープンエラーが発生しました。 file = "
         << pval->texturefnm.c_str() << endl;
    return false;
  }
  else if(ret == 2)
  {
    cout << "テクスチャファイルの読込みエラーが発生しました。 file = "
         << pval->texturefnm.c_str() << endl;
    return false;
  }

  glEnable(GL_TEXTURE_2D);                       // テクスチャマッピング有効化

  glBindTexture(GL_TEXTURE_2D, pval->tNo);       // テクスチャのバインド

  // テクスチャ環境
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, fimage);

  glDisable(GL_TEXTURE_2D);                      // テクスチャマッピング無効化

  free(fimage);                                  // イメージ領域の開放
  return true;
}

//-----------------------------------------------------------------//
// スフィアマッピング用テクスチャのロードを行います。              //
//-----------------------------------------------------------------//
bool CMtl::loadSphereTexture(const St_mtl_data *pval)
{
  int ret;                                       // 戻り値
  int width;                                     // イメージの横幅
  int height;                                    // イメージの高さ
  int depth;                                     // イメージの深度
  int bits;                                      // イメージのビット値(8/16)
  unsigned char *fimage;                         // イメージデータ

  // テクスチャファイルの読込み
  ret = readPngImage(pval->texturefnm.c_str(), &width, &height, &depth, &bits, &fimage);
  if(ret == 1)
  {
    cout << "テクスチャファイルのオープンエラーが発生しました。 file = "
         << pval->texturefnm.c_str() << endl;
    return false;
  }
  else if(ret == 2)
  {
    cout << "テクスチャファイルの読込みエラーが発生しました。 file = "
         << pval->texturefnm.c_str() << endl;
    return false;
  }

  glBindTexture(GL_TEXTURE_2D, pval->tNo);       // テクスチャのバインド
#ifdef _WIN32
  glActiveTexture(GL_TEXTURE1);
#endif
  // テクスチャ画像はバイト単位に詰め込まれている
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  // テクスチャの割り当て
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0
        ,GL_RGB, GL_UNSIGNED_BYTE, fimage);
  delete[] fimage;

#ifdef _WIN32
  glActiveTexture(GL_TEXTURE0);
#endif

  return true;
}

//-----------------------------------------------------------------//
// キューブマッピング用テクスチャのロードを行います。              //
//-----------------------------------------------------------------//
bool CMtl::loadCubeTexture(const St_mtl_data *pval)
{
  int ret;                                       // 戻り値
  int width;                                     // イメージの横幅
  int height;                                    // イメージの高さ
  int depth;                                     // イメージの深度
  int bits;                                      // イメージのビット値(8/16)
  unsigned char *fimage;                         // イメージデータ

  // テクスチャのターゲット名
  int target[] =
  {
      GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,  // 下
      GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,  // 裏
      GL_TEXTURE_CUBE_MAP_POSITIVE_X,  // 右
      GL_TEXTURE_CUBE_MAP_POSITIVE_Z,  // 前
      GL_TEXTURE_CUBE_MAP_NEGATIVE_X,  // 左
      GL_TEXTURE_CUBE_MAP_POSITIVE_Y   // 上
  };

  glBindTexture(GL_TEXTURE_CUBE_MAP, pval->tNo);

#ifdef _WIN32
  glActiveTexture(GL_TEXTURE2);
#endif

  // テクスチャ数分(6つ)ループ
  for(int i = 0; i < pval->textureCnt; i++)
  {
    // テクスチャファイルの読込み
    ret = readPngImage(pval->v_texturefnm[pval->textureIndex[i]].c_str(), &width, &height, &depth, &bits, &fimage);
    if(ret == 1)
    {
      cout << "テクスチャファイルのオープンエラーが発生しました。 file = "
           << pval->v_texturefnm[pval->textureIndex[i]].c_str() << endl;
      return false;
    }
    else if(ret == 2)
    {
      cout << "テクスチャファイルの読込みエラーが発生しました。 file = "
           << pval->v_texturefnm[pval->textureIndex[i]].c_str() << endl;
      return false;
    }
    // テクスチャ画像はバイト単位に詰め込まれている
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // テクスチャの割り当て
    glTexImage2D(target[i], 0, GL_RGB, width, height, 0
          ,GL_RGB, GL_UNSIGNED_BYTE, fimage);

    delete[] fimage;
  }

#ifdef _WIN32
  glActiveTexture(GL_TEXTURE0);
#endif

  return true;
}
