#include "platform.h"

#include "ezUtil.h"
#include "sim.h"

#include "mymodel.h" //★

extern SimDataT simdata;

//-------- prototypes for user functions

void applyMaterialColor( float r, float g, float b );
void drawString(float x, float y, float z, float xscl, float yscl,
	const char *string);

/**/
/*---------------------------------------------------------------------------*/
void drawString(float x, float y, float z, float xscl, float yscl,
	const char *string)
{
	bool lighting = glIsEnabled(GL_LIGHTING);
	if (lighting) glDisable(GL_LIGHTING);

	glPushMatrix();
	glTranslatef(x, y, z);
	glScalef(xscl*0.001, yscl*0.001, 1.0);
	while (*string) {
		glutStrokeCharacter(GLUT_STROKE_ROMAN, *string++);
	}
	glPopMatrix();
	if( lighting ) glEnable(GL_LIGHTING);
	return;
}
/*--------------------------------------------------------- setMaterialColor
 * applyMaterialColor:
 *--------*/
void applyMaterialColor( float r, float g, float b )
{
    float diffuse[4];
    float specular[] = { 0.8, 0.8, 0.8, 1.0 };

	diffuse[0] = r;
	diffuse[1] = g;
	diffuse[2] = b;
	diffuse[3] = 1.0;

    glMaterialfv( GL_FRONT, GL_DIFFUSE, diffuse );
    glMaterialfv( GL_FRONT, GL_SPECULAR, specular );
    glMaterialf( GL_FRONT, GL_SHININESS, 32.0 );

    return;
}
/*----------------------------------------------------------- drawSolidCube
 * drawSolidCube:
 *--------*/
void drawSolidCube( void )
{
	glPushMatrix();
	{
		glTranslatef( 0.0, 0.0, 0.0 );   //オブジェクト基準位置調整
		glRotatef( 0.0, 0.0, 1.0, 0.0 ); //オブジェクト基準姿勢調整：ヨー角
		glRotatef( 0.0, 1.0, 0.0, 0.0 ); //オブジェクト基準姿勢調整：ピッチ角
		glRotatef( 0.0, 0.0, 0.0, 1.0 ); //オブジェクト基準姿勢調整：ロール角
		glutSolidCube( 1.0 );
	}
	glPopMatrix();
    return;
}
/*--------------------------------------------------------- drawSolidSphere
 * drawSolidSphere:
 *--------*/
void drawSolidSphere( void )
{
	glPushMatrix();
	{
		glTranslatef( 0.0, 0.0, 0.0 );    //オブジェクト基準位置調整
		glRotatef( 0.0, 0.0, 1.0, 0.0 );  //オブジェクト基準姿勢調整：ヨー角
		glRotatef( 90.0, 1.0, 0.0, 0.0 ); //オブジェクト基準姿勢調整：ピッチ角
		glRotatef( 0.0, 0.0, 0.0, 1.0 );  //オブジェクト基準姿勢調整：ロール角
		glutSolidSphere( 0.5, 18, 16 );   //半径，経度方向分割数，緯度方向分割数
	}
	glPopMatrix();
    return;
}
//----------------------------------------------- drawPlayer
void drawPlayer( void )
{
	glPushMatrix();
	{
		glTranslatef( 0.0, 0.0, 0.0 );    //オブジェクト基準位置調整
		glRotatef( 0.0, 0.0, 1.0, 0.0 );  //オブジェクト基準姿勢調整：ヨー角
		glRotatef( 180.0, 1.0, 0.0, 0.0 ); //オブジェクト基準姿勢調整：ピッチ角
		glRotatef( 0.0, 0.0, 0.0, 1.0 );  //オブジェクト基準姿勢調整：ロール角
		glutSolidCone( 0.5, 1.0, 6, 4 );   //半径，経度方向分割数，緯度方向分割数
	}
	glPopMatrix();
}

void drawHandR( void )
{
	glPushMatrix();
	applyObjColor( &simdata.handR );
	applyObjTransform( &simdata.handR );
	//glutSolidSphere( 0.1, 8, 6 );
	drawHandRmodel();//mymodels.cppで作った描画関数
	glPopMatrix();
}
void drawHandL( void )
{
	glPushMatrix();
	applyObjColor( &simdata.handL );
	applyObjTransform( &simdata.handL );
	glutSolidSphere( 0.1, 8, 6 );
	//drawHandLmodel();
	glPopMatrix();
}
/*---------------------------------------------------------------- PreDraw
 * PreDraw:
 *--------*/
void PreDraw(void)
{
}
/*--------------------------------------------------------------- PostDraw
* PostDraw: 3Dのシーンに重ねて2Dの情報を表示する関数
*--------*/
void PostDraw(void)
{
	//文字を描画するコード一式など
	//ライティングなし
	//2Dの情報画面
	//2Dの投影変換を設定する（並行投影）
	glMatrixMode(GL_PROJECTION);//投影変換マトリクス編集モード
	glLoadIdentity();//初期化
	glOrtho( 0.0, 1.0, 0.0, 1.0, -1.0, 1.0); //画面全体を0.0～1.0の範囲に設定
	glMatrixMode(GL_MODELVIEW);//モデリング変換マトリクスモード
	glLoadIdentity();//初期化

	//お好きに描画
	char time_message[32];
	sprintf(time_message, "time = %d", simdata.time );
	glColor3f( 1.0, 1.0, 1.0 );
	drawString(0.1, 0.1, 0.0, 1.0, 1.0, time_message );

}
/**/
/*---------------------------------------------------------------- DrawScene
 * DrawScene:
 *--------*/
void DrawScene( void )
{
	for( int i = 0; i < N_TARGET; i++ ){

		if( !simdata.TargetList[i].detected ) continue;

		glPushMatrix();
		{
			glTranslatef( simdata.TargetList[i].pos.x,
				simdata.TargetList[i].pos.y,
				simdata.TargetList[i].pos.z );

			glRotatef( simdata.TargetList[i].ori.angle,
				simdata.TargetList[i].ori.x,
				simdata.TargetList[i].ori.y,
				simdata.TargetList[i].ori.z );
				
			switch( i ){/////ターゲット用
			case 0://////////PageOne200-202//target:1
				applyMaterialColor(1.0, 0.0, 1.0);
				glutSolidCube(0.1);
				break;
			case 1://////////PageTwo203-205//target:2
				applyMaterialColor(0.0, 1.0, 0.0);
				glutSolidCube(0.1);

				break;
			case 2:
				applyMaterialColor(0.0, 0.0, 1.0);
				glutSolidCube(0.1);

				break;
			case 3:
				applyMaterialColor(1.0, 1.0, 1.0);
				glutSolidCube(0.1);

				break;
			case 4:
				
				break;
			case 5:
				
				break;
			case 6:
				break;
			case 7:
				
				break;
			case 8:
				
				break;
			case 9:

				break;
			case 10:

				break;
			case 11:

				break;
			case 12:

				break;
			case 13:

				break;
			case 14:

				break;
			default:
				break;

			}
		}
		glPopMatrix();
	}
}