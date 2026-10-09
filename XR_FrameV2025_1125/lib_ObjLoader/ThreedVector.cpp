#include ".\threedvector.h"

//-----------------------------------------------------------------//
//  デフォルトコンストラクタ                                       //
//-----------------------------------------------------------------//
CThreedVector::CThreedVector(void)
{
  for(int i=0; i < 3; i++)
  {
    this->fvector[i] = 0.0;
  }
}

//-----------------------------------------------------------------//
//  コンストラクタ(座標値配列指定)                                 //
//-----------------------------------------------------------------//
CThreedVector::CThreedVector(double *pval)
{
  for(int i=0; i < 3; i++)
  {
    this->fvector[i] = pval[i];
  }
}

//-----------------------------------------------------------------//
//  コンストラクタ(座標値個別指定)                                 //
//-----------------------------------------------------------------//
CThreedVector::CThreedVector(double val1, double val2, double val3)
{
  this->fvector[0] = val1;
  this->fvector[1] = val2;
  this->fvector[2] = val3;
}

//-----------------------------------------------------------------//
//  コピーコンストラクタ                                           //
//-----------------------------------------------------------------//
CThreedVector::CThreedVector(const CThreedVector &val)
{
  fvector[0] = val.fvector[0];
  fvector[1] = val.fvector[1];
  fvector[2] = val.fvector[2];
}

//-----------------------------------------------------------------//
//  デストラクタ                                                   //
//-----------------------------------------------------------------//
CThreedVector::~CThreedVector(void)
{
}

//-----------------------------------------------------------------//
//  X座標値設定                                                    //
//-----------------------------------------------------------------//
void CThreedVector::setPointX(double val)
{
  this->fvector[0] = val;
}

//-----------------------------------------------------------------//
//  Y座標値設定                                                    //
//-----------------------------------------------------------------//
void CThreedVector::setPointY(double val)
{
  this->fvector[1] = val;
}

//-----------------------------------------------------------------//
//  Z座標値設定                                                    //
//-----------------------------------------------------------------//
void CThreedVector::setPointZ(double val)
{
  this->fvector[2] = val;
}

//-----------------------------------------------------------------//
//  ベクトル座標値設定                                             //
//-----------------------------------------------------------------//
void CThreedVector::setPointAll(const double *pval)
{
  for(int i=0; i < 3; i++)
  {
    this->fvector[i] = pval[i];
  }
}

//-----------------------------------------------------------------//
//  X座標値取得                                                    //
//-----------------------------------------------------------------//
double CThreedVector::getPointX(void)
{
  return this->fvector[0];
}

//-----------------------------------------------------------------//
//  Y座標値取得                                                    //
//-----------------------------------------------------------------//
double CThreedVector::getPointY(void)
{
  return this->fvector[1];
}

//-----------------------------------------------------------------//
//  Z座標値取得                                                    //
//-----------------------------------------------------------------//
double CThreedVector::getPointZ(void)
{
  return this->fvector[2];
}

//-----------------------------------------------------------------//
//  ベクトル座標値取得                                             //
//-----------------------------------------------------------------//
double *CThreedVector::getPointAll(void)
{
  return this->fvector;
}

//-----------------------------------------------------------------//
//  単位ベクトルX座標値取得                                        //
//-----------------------------------------------------------------//
double CThreedVector::getNormalizeX(void)
{
  culcNormalVector();      // 単位ベクトルを計算
  return this->fnormalvector[0];
}

//-----------------------------------------------------------------//
//  単位ベクトルY座標値取得                                        //
//-----------------------------------------------------------------//
double CThreedVector::getNormalizeY(void)
{
  culcNormalVector();      // 単位ベクトルを計算
  return this->fnormalvector[1];
}

//-----------------------------------------------------------------//
//  単位ベクトルZ座標値取得                                        //
//-----------------------------------------------------------------//
double CThreedVector::getNormalizeZ(void)
{
  culcNormalVector();      // 単位ベクトルを計算
  return this->fnormalvector[2];
}

//-----------------------------------------------------------------//
//  単位ベクトル座標値取得                                         //
//-----------------------------------------------------------------//
double *CThreedVector::getNormalizeALL(void)
{
  culcNormalVector();      // 単位ベクトルを計算
  return this->fnormalvector;
}

//-----------------------------------------------------------------//
//  ベクトルの長さ取得                                             //
//-----------------------------------------------------------------//
double CThreedVector::getLength(void)
{
  culcLength();      // ベクトルの長さを計算
  return this->fvectorlength;
}

//-----------------------------------------------------------------//
//  ベクトルの加算                                                 //
//-----------------------------------------------------------------//
CThreedVector CThreedVector::operator+(CThreedVector &val)
{
  CThreedVector temp;    // 一時オブジェクト

  temp.setPointX(this->getPointX() + val.getPointX());
  temp.setPointY(this->getPointY() + val.getPointY());
  temp.setPointZ(this->getPointZ() + val.getPointZ());

  return temp;
}

//-----------------------------------------------------------------//
//  ベクトルの減算                                                 //
//-----------------------------------------------------------------//
CThreedVector CThreedVector::operator-(CThreedVector &val)
{
  CThreedVector temp;    // 一時オブジェクト

  temp.setPointX(this->getPointX() - val.getPointX());
  temp.setPointY(this->getPointY() - val.getPointY());
  temp.setPointZ(this->getPointZ() - val.getPointZ());

  return temp;
}

//-----------------------------------------------------------------//
//  スカラーの乗算                                                 //
//-----------------------------------------------------------------//
CThreedVector CThreedVector::operator*(double val)
{
  CThreedVector temp;    // 一時オブジェクト

  temp.setPointX(this->getPointX() * val);
  temp.setPointY(this->getPointY() * val);
  temp.setPointZ(this->getPointZ() * val);

  return temp;
}

//-----------------------------------------------------------------//
//  スカラーの除算                                                 //
//-----------------------------------------------------------------//
CThreedVector CThreedVector::operator/(double val)
{
  CThreedVector temp;    // 一時オブジェクト

  // 0除算対応
  if(val == 0.0f)
  {
    cout << "スカラー量の減算に0が指定されました。" << endl;
    return NULL;
  }

  temp.setPointX(this->getPointX() / val);
  temp.setPointY(this->getPointY() / val);
  temp.setPointZ(this->getPointZ() / val);

  return temp;
}

//-----------------------------------------------------------------//
//  ベクトルの代入                                                 //
//-----------------------------------------------------------------//
CThreedVector &CThreedVector::operator=(const CThreedVector &val)
{
  this->fvector[0] = val.fvector[0];
  this->fvector[1] = val.fvector[1];
  this->fvector[2] = val.fvector[2];

  return *this;
}

//-----------------------------------------------------------------//
//  ベクトルの比較(==)                                             //
//-----------------------------------------------------------------//
bool CThreedVector::operator==(CThreedVector &val)
{
  return (this->fvector[0] == val.fvector[0]
      && this->fvector[1] == val.fvector[1]
      && this->fvector[2] == val.fvector[2]);
}

//-----------------------------------------------------------------//
//  ベクトルの比較(!=)                                             //
//-----------------------------------------------------------------//
bool CThreedVector::operator!=(CThreedVector &val)
{
  return (this->fvector[0] != val.fvector[0]
      && this->fvector[1] != val.fvector[1]
      && this->fvector[2] != val.fvector[2]);
}

//-----------------------------------------------------------------//
//  行列要素の取得                                                 //
//-----------------------------------------------------------------//
double & CThreedVector::operator[](int id)
{
  // 0より小さく、15より大きい場合はエラー
  if(id > 4 || 0 > id)
  {
    cout << "引数の指定が不正(0より小さい、又は4より大きい)です。" << endl;
    exit(1);
  }

  return this->fvector[id];
}

//-----------------------------------------------------------------//
//  内積取得                                                       //
//-----------------------------------------------------------------//
double CThreedVector::getDotProduct(CThreedVector &val)
{
  return this->getPointX() * val.getPointX()
      + this->getPointY() * val.getPointY()
      + this->getPointZ() * val.getPointZ();
}

//-----------------------------------------------------------------//
//  外積取得                                                       //
//-----------------------------------------------------------------//
CThreedVector CThreedVector::getCrossProduct(CThreedVector &val)
{
  CThreedVector temp;    // 一時オブジェクト
  
  temp.setPointX((this->getPointY() * val.getPointZ()) - (this->getPointZ() * val.getPointY()));
  temp.setPointY((this->getPointZ() * val.getPointX()) - (this->getPointX() * val.getPointZ()));
  temp.setPointZ((this->getPointX() * val.getPointY()) - (this->getPointY() * val.getPointX()));

  return temp;
}

//-----------------------------------------------------------------//
//  角度取得                                                       //
//-----------------------------------------------------------------//
double CThreedVector::getAngle(CThreedVector &val)
{
  double angle;      // 角度

  // acos(内積 / |A|*|B|)
  angle = acos(this->getDotProduct(val) / (this->fvectorlength * val.fvectorlength));
  // ラジアンから角度に変換
  angle = (angle * 180.0f) / (double)M_PI;

  return angle;
}

//-----------------------------------------------------------------//
//  ベクトルの長さを計算                                           //
//-----------------------------------------------------------------//
void CThreedVector::culcLength(void)
{
  this->fvectorlength = sqrt(pow(this->fvector[0], 2)
                + pow(this->fvector[1], 2)
                + pow(this->fvector[2], 2));
}

//-----------------------------------------------------------------//
//  単位ベクトルを計算                                             //
//-----------------------------------------------------------------//
void CThreedVector::culcNormalVector(void)
{
  culcLength();

  for(int i = 0; i < 3; i++)
  {
    // ベクトルの長さが0である場合
    if(this->fvectorlength == 0)
    {
      this->fnormalvector[i] = 0;
    }
    else
    {
      this->fnormalvector[i] = this->fvector[i] / this->fvectorlength;
    }
  }
}

//-----------------------------------------------------------------//
//  内部データ出力                                                 //
//-----------------------------------------------------------------//
void CThreedVector::show(ostream &ost)
{
  ost << "=======================================" << endl;
  ost << "＜＜基本情報＞＞" << endl;
  ost << "ベクトル X:" << this->getPointX() << endl;
  ost << "ベクトル Y:" << this->getPointY() << endl;
  ost << "ベクトル Z:" << this->getPointZ() << endl;
  ost << "ベクトルの長さ:" << this->getLength() << endl;
  ost << "単位ベクトル X:" << this->getNormalizeX() << endl;
  ost << "単位ベクトル Y:" << this->getNormalizeY() << endl;
  ost << "単位ベクトル Z:" << this->getNormalizeZ() << endl;
}
