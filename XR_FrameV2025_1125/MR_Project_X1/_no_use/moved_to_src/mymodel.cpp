#include "platform.h"
#include "GLMetaseq.h"

#include "sim.h"
#include "mymodel.h"

static MQO_MODEL mymodel1;
static MQO_MODEL mymodel2;

//★追加するモデル用の変数を追加する
static MQO_MODEL chicken;

static MQO_MODEL kingyo;
static MQO_MODEL shell;
static MQO_MODEL kumanomi;
static MQO_MODEL kumanomi_02;
static MQO_MODEL grande;
static MQO_MODEL star_take1;
static MQO_MODEL star_take2;
static MQO_MODEL star_take3;
static MQO_MODEL star_take4;
static MQO_MODEL starfish;

static MQO_MODEL handR; //★右手のモデルを追加するとする
static MQO_MODEL handL; //左手も追加する

static GLuint globj_block;
static GLuint globj_stars;

void applyMaterialColor( float r, float g, float b );

void makeRandomBlocks( int n )
{
	globj_block = glGenLists( 1 );
    glNewList(  globj_block, GL_COMPILE );

	for( int i = 0; i<n; i++ ){
		// draw one object
	}

	glEndList();
}

//-------------------------------------------------------------------- makeStars
/* makeStars:
 */
void makeStars( int n, float x_range, float y_range, float z_range )
{
    globj_stars = glGenLists( 1 );
    glNewList( globj_stars, GL_COMPILE );
	glShadeModel( GL_FLAT );
    int i;
    float x, y, z;
    //glColor3f( 1.0, 1.0, 1.0 );
	applyMaterialColor( 0.8, 0.8, 0.8 );
    //glPointSize( 2.0 );
    //glBegin( GL_POINTS );
    for( i = 0; i < n; i++ ){
        x = x_range * (float)rand()/(float)RAND_MAX - x_range / 2.0;
        y = y_range * (float)rand()/(float)RAND_MAX - y_range / 2.0;
        z = z_range * (float)rand()/(float)RAND_MAX - z_range / 2.0;
		glPushMatrix();
		glTranslatef( x, y, z );
		glRotatef( 90.0, 1.0, 0.0, 0.0 );
		glutSolidSphere( 0.1, 8, 5 );
		glPopMatrix();
        //glVertex3f( x, y, z );
    }
    //glEnd();
    glEndList();
    return;
}
void makeStars( int n, float range )
{
	makeStars( n, range, range, range );
}
//-------------------------------------------------------------------- drawStars
/* drawStars: 星を描画する
 */
void drawStars( void )
{
    glPushAttrib( GL_LIST_BIT );
    //glListBase( globj_stars );
    glCallList( globj_stars );
    glPopAttrib();
    return;
}
/////////////////////////// 共通関数
/*----------------------------------------------------- CreateMyModels
 * CreateMyModels: InitSceneで1回だけ呼び出すこと
 */
void CreateMyModels()
{
    //mymodel1 = mqoCreateModel( "mymodel1.mqo", 1.0 );
    //mymodel2 = mqoCreateModel( "mymodel2.mqo", 1.0 );

	makeStars( 2000, 100, 10, 100 );

	//▼モデルファイル読み込み
	//chicken = mqoCreateModel( "data/full_body_chicken.mqo", 0.1 );

	handR = mqoCreateModel( "models/kumanomi.mqo", 0.001 );//★
	handL = mqoCreateModel( "models/background.mqo", 1.0 );
	
	kingyo = mqoCreateModel("models/kingyo.mqo", 0.0001 );
	shell = mqoCreateModel("models/shell.mqo", 0.0001 );
	
	/*
	kumanomi = mqoCreateModel("models/kumanomi.mqo", 0.0001 );
	kumanomi_02 = mqoCreateModel("models/kumanomi_02.mqo", 0.0001 );
	grande = mqoCreateModel("models/grande.mqo", 0.0001 );
	star_take1 = mqoCreateModel("models/star_take1.mqo", 0.001 );
	star_take2 = mqoCreateModel("models/star_take2.mqo", 0.001 );
	star_take3 = mqoCreateModel("models/star_take3.mqo", 0.001 );
	star_take4 = mqoCreateModel("models/star_take4.mqo", 0.001 );
	starfish = mqoCreateModel("models/starfish.mqo", 0.0001 );
	*/
}

/*----------------------------------------------------- DeleteMyModels
 * DeleteMyModels
 */
void DeleteMyModels()
{
	/*
    mqoDeleteModel( mymodel1 );
    mqoDeleteModel( mymodel2 );
	*/
	mqoDeleteModel( kingyo );
	mqoDeleteModel( shell );
}

/////////////////////////// 個別の関数

void drawHandRmodel()
{
    glPushMatrix();
    //glRotatef( 90.0, 1.0, 0.0, 0.0 );//モデルの座標軸調整など
    mqoCallModel( handR );//mqoモデルを描画する
    glPopMatrix();
}

void drawHandLmodel()
{
    glPushMatrix();
    //glRotatef( 90.0, 1.0, 0.0, 0.0 );//モデルの座標軸調整など
    mqoCallModel( handL );
    glPopMatrix();
}

/*------------------------------------------------------ DrawChicken
 */
void drawChicken()
{
    glPushMatrix();
    //glRotatef( 90.0, 1.0, 0.0, 0.0 );//モデルの座標軸調整など
    mqoCallModel( chicken );
    glPopMatrix();
}

/*------------------------------------------------------ DrawMymodel1
 */
void DrawMymodel1()
{
    glPushMatrix();
    glRotatef( 90.0, 1.0, 0.0, 0.0 );//モデルの座標軸調整など
    mqoCallModel( mymodel1 );
    glPopMatrix();
}
/*------------------------------------------------------ DreaMymodel2
 */
void DrawMymodel2()
{
    glPushMatrix();
    glRotatef( 90.0, 1.0, 0.0, 0.0 );//モデルの座標軸調整など
    mqoCallModel( mymodel2 );
    glPopMatrix();
}
/*------------------------------------------------------ Drawkingyo
 */
void Drawkingyo( void )
{
	glPushMatrix();
	glTranslatef(0.0, 0.0, 0.0);
	glRotatef(-90.0, 0.0, 1.0, 0.0);
	glRotatef(0.0, 1.0, 0.0, 0.0);
	glRotatef(0.0, 0.0, 0.0, 1.0);
	mqoCallModel( kingyo );

	glPopMatrix();
}
/*------------------------------------------------------ Drawkingyo
*/
void Drawshell( void )
{
	glPushMatrix();
	glTranslatef(0.0, 0.0, 0.0);
	glRotatef(-90.0, 0.0, 1.0, 0.0);
	glRotatef(0.0, 1.0, 0.0, 0.0);
	glRotatef(0.0, 0.0, 0.0, 1.0);
	mqoCallModel( shell );
	glPopMatrix();
}
/*------------------------------------------------------ Drawkumanomi
*/
void Drawkumanomi( void )
{
	glPushMatrix();
	glTranslatef(0.0, 0.0, 0.0);
	glRotatef(-90.0, 0.0, 1.0, 0.0);
	glRotatef(0.0, 1.0, 0.0, 0.0);
	glRotatef(0.0, 0.0, 0.0, 1.0);
	mqoCallModel( kumanomi );
	glPopMatrix();
}
/*------------------------------------------------------ Drawkumanomi_02
*/
void Drawkumanomi_02( void )
{
	glPushMatrix();

	glTranslatef(0.0, 0.0, 0.0);
	glRotatef(-90.0, 0.0, 1.0, 0.0);
	glRotatef(0.0, 1.0, 0.0, 0.0);
	glRotatef(0.0, 0.0, 0.0, 1.0);
	mqoCallModel( kumanomi_02 );

	glPopMatrix();
}
/*------------------------------------------------------ Drawgrande
*/
void Drawgrande( void )
{
	glPushMatrix();

	glTranslatef(0.0, 0.0, 0.0);
	glRotatef(-90.0, 0.0, 1.0, 0.0);
	glRotatef(0.0, 1.0, 0.0, 0.0);
	glRotatef(0.0, 0.0, 0.0, 1.0);
	mqoCallModel( grande );

	glPopMatrix();
}
/*------------------------------------------------------ Drawstar_take1
*/
void Drawstar_take1( void )
{
	glPushMatrix();

	glTranslatef(0.0, 0.0, 0.0);
	glRotatef(-90.0, 0.0, 1.0, 0.0);
	glRotatef(0.0, 1.0, 0.0, 0.0);
	glRotatef(0.0, 0.0, 0.0, 1.0);
	mqoCallModel( star_take1 );

	glPopMatrix();
}
/*------------------------------------------------------ Drawstar_take2
*/
void Drawstar_take2( void )
{
	glPushMatrix();

	glTranslatef(0.0, 0.0, 0.0);
	glRotatef(-90.0, 0.0, 1.0, 0.0);
	glRotatef(0.0, 1.0, 0.0, 0.0);
	glRotatef(0.0, 0.0, 0.0, 1.0);
	mqoCallModel( star_take2 );

	glPopMatrix();
}
/*------------------------------------------------------ Drawstar_take3
*/
void Drawstar_take3( void )
{
	glPushMatrix();

	glTranslatef(0.0, 0.0, 0.0);
	glRotatef(-90.0, 0.0, 1.0, 0.0);
	glRotatef(0.0, 1.0, 0.0, 0.0);
	glRotatef(0.0, 0.0, 0.0, 1.0);
	mqoCallModel( star_take3 );

	glPopMatrix();
}
/*------------------------------------------------------ Drawstar_take4
*/
void Drawstar_take4( void )
{
	glPushMatrix();

	glTranslatef(0.0, 0.0, 0.0);
	glRotatef(-90.0, 0.0, 1.0, 0.0);
	glRotatef(0.0, 1.0, 0.0, 0.0);
	glRotatef(0.0, 0.0, 0.0, 1.0);
	mqoCallModel( star_take4 );

	glPopMatrix();
}
/*------------------------------------------------------ Drawstarfish
*/
void Drawstarfish( void )
{
	glPushMatrix();

	glTranslatef(0.0, 0.0, 0.0);
	glRotatef(-90.0, 0.0, 1.0, 0.0);
	glRotatef(0.0, 1.0, 0.0, 0.0);
	glRotatef(0.0, 0.0, 0.0, 1.0);
	mqoCallModel( starfish );

	glPopMatrix();
}