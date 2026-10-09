#include ".\objdata.h"

using namespace objloader;

//-----------------------------------------------------------------//
// デフォルトコンストラクタ                                        //
//-----------------------------------------------------------------//
CObjData::CObjData(void)
{
}

//-----------------------------------------------------------------//
//  デストラクタ                                                   //
//-----------------------------------------------------------------//
CObjData::~CObjData(void)
{
}

//-----------------------------------------------------------------//
// グループNoを取得します。                                        //
//-----------------------------------------------------------------//
int CObjData::getGroupNo(void)
{
  return groupNo;
}

//-----------------------------------------------------------------//
// グループ名を取得します。                                        //
//-----------------------------------------------------------------//
string CObjData::getGroupName(void)
{
  return groupName;
}

//-----------------------------------------------------------------//
// マテリアル定義名を取得します。                                  //
//-----------------------------------------------------------------//
string CObjData::getMtlName(void)
{
  return mtlName;
}

//-----------------------------------------------------------------//
// 面法線を取得取得します。                                        //
//-----------------------------------------------------------------//
CThreedVector *CObjData::getFaceNormal(void)
{
  return &faceNormal;
}

//-----------------------------------------------------------------//
// 法線フラグを取得します。                                        //
//-----------------------------------------------------------------//
bool CObjData::getNormalFlg(void)
{
  return normalFlg;
}

//-----------------------------------------------------------------//
// テクスチャフラグを取得します。                                  //
//-----------------------------------------------------------------//
bool CObjData::getTextureFlg(void)
{
  return textureFlg;
}

//-----------------------------------------------------------------//
// 頂点法線を取得します。                                          //
//-----------------------------------------------------------------//
CThreedVector *CObjData::getVertexNormal(unsigned int val)
{
  // 引数が、0以上かつ、vectorサイズ未満である場合
  if(0 <= val && val < vertexNormal.size())
  {
    return &vertexNormal[val];
  }
  // 上記以外の場合
  else
  {
    return NULL;
  }
}

//-----------------------------------------------------------------//
// OBJ詳細データを取得します。                                     //
//-----------------------------------------------------------------//
CObjDetailData *CObjData::getObjDetailData(unsigned int val)
{
  // 引数が、0以上かつ、vectorサイズ未満である場合
  if(0 <= val && val < objDetailData.size())
  {
    return &objDetailData[val];
  }
  // 上記以外の場合
  else
  {
    return NULL;
  }
}

//-----------------------------------------------------------------//
// グループNoを設定します。                                        //
//-----------------------------------------------------------------//
void CObjData::setGroupNo(int val)
{
  groupNo = val;
}

//-----------------------------------------------------------------//
// グループ名を設定します。                                        //
//-----------------------------------------------------------------//
void CObjData::setGroupName(string val)
{
  groupName = val;
}

//-----------------------------------------------------------------//
// マテリアル定義名を設定します。                                  //
//-----------------------------------------------------------------//
void CObjData::setMtlName(string val)
{
  mtlName = val;
}

//-----------------------------------------------------------------//
// 面法線を設定します。                                            //
//-----------------------------------------------------------------//
void CObjData::setFaceNormal(const CThreedVector *pval)
{
  faceNormal = *pval;
}

//-----------------------------------------------------------------//
// 法線フラグを設定します。                                        //
//-----------------------------------------------------------------//
void CObjData::setNormalFlg(bool val)
{
  normalFlg = val;
}

//-----------------------------------------------------------------//
// テクスチャフラグを設定します。                                  //
//-----------------------------------------------------------------//
void CObjData::setTextureFlg(bool val)
{
  textureFlg = val;
}

//-----------------------------------------------------------------//
// 法線座標インデックスを設定します。                              //
//-----------------------------------------------------------------//
void CObjData::setNormalIndex(int arrayindex, long index)
{
  v_index[arrayindex].detail_data[2] = index;
}

//-----------------------------------------------------------------//
// 頂点法線を設定します。                                          //
//-----------------------------------------------------------------//
void CObjData::addVertexNormal(const CThreedVector &pval)
{
  vertexNormal.push_back(pval);
}

//-----------------------------------------------------------------//
// OBJ詳細データを追加します。                                     //
//-----------------------------------------------------------------//
void CObjData::addObjDetailData(const CObjDetailData &pval)
{
  objDetailData.push_back(pval);
}

//-----------------------------------------------------------------//
// OBJインデックスデータを追加します。                             //
//-----------------------------------------------------------------//
void CObjData::addIndex(const St_vf_detail_data & pval)
{
  v_index.push_back(pval);
}

//-----------------------------------------------------------------//
// OBJデータの初期化を行います。                                   //
//-----------------------------------------------------------------//
void CObjData::clear(void)
{
  groupNo = 0;
  groupName = "";
  mtlName = "";
  normalFlg = false;
  textureFlg = false;
  vertexNormal.clear();
  objDetailData.clear();
  v_index.clear();
}

//-----------------------------------------------------------------//
// OBJ詳細データ数を取得します。                                   //
//-----------------------------------------------------------------//
unsigned int CObjData::getObjDetailDataSize(void)
{
  return (unsigned int)objDetailData.size();
}

//-----------------------------------------------------------------//
// OBJインデックスデータ数を取得します。                           //
//-----------------------------------------------------------------//
unsigned int CObjData::getIndexSize(void)
{
  return (unsigned int)v_index.size();
}

//-----------------------------------------------------------------//
// OBJインデックスデータを取得します。                             //
//-----------------------------------------------------------------//
St_vf_detail_data CObjData::getIndex(int index)
{
  St_vf_detail_data tmp;
  tmp.detail_data[0] = -1;
  tmp.detail_data[1] = -1;
  tmp.detail_data[2] = -1;

  // インデックス指定が範囲外である場合
  if(index < 0 || (int)v_index.size() <= index)
  {
    return tmp;
  }

  return v_index[index];
}

//-----------------------------------------------------------------//
// 内部データを出力します。                                        //
//-----------------------------------------------------------------//
void CObjData::show(ostream &os)
{
  os << "==============================================" << endl;
  os << "groupNo:" << groupNo << endl;

  os << "groupName:" << groupName.c_str() << endl;

  os << "mtlName:" << mtlName.c_str() << endl;

  os << "faceNormal: x = " << faceNormal.getPointX()
        << ", y = " << faceNormal.getPointY()
        << ", z = " << faceNormal.getPointZ() << endl;

  os << "normalFlg:" << normalFlg << endl;

  os << "textureFlg:" << textureFlg << endl;

  os << "----------------------------------------------" << endl;
  for(unsigned int i = 0; i < vertexNormal.size(); i++)
  {
    os << "vertexNormal[" << i << "]: x = " << vertexNormal[i].getPointX()
          << ", y = " << vertexNormal[i].getPointY()
          << ", z = " << vertexNormal[i].getPointZ() << endl;
  }

  os << "----------------------------------------------" << endl;
  for(unsigned int i = 0; i < objDetailData.size(); i++)
  {
    os << "objDetailData[" << i << "] Vertex: x = " << objDetailData[i].getVertexCoordinateX()
                      << ", y = " << objDetailData[i].getVertexCoordinateY()
                      << ", z = " << objDetailData[i].getVertexCoordinateZ() << endl;

    os << "objDetailData[" << i << "] Normal: x = " << objDetailData[i].getNormalCoordinateX()
                      << ", y = " << objDetailData[i].getNormalCoordinateY()
                      << ", z = " << objDetailData[i].getNormalCoordinateZ() << endl;

    os << "objDetailData[" << i << "] texture: U = " << objDetailData[i].getTextureCoordinateU()
                      << ", V = " << objDetailData[i].getTextureCoordinateV() << endl;
  }
}
