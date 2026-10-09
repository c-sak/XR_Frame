#include "pch.h"
#include "UnityObjLoader.h"

using namespace objloader;

//-----------------------------------------------------------------//
// デフォルトコンストラクタ                                        //
//-----------------------------------------------------------------//
uObjLoader::uObjLoader(void)
{
  debug_cnt = 0;
}

//-----------------------------------------------------------------//
// デストラクタ                                                    //
//-----------------------------------------------------------------//
uObjLoader::~uObjLoader(void)
{
}

//-----------------------------------------------------------------//
// OBJファイルを読込みます。                                       //
//-----------------------------------------------------------------//
bool uObjLoader::read(string objfnm)
{
  isMakeDispList = true;                         // ディスプレイリスト作成あり

  return (readTrans(objfnm));                    // OBJファイル読込み
}

//-----------------------------------------------------------------//
// 法線なしのOBJモデルを、法線を付加してOBJファイルに出力します。  //
//-----------------------------------------------------------------//
bool uObjLoader::write(string inFileNm, string outFileNm)
{
  bool ret = true;                               // 戻り値
  isMakeDispList = false;                        // ディスプレイリスト作成なし

  // OBJファイルの読込み
  if(!readTrans(inFileNm))
  {
    ret = false;                                 // 異常終了に設定
  }
  // OBJファイルの出力
  else if(!writeTrans(outFileNm))
  {
    ret = false;                                 // 異常終了に設定
  }
  clear();                                       // 内部データ初期化

  return ret;                                    // 呼び出し元に返す
}

//-----------------------------------------------------------------//
//  内部データ初期化処理を行います。                               //
//-----------------------------------------------------------------//
void uObjLoader::clear(void)
{
  debug_cnt = 0;                                 // デバッグ用カウンタ
  vf_cnt = 1;                                    // 頂点法線用インデックスカウンタ

  cmtl.clear();

  v_v.clear();                                   // 頂点座標用 vector
  v_vn.clear();                                  // 法線座標 vector
  v_vt.clear();                                  // テクスチャ座標 vector
  v_f.clear();                                   // 面のデータ vector

  objData.clear();                               // OBJ基本データ
  v_mtlfn.clear();                               // マテリアルファイル名 vector
}

//-----------------------------------------------------------------//
// 引数であるOBJファイル名を使用して、OBJファイルを読込みます。    //
//-----------------------------------------------------------------//
bool uObjLoader::readTrans(string objfnm)
{
  bool ret = false;                              // 戻り値

  ifstream ifs;                                  // 入力ファイル用ストリーム

  char line_buff[255];                           // ラインバッファ
  char *psrc;                                    // 文字列検索用ポインタ
  char *ptemp;                                   // 汎用ポインタ
  char *token;                                   // 文字列分割用ポインタ

  string mtl_buff;                               // マテリアル定義名バッファ
  string group_buff;                             // グループ名バッファ

  vector<double> v_temp;                         // 一時領域

  uoThreedVector vectorCtv;                       // ベクトル一時領域
  uoThreedVector normalCtv;                       // 法線ベクトル一時領域

  St_uo_TextureCoordinate textureCoordinate_temp;   // テクスチャ座標一時領域

  int groupNoCnt = 0;                            // グループNOカウンタ

  uObjData objData_temp;                         // 一時OBJデータ
  uObjDetailData objDetailData_temp;             // 一時OBJ詳細データ

  bool vn_flg = false;                           // VN設定フラグ(true:存在する, false:存在しない)

  clear();                                       // 初期化処理

  ifs.open(objfnm.c_str(), ios::in);             // objファイルオープン

  // ファイルオープンチェック
  if(!ifs.is_open())
  {
    cout << "読込みファイルのオープンに失敗しました。file: " << objfnm.c_str() << endl;
    return ret;
  }

  // OBJ格納ディレクトリパスを取得
  string::size_type copysize = objfnm.rfind("/");
  string objdir = objfnm.substr(0, copysize+1);

  ifs.unsetf(ios::skipws);                       // ブランクをスキップさせない
  string mtlpath;                                // mtl格納ディレクトリパス

  // EOFが検出されるまでループ
  while(!ifs.eof())
  {
    ifs.getline(line_buff, 255 - 1);             // １行読込み
    psrc = line_buff;

    // 先頭が「#」である場合(コメント扱い)
    if(*psrc == '#')
    {
      ;  // スキップ
    }
    else if(*psrc == 'v')
    {
      // 法線座標の指定である場合
      if(*(psrc + 1) == 'n')
      {
        // ブランクを区切り文字として検索
        token = strtok((psrc + 3), " ");
        for(int i = 0; token != NULL; i++, token = strtok(NULL, " "))
        {
          normalCtv[i] = atof(token);            // 法線座標を設定
        }
        // 単位ベクトルで再作成
        uoThreedVector Ctvtmp(normalCtv.getNormalizeALL());
        v_vn.push_back(Ctvtmp);                  // 法線ベクトルを追加
        vn_flg = true;                           // VN定義有無フラグをON
      }
      // テクスチャ座標の指定である場合
      else if(*(psrc + 1) == 't')
      {
        // ブランクを区切り文字として検索
        token = strtok((psrc + 3), " ");
        for(int i = 0; token != NULL; i++, token = strtok(NULL, " "))
        {
          // U座標値である場合
          if(i == 0)
          {
            textureCoordinate_temp.u = atof(token);// テクスチャU座標を設定
          }
          // V座標値である場合
          else if(i == 1)
          {
            textureCoordinate_temp.v = atof(token);// テクスチャV座標を設定
          }
          // 上記以外である場合
          else
          {
            ;
          }
        }
        v_vt.push_back(textureCoordinate_temp);  // テクスチャ座標 vectorに追加

      }
      // 頂点座標の指定である場合
      else
      {
        // ブランクを区切り文字として検索
        token = strtok((psrc + 2), " ");

        // トークンが存在する場合
        if(token != NULL)
        {
          // トークンがなくなるまでループ
          for(int i = 0; token != NULL; i++, token = strtok(NULL, " "))
          {
            vectorCtv[i] = atof(token);          // 頂点座標を設定
          }
        }
        v_v.push_back(vectorCtv);                // 頂点座標データを追加
      }
    }
    // 描画定義
    else if(*psrc == 'f')
    {
      vector< vector<int> > v_i;                 // 面を管理
      vector<int> v_i_temp;                      // 頂点座標を管理
      vector<char*> v_c;                         // 文字列分割用一時領域
      St_uo_vf_detail_data vf_data;                 // インデックス管理用一時領域

      int cnt = 0;                               // 引数カウンタ
      bool flg = false;                          // テクスチャ座標指定有無(true:無, false:有)

      // ブランクを区切り文字として検索
      token = strtok((psrc + 2), " ");

      // 頂点数分切り分け
      while(token != NULL)
      {
        v_c.push_back(token);
        token = strtok(NULL, " ");
      }

      // トークンがなくなるまでループ
      for(unsigned int i = 0; i< v_c.size(); i++)
      {
        // テクスチャ座標指定がない場合(f (v)//(vn))
        if(strstr(v_c[i], "//"))
        {
          // テクスチャ座標指定なし
          flg = true;
        }
        // 「/」を区切り文字として分割する
        token = strtok(v_c[i], "/");
        for(int i = 0; token != NULL; i++, token = strtok(NULL, "/"))
        {
          vf_data.detail_data[i] = atol(token);  // 頂点座標No、テクスチャ座標No、法線座標Noのいずれかを設定

          cnt++;                                 // トークン数をインクリメント
        }

        // テクスチャ座標指定がない、又はトークンが３つより少ない場合
        if(flg || cnt < 3)
        {
          // テクスチャ座標指定がない場合
          if(flg)
          {
                                                 // 使用する領域を変更
            vf_data.detail_data[2] = vf_data.detail_data[1];

            vf_data.detail_data[1] = 0;          // 使用しない領域に0を設定
            flg = false;                         // フラグの初期化
          }
          // トークンが３つより少ない場合
          else
          {
            // ３つ目の要素を0に設定
            vf_data.detail_data[2] = 0;          // 使用しない領域に0を設定
          }

          // 頂点座標の指定しかない場合(f (v))
          if(cnt == 1)
          {
            vf_data.detail_data[2] = 0;          // 使用しない領域に0を設定
          }
        }
        // 座標インデックス情報設定
        objData_temp.addIndex(vf_data);

        // 頂点情報設定
        objDetailData_temp.setVertexCoordinate(v_v[vf_data.detail_data[0] - 1].getPointAll());

        // テクスチャ情報が存在する場合
        if(vf_data.detail_data[1])
        {
          // テクスチャ情報の設定
          objData_temp.setTextureFlg(true);
          objDetailData_temp.setTextureCoordinateU(v_vt[vf_data.detail_data[1] - 1].u);
          objDetailData_temp.setTextureCoordinateV(v_vt[vf_data.detail_data[1] - 1].v);
        }
        // テクスチャ情報が存在しない場合
        else
        {
          // テクスチャ情報を無しに設定
          objData_temp.setTextureFlg(false);
        }

        // 法線情報が存在する場合
        if(vn_flg && vf_data.detail_data[2])
        {
          // 法線情報の設定
          uoThreedVector temp(v_vn[vf_data.detail_data[2] - 1].getPointAll());

          objData_temp.setNormalFlg(true);
          objData_temp.setFaceNormal(&temp);
          objDetailData_temp.setNormalCoordinate(v_vn[vf_data.detail_data[2] - 1].getPointAll());
        }
        // 法線情報が存在しない場合
        else
        {
          // 法線情報を無しに設定
          objData_temp.setNormalFlg(false);
        }

        // OBJ基本情報に追加
        objData_temp.addObjDetailData(objDetailData_temp);

        cnt = 0;
      }

      objData_temp.setGroupNo(groupNoCnt);       // グループNo設定
      objData_temp.setGroupName(group_buff);     // グループ名設定
      objData_temp.setMtlName(mtl_buff);         // マテリアル定義名設定

      // Quad/Polygonを三角形に分割（Fan triangulation）
      int vertCount = (int)objData_temp.getIndexSize();
      if(vertCount <= 3)
      {
        objData.push_back(objData_temp);         // 三角形以下はそのまま追加
      }
      else
      {
        // 頂点0を基点にFan triangulationで三角形へ分割
        for(int ti = 1; ti <= vertCount - 2; ti++)
        {
          uObjData triData;
          triData.setGroupNo(groupNoCnt);
          triData.setGroupName(group_buff);
          triData.setMtlName(mtl_buff);
          triData.setNormalFlg(objData_temp.getNormalFlg());
          triData.setTextureFlg(objData_temp.getTextureFlg());

          if(objData_temp.getNormalFlg())
          {
            triData.setFaceNormal(objData_temp.getFaceNormal());
          }

          int triIdx[3] = { 0, ti, ti + 1 };
          for(int k = 0; k < 3; k++)
          {
            triData.addIndex(objData_temp.getIndex(triIdx[k]));
            triData.addObjDetailData(*objData_temp.getObjDetailData(triIdx[k]));
          }

          objData.push_back(triData);
        }
      }
      objData_temp.clear();                      // OBJデータの初期化
    }
    // グルーピング定義
    else if(*psrc == 'g')
    {
      bool findflg = false;                      // 検索HITフラグ

      // ロード済み面数分ループ
      for(unsigned int gcnt = 0; gcnt < objData.size(); gcnt++)
      {
        // グループ名が同じである場合
        if(strcmp(objData[gcnt].getGroupName().c_str(), psrc + 2) == 0)
        {
          group_buff = psrc + 2;                 // 現時点でのグループ名を設定
          findflg = true;                        // 検索HITフラグON
          break;
        }
      }

      // 検索にHITしなかった場合
      if(!findflg)
      {
        group_buff = psrc + 2;                   // 現時点でのグループ名を設定
      }
    }
    // mtlファイルデータ取り込み指定である場合
    else if(ptemp = strstr(psrc, "mtllib"))
    {
      // OBJ格納ディレクトリパスを取得
      string::size_type copysize = objfnm.rfind("/");
      mtlpath = objfnm.substr(0, copysize+1);

      mtlpath.append(ptemp + 7);
      if(!cmtl.loadMtl(mtlpath))                 // mtlファイル読込み
      {
        return false;
      }
      v_mtlfn.push_back(ptemp + 7);              // マテリアルファイル名を追加
    }
    // マテリアル定義設定の指定である場合
    else if(ptemp = strstr(psrc, "usemtl"))
    {
      mtl_buff = ptemp + 7;
    }
    else
    {
      ;                                          // スキップ
    }
  }
  ifs.close();                                   // objファイルクローズ

  ret = true;

  return ret;
}

//-----------------------------------------------------------------//
// read()で読込み済みのOBJモデルをOBJファイルに出力します。        //
//-----------------------------------------------------------------//
bool uObjLoader::writeTrans(string filenm)
{
  bool ret = false;                              // 戻り値
  unsigned int i = 0;

  ofstream ofs(filenm.c_str(), ios::out);        // objファイルオープン

  // ファイルオープンチェック
  if(!ofs.is_open())
  {
    cout << "出力ファイルのオープンに失敗しました。file: " << filenm.c_str() << endl;
    return ret;
  }

  ofs << "# Create by OBJ Loader." << endl;
  ofs << endl;

  // 使用するマテリアルファイルの宣言
  for(i = 0; i < v_mtlfn.size(); i++)
  {
    ofs << "mtllib " << v_mtlfn[i].c_str() << endl;
  }
  ofs << "# " << i << " material files." << endl;
  ofs << endl;

  // 頂点座標の列挙
  for(i = 0; i < v_v.size(); i++)
  {
    ofs << "v " << v_v[i].getPointX() << " "
                << v_v[i].getPointY() << " "
                << v_v[i].getPointZ() << endl;
  }
  ofs << "# " << i << " vertices." << endl;
  ofs << endl;

  // テクスチャ座標の列挙
  for(i = 0; i < v_vt.size(); i++)
  {
    //ofs << "vt " << v_vt[i].getU() << " "
    //             << v_vt[i].getV() << endl;
    ofs << "vt " << v_vt[i].u << " "
                 << v_vt[i].v << endl;
  }
  ofs << "# " << i << " texture vertices." << endl;
  ofs << endl;

  // 法線座標の列挙
  // 法線記述がない場合
  if(v_vn.size() == 0)
  {
    uoThreedVector *tmp;                           // 一時バッファ

    // OBJの面数分ループ
    for(i = 0; i < objData.size(); i++)
    {
      // 面の頂点数分ループ
      for(unsigned int j = 0; j < objData[i].getObjDetailDataSize(); j++)
      {
        // 計算済み法線座標取得
        tmp = objData[i].getVertexNormal(j);
        // 法線座標の出力
        ofs << "vn " << tmp->getPointX() << " "
                     << tmp->getPointY() << " "
                     << tmp->getPointZ() << endl;
      }
    }
  }
  // 法線記述がある場合
  else
  {
    // 定義済みの法線数分ループ
    for(i = 0; i < v_vn.size(); i++)
    {
      // 法線座標の出力
      ofs << "vn " << v_vn[i].getPointX() << " "
                  << v_vn[i].getPointY() << " "
                  << v_vn[i].getPointZ() << endl;
    }
  }
  ofs << "# " << i << " normal vertices." << endl;
  ofs << endl;

  string groupNametmp("");                       // グループファイル名一時バッファ
  string mtlNametmp("");                         // MTLファイル名一時バッファ

  // OBJの面数分ループ
  for(i = 0; i < objData.size(); i++)
  {
    // 前回のグループ名と異なる場合
    if(strcmp(groupNametmp.c_str(), objData[i].getGroupName().c_str()) != 0)
    {
      ofs << "g " << objData[i].getGroupName().c_str() << endl;
      groupNametmp = objData[i].getGroupName().c_str();
    }
    // 前回のマテリアル名と異なる場合
    if(strcmp(mtlNametmp.c_str(), objData[i].getMtlName().c_str()) != 0)
    {
      ofs << "usemtl " << objData[i].getMtlName().c_str() << endl;
      mtlNametmp = objData[i].getMtlName().c_str();
    }

    ofs << "f";

    // 面の頂点数分ループ
    for(unsigned int j = 0; j < objData[i].getIndexSize(); j++)
    {
      ofs << " ";
      // 頂点座標インデックスを出力
      if(objData[i].getIndex(j).detail_data[0] != -1)
      {
        ofs << objData[i].getIndex(j).detail_data[0];
      }
      ofs << "/";

      // テクスチャ座標インデックスが存在する場合
      if(objData[i].getIndex(j).detail_data[1] != -1)
      {
        // テクスチャ座標インデックスを出力
        ofs << objData[i].getIndex(j).detail_data[1];
      }

      // 法線座標インデックスが存在する場合
      if(objData[i].getIndex(j).detail_data[2] != -1)
      {
        // 法線座標インデックスを出力
        ofs << "/";
        ofs << objData[i].getIndex(j).detail_data[2];
      }
    }
    ofs << endl;
  }
  ofs << "# " << i << " elements." << endl;
  ofs.close();

  ret = true;

  return ret;
}

int uObjLoader::getMaterialCount()
{
  return cmtl.getCount();
}

int uObjLoader::getArrayCount(int materialID)
{
  if(materialID < 0 || cmtl.getCount() <= materialID)
  {
    return 0;
  }

  string materialName = cmtl.getName(materialID);
  int arrayCount = 0;
  for(unsigned int i = 0; i < objData.size(); i++)
  {
    if(strcmp(objData[i].getMtlName().c_str(), materialName.c_str()) == 0)
    {
      arrayCount += (int)objData[i].getObjDetailDataSize();
    }
  }

  return arrayCount;
}

bool uObjLoader::getArrayInfo(int materialID, float* vertex, float* normal, float* uv)
{
  if(materialID < 0 || cmtl.getCount() <= materialID)
  {
    return false;
  }

  if(vertex == NULL || normal == NULL)
  {
    return false;
  }

  string materialName = cmtl.getName(materialID);
  int arrayIndex = 0;

  for(unsigned int i = 0; i < objData.size(); i++)
  {
    if(strcmp(objData[i].getMtlName().c_str(), materialName.c_str()) != 0)
    {
      continue;
    }

    bool hasNormal = objData[i].getNormalFlg();
    bool hasTexture = objData[i].getTextureFlg();

    for(unsigned int j = 0; j < objData[i].getObjDetailDataSize(); j++)
    {
      uObjDetailData* detail = objData[i].getObjDetailData(j);
      if(detail == NULL)
      {
        continue;
      }

      vertex[arrayIndex * 3 + 0] = (float)detail->getVertexCoordinateX();
      vertex[arrayIndex * 3 + 1] = (float)detail->getVertexCoordinateY();
      vertex[arrayIndex * 3 + 2] = (float)detail->getVertexCoordinateZ();

      if(hasNormal)
      {
        normal[arrayIndex * 3 + 0] = (float)detail->getNormalCoordinateX();
        normal[arrayIndex * 3 + 1] = (float)detail->getNormalCoordinateY();
        normal[arrayIndex * 3 + 2] = (float)detail->getNormalCoordinateZ();
      }
      else
      {
        normal[arrayIndex * 3 + 0] = 0.0f;
        normal[arrayIndex * 3 + 1] = 0.0f;
        normal[arrayIndex * 3 + 2] = 0.0f;
      }

      if(uv != NULL)
      {
        if(hasTexture)
        {
          uv[arrayIndex * 2 + 0] = (float)detail->getTextureCoordinateU();
          uv[arrayIndex * 2 + 1] = (float)detail->getTextureCoordinateV();
        }
        else
        {
          uv[arrayIndex * 2 + 0] = 0.0f;
          uv[arrayIndex * 2 + 1] = 0.0f;
        }
      }

      arrayIndex++;
    }
  }

  return true;
}

bool uObjLoader::getMaterialInfo(int materialID,
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
  return cmtl.getInfo(materialID,
    hasAmbient, ambient,
    hasDiffuse, diffuse,
    hasSpecular, specular,
    hasTransparency, transparency,
    hasShininess, shininess,
    hasIllumination, illumination,
    hasTexture, textureID,
    hasSphereTexture, sphereTextureID,
    hasCubeTexture, cubeTextureIDs);
}

bool uObjLoader::getTextureBufferInfo(int textureID,
  int* width, int* height,
  int* channels, int* bufferSize)
{
  return cmtl.getTextureBufferInfo(textureID, width, height, channels, bufferSize);
}

bool uObjLoader::getTextureBufferImage(int textureID, unsigned char* image, int bufferSize)
{
  return cmtl.getTextureBufferImage(textureID, image, bufferSize);
}
