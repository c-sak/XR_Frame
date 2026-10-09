#include ".\objloader.h"

using namespace objloader;

//-----------------------------------------------------------------//
// デフォルトコンストラクタ                                        //
//-----------------------------------------------------------------//
CObjLoader::CObjLoader(void)
{
  debug_cnt = 0;
}

//-----------------------------------------------------------------//
// デストラクタ                                                    //
//-----------------------------------------------------------------//
CObjLoader::~CObjLoader(void)
{
}

//-----------------------------------------------------------------//
// OBJファイルを読込みます。                                       //
//-----------------------------------------------------------------//
bool CObjLoader::read(string objfnm)
{
  isMakeDispList = true;                         // ディスプレイリスト作成あり

  return (readTrans(objfnm));                    // OBJファイル読込み
}

//-----------------------------------------------------------------//
// read()で読込み済みのOBJモデルを描画します。                     //
//-----------------------------------------------------------------//
void CObjLoader::draw(double enlarge)
{
  glPushMatrix();
  {
    // 拡大比率が1.0倍以外である場合
    if(enlarge != 1.0)
    {
      // 拡大比率でモデルの大きさを調整
      glScaled(enlarge, enlarge, enlarge);
    }
    // ディスプレイリストを呼び出し
    glCallList(displaylistNo);
  }
  glPopMatrix();
}

//-----------------------------------------------------------------//
// read()で読込み済みのOBJモデルの法線を描画します。               //
//-----------------------------------------------------------------//
void CObjLoader::drawNormal(double enlarge)
{
  glPushMatrix();
  {
    // 拡大比率が1.0倍以外である場合
    if(enlarge != 1.0)
    {
      glScaled(enlarge, enlarge, enlarge);   // モデルの拡大
    }
    // 法線描画用ディスプレイリストを呼び出し
    glCallList(lineDisplaylistNo);
  }
  glPopMatrix();
}

//-----------------------------------------------------------------//
// read()で読込み済みのOBJモデルをライン描画します。               //
//-----------------------------------------------------------------//
void CObjLoader::drawLine(double enlarge)
{
  glDisable(GL_LIGHTING);                        // ライティングOFF
  {
    glPolygonMode(GL_FRONT, GL_LINE);            // ライン描画設定ON
    {
      glPushMatrix();
      {
        // 拡大比率が1.0倍以外である場合
        if(enlarge != 1.0)
        {
          glScaled(enlarge, enlarge, enlarge);   // モデルの拡大
        }
        glCallList(displaylistNo);               // 法線描画用ディスプレイリストを呼び出し
      }
      glPopMatrix();
    }
    glPolygonMode(GL_FRONT, GL_FILL);            // ライン描画設定OFF
  }
  glEnable(GL_LIGHTING);                         // ライティングON
}

//-----------------------------------------------------------------//
// 法線なしのOBJモデルを、法線を付加してOBJファイルに出力します。  //
//-----------------------------------------------------------------//
bool CObjLoader::write(string inFileNm, string outFileNm)
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
// モデル描画のディスプレイリストを作成します。                    //
//-----------------------------------------------------------------//
void CObjLoader::makeDisplayList(bool isDelete)
{
  float colors_line[] = {0.8f, 0.8f, 0.0f};      // 法線色
  float colorCl[] = {1.0, 1.0, 1.0, 1.0};        // 初期値
  bool mtlFlg = false;
  bool drawFlg = false;

  pre_MaterialName = "";                         // 前回のマテリアル名を初期化

  displaylistNo = glGenLists(1);                 // ディスプレイリストNO取得

  // ディスプレイリストへ登録
  glNewList(displaylistNo, GL_COMPILE);
  {
    // 描画定義のレコード数分ループ
    for(unsigned int i = 0; i < objData.size(); i++)
    {
      draw_face(objData[i]);                     // 面の描画
    }
    cmtl.clearMtl();                             // OBJ用マテリアル設定の初期化
  }
  glEndList();


  lineDisplaylistNo = glGenLists(1);             // ディスプレイリストNO取得

  // 法線描画用ディスプレイリストへ登録
  glNewList(lineDisplaylistNo, GL_COMPILE);
  {
    glDisable(GL_LIGHTING);                      // ライティングOFF
    {
      glColor3fv(colors_line);                   // 法線描画色を設定

      glPushMatrix();
      {
        // 法線のライン描画
        glBegin(GL_LINES);
        {
          // 描画定義のレコード数分ループ
          for(unsigned int i = 0; i < objData.size(); i++)
          {
            draw_normal(objData[i]);                 // 法線の描画
          }
        }
        glEnd();
      }
      glPopMatrix();

      glColor3fv(colorCl);                       // マテリアル設定初期化
    }
    glEnable(GL_LIGHTING);                       // ライティングOFF
  }
  glEndList();

  // メモリ領域開放ありの場合
  if(isDelete)
  {
    this->clear();                               // メモリ領域開放
  }
}

//-----------------------------------------------------------------//
//  オブジェクトの面の描画(1つの面データを描画)を行います。        //
//-----------------------------------------------------------------//
void CObjLoader::draw_face(CObjData &val)
{
  //int tempCnt;                                   // カウンタ一時保持用
  vector <int> v_index;                          // 頂点描画インデックス

  v_index.push_back(0);                          // 1つ目の頂点インデックス
  v_index.push_back(1);                          // 2つ目の頂点インデックス
  v_index.push_back(2);                          // 3つ目の頂点インデックス

  // 頂点数が3つである場合
  if(val.getObjDetailDataSize() == 3)
  {
    ;
  }
  // 頂点数が4つである場合
  else if(val.getObjDetailDataSize() == 4)
  {
    // f指定でパラメータが4つである場合
    // 頂点の処理順序は「0 ⇒ 1 ⇒ 3 ⇒ 2」となる
    v_index[2] = 3;
    v_index.push_back(2);
  }
  // 上記以外
  else
  {
    v_index.clear();                             // インデックスのクリア
  }

  // マテリアル定義名が設定されている場合
  if(val.getMtlName().length() > 0
    && pre_MaterialName.compare(val.getMtlName()) != 0)
  {
    // OBJ用マテリアル定義の初期化
    cmtl.clearMtl();

    // OBJ用マテリアル定義の適用
    cmtl.applyMtl(val.getMtlName());

    // マテリアル名履歴に設定
    pre_MaterialName = val.getMtlName().c_str();
  }

  // 三角形ストリップ描画
  glBegin(GL_TRIANGLE_STRIP);
  {
  // 頂点の個数分ループ
  for(unsigned int i = 0; i < v_index.size(); i++)
  {
    // 法線座標の設定値が存在する場合
    if(val.getNormalFlg())
    {
      // 法線座標の設定
      glNormal3dv(val.getObjDetailData(v_index[i])->getNormalCoordinates());
    }
    // 法線座標の設定値が存在しない場合
    else
    {
      // 頂点法線の計算
      culcVertexNormal(val, v_index[i]);
    }
    
    // テクスチャ設定値が存在する場合
    if(val.getTextureFlg())
    {
      // テクスチャ座標の設定
      glTexCoord2f(val.getObjDetailData(v_index[i])->getTextureCoordinateU()
            , val.getObjDetailData(v_index[i])->getTextureCoordinateV());
    }
    // テクスチャ設定値が存在しない場合
    else
    {
      // スキップ
      ;
    }
    // 頂点座標の設定
    glVertex3dv(val.getObjDetailData(v_index[i])->getVertexCoordinates());
  }
  }
  glEnd();
}

//-----------------------------------------------------------------//
// オブジェクトの面の法線描画(1つの面データを描画)を行います。     //
//-----------------------------------------------------------------//
void CObjLoader::draw_normal(CObjData &val)
{
  CThreedVector stPoint, endPoint;               // 始点、終点

    // 頂点の個数分ループ
    for(unsigned int i = 0; i < val.getObjDetailDataSize(); i++)
    {
                                                 // 頂点座標を取得
      stPoint.setPointAll(val.getObjDetailData(i)->getVertexCoordinates());

      glVertex3dv(stPoint.getPointAll());        // 頂点座標の描画

      // 面法線座標の設定値が存在する場合
      if(val.getNormalFlg())
      {
                                                 // 面法線を取得
        endPoint.setPointAll(val.getObjDetailData(i)->getNormalCoordinates());

        endPoint = stPoint + endPoint;           // 頂点座標＋面法線
        glVertex3dv(endPoint.getPointAll());     // 法線座標の描画
      }
      // 法線座標の設定値が存在しない場合
      else
      {
        // 頂点法線を取得
        endPoint.setPointAll(val.getVertexNormal(i)->getNormalizeALL());

        endPoint = stPoint + endPoint;           // 頂点座標＋頂点法線
        glVertex3dv(endPoint.getPointAll());     // 頂点法線座標の描画
      }
    }
}

//-----------------------------------------------------------------//
//  面法線の計算を行います。                                       //
//-----------------------------------------------------------------//
void CObjLoader::culcFaceNormal(CObjData &val)
{
  vector < CThreedVector > vArrayNo;             // 面法線リスト
  CThreedVector ctv_1, ctv_2, temp;              // 汎用ベクトル

  // 法線情報が既に存在する場合、処理を行わない
  if(val.getNormalFlg())
  {
    return;                                      // 呼び出し元に戻す
  }

  // 頂点の数だけループ
  for(unsigned int i = 0; i < val.getObjDetailDataSize(); i++)
  {
    // ベクトルの取得(objに設定してある法線情報を取得)
    vArrayNo.push_back(val.getObjDetailData(i)->getVertexCoordinates());
  }
  // 複数分割が必要でも、面法線としては同じ方向なので、頂点1～3を使用
  ctv_1 = vArrayNo[1] - vArrayNo[0];
  ctv_2 = vArrayNo[2] - vArrayNo[0];

  temp = ctv_1.getCrossProduct(ctv_2);           // 外積を取得
  val.setFaceNormal(&temp);                      // 外積を面法線に設定
}

//-----------------------------------------------------------------//
//  頂点法線の計算を行います。                                     //
//-----------------------------------------------------------------//
void CObjLoader::culcVertexNormal(CObjData &val, int cntval)
{
  vector <CThreedVector> faceNormal;             // 隣接面法線バッファ
  CThreedVector ctv_1                            // 一時ベクトル領域
              , ctv_2                            // 一時ベクトル領域
              , ctv_faceNormal                   // 面法線ベクトル
              , ctv_vertexNormal;                // 頂点法線ベクトル

  // 計算対象のベクトル座標をコピー
  ctv_2.setPointAll(val.getObjDetailData(cntval)->getVertexCoordinates());

  // 自身の面法線を追加
  faceNormal.push_back(*val.getFaceNormal());

  // 総面数分ループ
  for(unsigned int i = 0; i < objData.size(); i++)
  {
    // 同じグループである場合
    if(!strcmp(val.getGroupName().c_str(), objData[i].getGroupName().c_str()))
    {
      // 1つの面の総頂点数分ループ
      for(unsigned int j = 0; j < objData[i].getObjDetailDataSize(); j++)
      {
        ctv_1.setPointAll(objData[i].getObjDetailData(j)->getVertexCoordinates());

        // 頂点座標が同じである場合
        if(ctv_1 == ctv_2)
        {
          // 面法線の値が同じでない場合
          if(objData[i].getFaceNormal() != val.getFaceNormal())
          {

            // 面法線を追加
            faceNormal.push_back(*objData[i].getFaceNormal());
            break;
          }
        }
        // 頂点座標が異なる場合
        else
        {
          // スキップ
          ;
        }
      }
    }
  }

  // 隣接した面法線分ループ
  for(unsigned int i = 0; i < faceNormal.size(); i++)
  {
    // 面法線を加算
    ctv_faceNormal = ctv_faceNormal + faceNormal[i];
  }

  // 単位ベクトル化(頂点法線)
  CThreedVector ctv_dot(ctv_faceNormal.getNormalizeALL());

  // スムージング角との比較により、面法線・頂点法線のどちらを使用するか判定
  // 内積計算(自身の法線と隣接する面の法線)
  double dot = val.getFaceNormal()->getDotProduct(ctv_dot);

  // スムージング角度のラジアン
  double angle = cos(SMOOTHING_ANGLE * M_PI / 180.0f);

  // スムージング角と同じ、または大きい場合
  if(dot >= angle)
  {
    // 頂点法線を設定
    glNormal3dv(ctv_faceNormal.getNormalizeALL());

    // 頂点法線を保存
    val.addVertexNormal(ctv_faceNormal);
  }
  // スムージング角より小さい場合
  else if(dot < angle)
  {
    // 面法線を設定
    glNormal3dv(val.getFaceNormal()->getPointAll());
    val.addVertexNormal(*val.getFaceNormal());
  }
  // 法線座標用インデックスを設定
  val.setNormalIndex(cntval, vf_cnt);

  // 法線座標用インデックスを+1
  vf_cnt++;
}

//-----------------------------------------------------------------//
//  内部データ初期化処理を行います。                               //
//-----------------------------------------------------------------//
void CObjLoader::clear(void)
{
  debug_cnt = 0;                                 // デバッグ用カウンタ
  vf_cnt = 1;                                    // 頂点法線用インデックスカウンタ

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
bool CObjLoader::readTrans(string objfnm)
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

  CThreedVector vectorCtv;                       // ベクトル一時領域
  CThreedVector normalCtv;                       // 法線ベクトル一時領域

  St_TextureCoordinate textureCoordinate_temp;   // テクスチャ座標一時領域

  int groupNoCnt = 0;                            // グループNOカウンタ

  CObjData objData_temp;                         // 一時OBJデータ
  CObjDetailData objDetailData_temp;             // 一時OBJ詳細データ

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
        CThreedVector Ctvtmp(normalCtv.getNormalizeALL());
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
      St_vf_detail_data vf_data;                 // インデックス管理用一時領域

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
          CThreedVector temp(v_vn[vf_data.detail_data[2] - 1].getPointAll());

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
      culcFaceNormal(objData_temp);              // 面法線の計算

      objData.push_back(objData_temp);           // OBJ基本情報に追加
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

  makeDisplayList(isMakeDispList);               // ディスプレイリストへ登録

  ret = true;

  return ret;
}

//-----------------------------------------------------------------//
// read()で読込み済みのOBJモデルをOBJファイルに出力します。        //
//-----------------------------------------------------------------//
bool CObjLoader::writeTrans(string filenm)
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
    CThreedVector *tmp;                           // 一時バッファ

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
