#include "pch.h"
#include "UnityObjDetailData.h"

using namespace objloader;

//-----------------------------------------------------------------//
//  デフォルトコンストラクタ                                       //
//-----------------------------------------------------------------//
uObjDetailData::uObjDetailData(void)
{
//  textureU = NULL;
//  textureV = NULL;
  st_texture.u = 0;
  st_texture.v = 0;
}

//-----------------------------------------------------------------//
//  デストラクタ                                                   //
//-----------------------------------------------------------------//
uObjDetailData::~uObjDetailData(void)
{
}

//-----------------------------------------------------------------//
// 頂点座標値の取得(配列)を行います。                              //
//-----------------------------------------------------------------//
double *uObjDetailData::getVertexCoordinates(void)
{
  return vertexCoordinate.getPointAll();
}

//-----------------------------------------------------------------//
// 頂点X座標値の取得を行います。                                   //
//-----------------------------------------------------------------//
double uObjDetailData::getVertexCoordinateX(void)
{
  return vertexCoordinate[0];
}

//-----------------------------------------------------------------//
// 頂点Y座標値の取得を行います。                                   //
//-----------------------------------------------------------------//
double uObjDetailData::getVertexCoordinateY(void)
{
  return vertexCoordinate[1];
}

//-----------------------------------------------------------------//
// 頂点Z座標値の取得を行います。                                   //
//-----------------------------------------------------------------//
double uObjDetailData::getVertexCoordinateZ(void)
{
  return vertexCoordinate[2];
}

//-----------------------------------------------------------------//
// テクスチャU座標値の取得を行います。                             //
//-----------------------------------------------------------------//
double uObjDetailData::getTextureCoordinateU(void)
{
//  return textureU;
  return st_texture.u;
}

//-----------------------------------------------------------------//
// テクスチャV座標値の取得を行います。                             //
//-----------------------------------------------------------------//
double uObjDetailData::getTextureCoordinateV(void)
{
//  return textureV;
  return st_texture.v;
}

//-----------------------------------------------------------------//
// 法線座標値の取得(配列)を行います。                              //
//-----------------------------------------------------------------//
double *uObjDetailData::getNormalCoordinates(void)
{
  return normalCoordinate.getPointAll();
}

//-----------------------------------------------------------------//
// 法線X座標値の取得を行います。                                   //
//-----------------------------------------------------------------//
double uObjDetailData::getNormalCoordinateX(void)
{
  return normalCoordinate[0];
}

//-----------------------------------------------------------------//
// 法線Y座標値の取得を行います。                                   //
//-----------------------------------------------------------------//
double uObjDetailData::getNormalCoordinateY(void)
{
  return normalCoordinate[1];
}

//-----------------------------------------------------------------//
// 法線Z座標値の取得を行います。                                   //
//-----------------------------------------------------------------//
double uObjDetailData::getNormalCoordinateZ(void)
{
  return normalCoordinate[2];
}

//-----------------------------------------------------------------//
// 頂点座標値の設定(配列指定)を行います。                          //
//-----------------------------------------------------------------//
void uObjDetailData::setVertexCoordinate(const double *pval)
{
  vertexCoordinate.setPointAll(pval);
}

//-----------------------------------------------------------------//
// 頂点座標値の設定(個別指定)を行います。                          //
//-----------------------------------------------------------------//
void uObjDetailData::setVertexCoordinate(double val_x
                                       , double val_y
                                       , double val_z)
{
  vertexCoordinate.setPointX(val_x);
  vertexCoordinate.setPointY(val_y);
  vertexCoordinate.setPointZ(val_z);
}

//-----------------------------------------------------------------//
// 頂点座標値の設定(3Dベクトルクラスオブジェクト指定)を行います。  //
//-----------------------------------------------------------------//
void uObjDetailData::setVertexCoordinate(const uoThreedVector *pval)
{
  vertexCoordinate = *pval;
}

//-----------------------------------------------------------------//
// テクスチャ座標値の設定(配列指定)を行います。                    //
//-----------------------------------------------------------------//
void uObjDetailData::setTextureCoordinate(const double *pval)
{
//  textureU = *pval;
//  textureV = *(pval + 1);
  st_texture.u = *pval;
  st_texture.v = *(pval + 1);
}

//-----------------------------------------------------------------//
// テクスチャ座標値の設定(個別指定)を行います。                    //
//-----------------------------------------------------------------//
void uObjDetailData::setTextureCoordinate(double val_u, double val_v)
{
//  textureU = val_u;
//  textureV = val_v;
  st_texture.u = val_u;
  st_texture.v = val_v;
}

//-----------------------------------------------------------------//
// テクスチャU座標値の設定を行います。                             //
//-----------------------------------------------------------------//
void uObjDetailData::setTextureCoordinateU(double val)
{
//  textureU = val;
  st_texture.u = val;
}

//-----------------------------------------------------------------//
// テクスチャV座標値の設定を行います。                             //
//-----------------------------------------------------------------//
void uObjDetailData::setTextureCoordinateV(double val)
{
//  textureV = val;
  st_texture.v = val;
}

//-----------------------------------------------------------------//
// 法線座標値の設定(配列指定)を行います。                          //
//-----------------------------------------------------------------//
void uObjDetailData::setNormalCoordinate(const double *pval)
{
  uoThreedVector temp((double *)pval);
  normalCoordinate = temp.getNormalizeALL();
}

//-----------------------------------------------------------------//
// 法線座標値の設定(個別指定)を行います。                          //
//-----------------------------------------------------------------//
void uObjDetailData::setNormalCoordinate(double val_x
                                       , double val_y
                                       , double val_z)
{
  uoThreedVector temp(val_x, val_y, val_z);
  normalCoordinate = temp.getNormalizeALL();
}

//-----------------------------------------------------------------//
// 法線座標値の設定(3Dベクトルクラスオブジェクト)を行います。      //
//-----------------------------------------------------------------//
void uObjDetailData::setNormalCoordinate(const uoThreedVector *pval)
{
  uoThreedVector temp(*pval);
  normalCoordinate = temp.getNormalizeALL();
}
