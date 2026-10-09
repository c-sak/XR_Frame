#include "pch.h"
#include "UnityMtl.h"

/////////////////////////////
//#ifdef _WIN32
//PFNGLACTIVETEXTUREPROC glActiveTexture;
//#endif
/////////////////////////////

//-----------------------------------------------------------------//
// デフォルトコンストラクタ                                        //
//-----------------------------------------------------------------//
uMtl::uMtl(void)
{
	//////////////////////////
//#ifdef _WIN32
//  glActiveTexture =
//		(PFNGLACTIVETEXTUREPROC)wglGetProcAddress("glActiveTexture");
//#endif
	//////////////////////////
  texturebind_flg = false;                       // テクスチャバインドフラグ初期化
  spherebind_flg = false;                        // スフィアテクスチャバインドフラグ初期化
  cubebind_flg = false;                          // キューブテクスチャバインドフラグ初期化
}

//-----------------------------------------------------------------//
// デストラクタ                                                    //
//-----------------------------------------------------------------//
uMtl::~uMtl(void)
{
  clear();
}

//-----------------------------------------------------------------//
// マテリアルファイルのロードを行います。                          //
//-----------------------------------------------------------------//
bool uMtl::loadMtl(string mtlfnm)
{
  St_uo_mtl_data *pst;                              // mtlデータ構造体一時領域
  int token_cnt = 0;                             // トークン用カウンタ
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
      pst = new St_uo_mtl_data;
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

      // テクスチャIDはロード時にバッファプール登録で設定する

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

      // テクスチャIDはロード時にバッファプール登録で設定する

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
// 内部データの初期化を行います。                                  //
//-----------------------------------------------------------------//
void uMtl::clear(void)
{
  texturebind_flg = false;                       // テクスチャマッピングバインドフラグ初期化
  spherebind_flg = false;                        // スフィアマッピングバインドフラグ初期化
  cubebind_flg = false;                          // キューブマッピングバインドフラグ初期化

  // mtl定義保持領域クリア
  v_mtl.clear();

  // テクスチャ名(ID)保持領域クリア
  v_tNo.clear();

  // テクスチャバッファ保持領域クリア
  clearTexturePool();
}

void uMtl::clearTexturePool(void)
{
  for(unsigned int i = 0; i < v_texturePool.size(); i++)
  {
    if(v_texturePool[i].image != NULL)
    {
      free(v_texturePool[i].image);
      v_texturePool[i].image = NULL;
    }
  }

  v_texturePool.clear();
}

int uMtl::registerTextureBuffer(const string& texturefnm)
{
  if(texturefnm.empty())
  {
    return -1;
  }

  for(unsigned int i = 0; i < v_texturePool.size(); i++)
  {
    if(strcmp(v_texturePool[i].texturefnm.c_str(), texturefnm.c_str()) == 0)
    {
      return (int)i;
    }
  }

  int ret;
  int width;
  int height;
  int depth;
  int bits;
  unsigned char* image;

  ret = readPngImage(texturefnm.c_str(), &width, &height, &depth, &bits, &image);
  if(ret == 1)
  {
    cout << "テクスチャファイルのオープンエラーが発生しました。 file = "
         << texturefnm.c_str() << endl;
    return -1;
  }
  else if(ret == 2)
  {
    cout << "テクスチャファイルの読込みエラーが発生しました。 file = "
         << texturefnm.c_str() << endl;
    return -1;
  }

  St_uo_texture_pool_data texPoolData;
  texPoolData.init();
  texPoolData.texturefnm = texturefnm;
  texPoolData.width = width;
  texPoolData.height = height;
  texPoolData.channels = depth;
  texPoolData.bits = bits;
  // readPngImage() の depth は実データ上の1pixelあたりバイト数
  texPoolData.imageSize = width * height * depth;
  texPoolData.image = image;

  v_texturePool.push_back(texPoolData);
  return (int)v_texturePool.size() - 1;
}

//-----------------------------------------------------------------//
// 内部データの出力を行います。                                    //
//-----------------------------------------------------------------//
void uMtl::show(const St_uo_mtl_data &val, ostream &os)
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
bool uMtl::loadTexture(St_uo_mtl_data *pval)
{
  if(pval == NULL)
  {
    return false;
  }

  int textureID = registerTextureBuffer(pval->texturefnm);
  if(textureID < 0)
  {
    return false;
  }
  pval->textureNo = textureID;
  pval->tNo = textureID;

  //glEnable(GL_TEXTURE_2D);                       // テクスチャマッピング有効化

  //glBindTexture(GL_TEXTURE_2D, pval->tNo);       // テクスチャのバインド

  // テクスチャ環境
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  //glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, fimage);

  //glDisable(GL_TEXTURE_2D);                      // テクスチャマッピング無効化

  return true;
}

//-----------------------------------------------------------------//
// スフィアマッピング用テクスチャのロードを行います。              //
//-----------------------------------------------------------------//
bool uMtl::loadSphereTexture(St_uo_mtl_data *pval)
{
  if(pval == NULL)
  {
    return false;
  }

  int textureID = registerTextureBuffer(pval->texturefnm);
  if(textureID < 0)
  {
    return false;
  }
  pval->sphereTextureNo = textureID;
  pval->tNo = textureID;

//  glBindTexture(GL_TEXTURE_2D, pval->tNo);       // テクスチャのバインド
//#ifdef _WIN32
//  glActiveTexture(GL_TEXTURE1);
//#endif
  // テクスチャ画像はバイト単位に詰め込まれている
  //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  // テクスチャの割り当て
  //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0
  //      ,GL_RGB, GL_UNSIGNED_BYTE, fimage);
//#ifdef _WIN32
//  glActiveTexture(GL_TEXTURE0);
//#endif

  return true;
}

//-----------------------------------------------------------------//
// キューブマッピング用テクスチャのロードを行います。              //
//-----------------------------------------------------------------//
bool uMtl::loadCubeTexture(St_uo_mtl_data *pval)
{
  if(pval == NULL)
  {
    return false;
  }

  // テクスチャのターゲット名
  //int target[] =
  //{
  //    GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,  // 下
  //    GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,  // 裏
  //    GL_TEXTURE_CUBE_MAP_POSITIVE_X,  // 右
  //    GL_TEXTURE_CUBE_MAP_POSITIVE_Z,  // 前
  //    GL_TEXTURE_CUBE_MAP_NEGATIVE_X,  // 左
  //    GL_TEXTURE_CUBE_MAP_POSITIVE_Y   // 上
  //};

//  glBindTexture(GL_TEXTURE_CUBE_MAP, pval->tNo);
//
//#ifdef _WIN32
//  glActiveTexture(GL_TEXTURE2);
//#endif

  // テクスチャ数分(6つ)ループ
  for(int i = 0; i < pval->textureCnt; i++)
  {
    int textureID = registerTextureBuffer(pval->v_texturefnm[pval->textureIndex[i]]);
    if(textureID < 0)
    {
      return false;
    }

    if(i == 0)
    {
      pval->tNo = textureID;
    }
    pval->cubeTextureNo[i] = textureID;
    // テクスチャ画像はバイト単位に詰め込まれている
    //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // テクスチャの割り当て
    //glTexImage2D(target[i], 0, GL_RGB, width, height, 0
    //      ,GL_RGB, GL_UNSIGNED_BYTE, fimage);

  }

//#ifdef _WIN32
//  glActiveTexture(GL_TEXTURE0);
//#endif

  return true;
}

int uMtl::getCount()
{
    return (int)v_mtl.size();
}

string uMtl::getName(int index)
{
  if(index < 0 || (int)v_mtl.size() <= index)
  {
    return "";
  }

  return v_mtl[index].mtlnm;
}

bool uMtl::getInfo(int index,
  int* hasAmbient, float* ambient,
  int* hasDiffuse, float* diffuse,
  int* hasSpecular, float* specular,
  int* hasTransparency, float* transparency,
  int* hasShininess, int* shininess,
  int* hasIllumination, int* illumination,
  int* hasTexture, int* textureID,
  int* hasSphereTexture, int* sphereTextureID,
  int* hasCubeTexture, int* cubeTextureIDs)
{
  if(index < 0 || (int)v_mtl.size() <= index)
  {
    return false;
  }

  const St_uo_mtl_data& mtl = v_mtl[index];

  if(hasAmbient != NULL) *hasAmbient = mtl.isAmbient ? 1 : 0;
  if(ambient != NULL)
  {
    ambient[0] = mtl.ambient[0];
    ambient[1] = mtl.ambient[1];
    ambient[2] = mtl.ambient[2];
  }

  if(hasDiffuse != NULL) *hasDiffuse = mtl.isDiffuse ? 1 : 0;
  if(diffuse != NULL)
  {
    diffuse[0] = mtl.diffuse[0];
    diffuse[1] = mtl.diffuse[1];
    diffuse[2] = mtl.diffuse[2];
  }

  if(hasSpecular != NULL) *hasSpecular = mtl.isSpecular ? 1 : 0;
  if(specular != NULL)
  {
    specular[0] = mtl.specular[0];
    specular[1] = mtl.specular[1];
    specular[2] = mtl.specular[2];
  }

  if(hasTransparency != NULL) *hasTransparency = mtl.isTransparency ? 1 : 0;
  if(transparency != NULL) *transparency = mtl.transparency;

  if(hasShininess != NULL) *hasShininess = mtl.isShininess ? 1 : 0;
  if(shininess != NULL) *shininess = mtl.shininess;

  if(hasIllumination != NULL) *hasIllumination = mtl.isIllumination ? 1 : 0;
  if(illumination != NULL) *illumination = mtl.illumination;

  if(hasTexture != NULL)
  {
    *hasTexture = mtl.isTexture ? 1 : 0;
  }
  if(textureID != NULL)
  {
    if(mtl.isTexture)
    {
      *textureID = mtl.textureNo;
    }
    else
    {
      *textureID = -1;
    }
  }

  if(hasSphereTexture != NULL) *hasSphereTexture = mtl.isSphereTexture ? 1 : 0;
  if(sphereTextureID != NULL)
  {
    if(mtl.isSphereTexture)
    {
      *sphereTextureID = mtl.sphereTextureNo;
    }
    else
    {
      *sphereTextureID = -1;
    }
  }

  if(hasCubeTexture != NULL) *hasCubeTexture = mtl.isCubeTexture ? 1 : 0;
  if(cubeTextureIDs != NULL)
  {
    for(int i = 0; i < 6; i++)
    {
      if(mtl.isCubeTexture)
      {
        cubeTextureIDs[i] = mtl.cubeTextureNo[i];
      }
      else
      {
        cubeTextureIDs[i] = -1;
      }
    }
  }

  return true;
}

bool uMtl::getTextureBufferInfo(int textureID,
  int* width, int* height,
  int* channels, int* bufferSize)
{
  if(textureID < 0 || (int)v_texturePool.size() <= textureID)
  {
    return false;
  }

  if(width != NULL) *width = v_texturePool[textureID].width;
  if(height != NULL) *height = v_texturePool[textureID].height;
  if(channels != NULL) *channels = v_texturePool[textureID].channels;
  if(bufferSize != NULL) *bufferSize = v_texturePool[textureID].imageSize;

  return true;
}

bool uMtl::getTextureBufferImage(int textureID, unsigned char* image, int bufferSize)
{
  if(textureID < 0 || (int)v_texturePool.size() <= textureID)
  {
    return false;
  }

  if(image == NULL)
  {
    return false;
  }

  if(bufferSize < v_texturePool[textureID].imageSize)
  {
    return false;
  }

  memcpy((void*)image,
    (const void*)v_texturePool[textureID].image,
    sizeof(unsigned char) * v_texturePool[textureID].imageSize);

  return true;
}
