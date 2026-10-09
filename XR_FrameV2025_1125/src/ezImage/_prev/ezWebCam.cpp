//#include "ez/platform.h"
#include <stdio.h>

#include "ezWebCam.h"

#include "ewclib/ewclib.h"
//簡易カメラ入力ライブラリEWCLIBのインクルード

//=============================================================================
// EWCLIBをラッピングしewclib.hを隠ぺいする
//-----------------------------------------------------------------------------
int ezWebCam_Open( int cam, int wx, int wy, double fps, int device, int mstype )
{
	GUID guidmstype;
	int status;
	if( mstype == -1 ) guidmstype = MEDIASUBTYPE_RGB24; 
	else guidmstype = EWC_TYPE;
	status = EWC_Open( cam, wx, wy, fps, device, guidmstype );
	if( status != 0 ){
		printf("[ezWebCam] EWC_Open Error 0\n");
	}
	EWC_PropertyPage( cam ); // カメラプロパティを開く
	printf("[ezWebCam] BufferSize: %d\n", EWC_GetBufferSize( cam )); 
	// 必要なバッファサイズの取得

	return status;
}
//-----------------------------------------------------------------------------
int ezWebCam_Close( int cam )
{
	return EWC_Close( cam );
}
//-----------------------------------------------------------------------------
void ezWebCam_GetImage( int cam, unsigned char *buffer )
{
	EWC_GetImage( cam, buffer );
	return;
}

