#include "pch.h"
#include "UnityObjData.h"

using namespace objloader;

//-----------------------------------------------------------------//
// デフォルトコンストラクタ                                        //
//-----------------------------------------------------------------//
uObjData::uObjData(void)
{
}

//-----------------------------------------------------------------//
//  デストラクタ                                                   //
//-----------------------------------------------------------------//
uObjData::~uObjData(void)
{
}

//-----------------------------------------------------------------//
// グループNoを取得します。                                        //
//-----------------------------------------------------------------//
int uObjData::getGroupNo(void)
{
  return groupNo;
}

//-----------------------------------------------------------------//
// グループ名を取得します。                                        //
//-----------------------------------------------------------------//
string uObjData::getGroupName(void)
{
  return groupName;
}

//-----------------------------------------------------------------//
// マテリアル定義名を取得します。                                  //
//-----------------------------------------------------------------//
string uObjData::getMtlName(void)
{
  return mtlName;
}

//-----------------------------------------------------------------//
// 面法線を取得取得します。                                        //
//-----------------------------------------------------------------//
uoThreedVector *uObjData::getFaceNormal(void)
{
  return &faceNormal;
}

//-----------------------------------------------------------------//
// 法線フラグを取得します。                                        //
//-----------------------------------------------------------------//
bool uObjData::getNormalFlg(void)
{
  return normalFlg;
}

//-----------------------------------------------------------------//
// テクスチャフラグを取得します。                                  //
//-----------------------------------------------------------------//
bool uObjData::getTextureFlg(void)
{
  return textureFlg;
}

//-----------------------------------------------------------------//
// 頂点法線を取得します。                                          //
//-----------------------------------------------------------------//
uoThreedVector *uObjData::getVertexNormal(unsigned int val)
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
uObjDetailData *uObjData::getObjDetailData(unsigned int val)
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
void uObjData::setGroupNo(int val)
{
  groupNo = val;
}

//-----------------------------------------------------------------//
// グループ名を設定します。                                        //
//-----------------------------------------------------------------//
void uObjData::setGroupName(string val)
{
  groupName = val;
}

//-----------------------------------------------------------------//
// マテリアル定義名を設定します。                                  //
//-----------------------------------------------------------------//
void uObjData::setMtlName(string val)
{
  mtlName = val;
}

//-----------------------------------------------------------------//
// 面法線を設定します。                                            //
//-----------------------------------------------------------------//
void uObjData::setFaceNormal(const uoThreedVector *pval)
{
  faceNormal = *pval;
}

//-----------------------------------------------------------------//
// 法線フラグを設定します。                                        //
//-----------------------------------------------------------------//
void uObjData::setNormalFlg(bool val)
{
  normalFlg = val;
}

//-----------------------------------------------------------------//
// テクスチャフラグを設定します。                                  //
//-----------------------------------------------------------------//
void uObjData::setTextureFlg(bool val)
{
  textureFlg = val;
}

//-----------------------------------------------------------------//
// 法線座標インデックスを設定します。                              //
//-----------------------------------------------------------------//
void uObjData::setNormalIndex(int arrayindex, long index)
{
  v_index[arrayindex].detail_data[2] = index;
}

//-----------------------------------------------------------------//
// 頂点法線を設定します。                                          //
//-----------------------------------------------------------------//
void uObjData::addVertexNormal(const uoThreedVector &pval)
{
  vertexNormal.push_back(pval);
}

//-----------------------------------------------------------------//
// OBJ詳細データを追加します。                                     //
//-----------------------------------------------------------------//
void uObjData::addObjDetailData(const uObjDetailData &pval)
{
  objDetailData.push_back(pval);
}

//-----------------------------------------------------------------//
// OBJインデックスデータを追加します。                             //
//-----------------------------------------------------------------//
void uObjData::addIndex(const St_uo_vf_detail_data & pval)
{
  v_index.push_back(pval);
}

//-----------------------------------------------------------------//
// OBJデータの初期化を行います。                                   //
//-----------------------------------------------------------------//
void uObjData::clear(void)
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
unsigned int uObjData::getObjDetailDataSize(void)
{
  return (unsigned int)objDetailData.size();
}

//-----------------------------------------------------------------//
// OBJインデックスデータ数を取得します。                           //
//-----------------------------------------------------------------//
unsigned int uObjData::getIndexSize(void)
{
  return (unsigned int)v_index.size();
}

//-----------------------------------------------------------------//
// OBJインデックスデータを取得します。                             //
//-----------------------------------------------------------------//
St_uo_vf_detail_data uObjData::getIndex(int index)
{
  St_uo_vf_detail_data tmp;
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
void uObjData::show(ostream &os)
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
