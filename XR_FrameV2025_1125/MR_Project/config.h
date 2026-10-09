#pragma once

//MREALのとき使う
//MREALではないときコメントにする
#define MREALMODE

//表示画面
const bool fullscreen = false; //◆フルスクリーンフラグ
//※ステレオモードのときには強制的にフルスクリーンになります
const bool no_cursor = false;  //◆カーソル非表示フラグ
//フルスクリーン状態のときカーソルは強制的に非表示になります

//HMDモード
const bool hmd = false; //◆HMDモードフラグ
const bool distortion = true; //◆HMD歪補正フラグ

//＊HMDモードがオフのとき下記設定が機能します
const bool cylindrical = false; //◆シリンドリカルフラグ
const bool stereo = false;    //◆ステレオフラグ
//※ステレオモードのときには強制的にフルスクリーンになります
const bool swapeyes = false; //◆左右画像の入れ替え 
const float parallax = 0.0625; //◆左右瞳間距離
const bool quadbuffer = false; //◆液晶シャッタメガネ対応
//※グラフィックスカードにはNVIDIA Quadroが必要です
//対応していないPC環境では、プログラムが落ちます

//トラッキングデバイス
const bool use_tracker = false; //◆トラッカーフラグ
const bool use_gyro = false; //◆ジャイロフラグ

//外部映像入力（usbビデオデバイス）
const bool usb_video = false; //◆USBビデオデバイス
