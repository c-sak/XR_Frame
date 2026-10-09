#include ".\material.h"

//-----------------------------------------------------------------//
// デフォルトコンストラクタ                                        //
//-----------------------------------------------------------------//
CMaterial::CMaterial(void)
{
}

//-----------------------------------------------------------------//
// デストラクタ                                                    //
//-----------------------------------------------------------------//
CMaterial::~CMaterial(void)
{
}

//-----------------------------------------------------------------//
// Diffuse(拡散光)設定を行います。                                 //
//-----------------------------------------------------------------//
void CMaterial::setDiffuse(int val, GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
  diffuse[0] = red;
  diffuse[1] = green;
  diffuse[2] = blue;
  diffuse[3] = alpha;

  glMaterialfv(val, GL_DIFFUSE, diffuse);
}

//-----------------------------------------------------------------//
// Ambient(環境光)設定を行います。                                 //
//-----------------------------------------------------------------//
void CMaterial::setAmbient(int val, GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
  ambient[0] = red;
  ambient[1] = green;
  ambient[2] = blue;
  ambient[3] = alpha;

  //glColor3d(red, green, blue);
  glMaterialfv(val, GL_AMBIENT, ambient);
}

//-----------------------------------------------------------------//
// Specular(鏡面光)設定を行います。                                //
//-----------------------------------------------------------------//
void CMaterial::setSpecular(int val, GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
  specular[0] = red;
  specular[1] = green;
  specular[2] = blue;
  specular[3] = alpha;

  glMaterialfv(val, GL_SPECULAR, specular);
}

//-----------------------------------------------------------------//
// Emission(放射光)設定を行います。                                //
//-----------------------------------------------------------------//
void CMaterial::setEmission(int val, GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
  emission[0] = red;
  emission[1] = green;
  emission[2] = blue;
  emission[3] = alpha;

  glMaterialfv(val, GL_EMISSION, emission);
}

//-----------------------------------------------------------------//
// Shininess(鏡面指数)設定を行います。                             //
//-----------------------------------------------------------------//
void CMaterial::setShininess(int val1, GLfloat val2)
{
  glMaterialf(val1, GL_SHININESS, val2);
}

//-----------------------------------------------------------------//
// 内部データの初期化を行います。                                  //
//-----------------------------------------------------------------//
void CMaterial::clear(void)
{
  // DIFFUSE設定値
  diffuse[0] = GL_FRONT;
  diffuse[1] = 0.8f;
  diffuse[2] = 0.8f;
  diffuse[3] = 0.8f;
  diffuse[4] = 1.0f;
  setDiffuse((int)diffuse[0], diffuse[1], diffuse[2], diffuse[3], diffuse[4]);

  // AMBIENT設定値
  ambient[0] = GL_FRONT;
  ambient[1] = 0.2f;
  ambient[2] = 0.2f;
  ambient[3] = 0.2f;
  ambient[4] = 1.0f;
  setAmbient((int)ambient[0], ambient[1], ambient[2], ambient[3], ambient[4]);
  
  // SPECULAR設定値
  specular[0] = GL_FRONT;
  specular[1] = 0.0f;
  specular[2] = 0.0f;  
  specular[3] = 0.0f;
  specular[4] = 1.0f;
  setSpecular((int)specular[0], specular[1], specular[2], specular[3], specular[4]);

  // EMISSION設定値
  emission[0] = GL_FRONT;
  emission[1] = 0.0f;
  emission[2] = 0.0f;
  emission[3] = 0.0f;
  emission[4] = 1.0f;
  setEmission((int)emission[0], emission[1], emission[2], emission[3], emission[4]);

  // SHAININESS設定値
  shininess[0] = GL_FRONT;
  shininess[1] = 0.0f;
  setShininess((int)shininess[0], shininess[1]);

}
