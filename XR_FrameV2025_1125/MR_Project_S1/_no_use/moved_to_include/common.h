#pragma once
//-------- common window parameters
typedef struct {
    int xo;
    int yo;
    int width;
    int height;
    float aspect;
	char *title;
} WindowDataT;

//-------- common mouse data
typedef struct {
	float xo, yo; // click position
	float xp, yp; // previous position
    float x,  y;  // current position
	float dx, dy; // movement
	float xAbs, yAbs; // absolute position
	float xRel, yRel; // relative position
	bool left;    // left button
	bool middle;  // middle button
 	bool right;   // right button - false:UP, true:DOWN
} MouseDataT;

//-------- common keyboard data
typedef struct {
	float x, y;
	bool charKey[128];//alpha-numeric printable ascii characters
	bool arrowLeft, arrowUp, arrowRight, arrowDown;
    bool funcKey[12]; //0:F1 - 11:F12
	bool pageUp, pageDown;
	bool home, end;
	bool insert;
} KeyDataT;

//-------- config data
typedef struct{
	//表示画面
	bool fullscreen; //◆フルスクリーンフラグ
	//※ステレオモードのときには強制的にフルスクリーンになります
	bool no_cursor;  //◆カーソル消去フラグ
	//※カーソル消去はフルスクリーンのときのみ設定できます

	//HMDモード
	bool hmd; //◆HMDモードフラグ
	bool distortion; //◆HMD歪補正フラグ

	//＊HMDモードがオフのとき下記設定が機能します
	bool cylindrical; //◆シリンドリカルフラグ
	bool stereo;    //◆ステレオフラグ
	//※ステレオモードのときには強制的にフルスクリーンになります
	bool swapeyes; //◆左右画像の入れ替え 
	float parallax; //◆左右瞳間距離
	bool quadbuffer; //◆液晶シャッタメガネ対応
	//※グラフィックスカードにはNVIDIA Quadroが必要です
	//対応していないPC環境では、プログラムが落ちます

	//トラッキングデバイス
	bool use_tracker; //◆トラッカーフラグ
	bool use_gyro; //◆ジャイロフラグ
} ConfigDataT;

class ezConfig: public ConfigDataT{
public:
	ezConfig(){

		//表示画面
		fullscreen = true; //◆フルスクリーンフラグ
		//※ステレオモードのときには強制的にフルスクリーンになります
		no_cursor = false;  //◆カーソル消去フラグ
		//※カーソル消去はフルスクリーンのときのみ設定できます

		//HMDモード
		hmd = false; //◆HMDモードフラグ
		distortion = true; //◆HMD歪補正フラグ

		//＊HMDモードがオフのとき下記設定が機能します
		cylindrical = false; //◆シリンドリカルフラグ
		stereo = true;    //◆ステレオフラグ
		//※ステレオモードのときには強制的にフルスクリーンになります
		swapeyes = false; //◆左右画像の入れ替え 
		parallax = 0.0625; //◆左右瞳間距離
		quadbuffer = false; //◆液晶シャッタメガネ対応
		//※グラフィックスカードにはNVIDIA Quadroが必要です
		//対応していないPC環境では、プログラムが落ちます

		//トラッキングデバイス
		use_tracker = false; //◆トラッカーフラグ
		use_gyro = true; //◆ジャイロフラグ
	}
	~ezConfig(){
	};
private:

};
