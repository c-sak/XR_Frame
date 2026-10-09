#pragma once

//MREALのプロジェクトで使う
//MREALではないときコメントにする
//#define MREALMODE

const bool debug = true;//◆デバッグフラグ
//★debug = true のときには、
// 以下のフラグ設定にかかわらず、
// 立体視・壁スクリーン・VICONをフルセットで活用します。

//表示画面
const bool fullscreen = false; //◆フルスクリーンフラグ
//※ステレオモードのときには強制的にフルスクリーンになります
const bool no_cursor = false;  //◆カーソル非表示フラグ
//フルスクリーン状態のときカーソルは強制的に非表示になります

//HMDモード
const bool hmd = false; //◆HMDモードフラグ
const bool distortion = true; //◆HMD歪補正フラグ
//// hmdモードで何某かバグあり20210819

//＊HMDモードがオフのとき下記設定が機能します
const bool cylindrical = true; //◆シリンドリカルフラグ
const bool stereo = false;    //◆ステレオフラグ
//※ステレオモードのときには強制的にフルスクリーンになります
const bool swapeyes = false; //◆左右画像の入れ替え 
const float parallax = 0.0625; //◆左右瞳間距離
const bool quadbuffer = false; //◆液晶シャッタメガネ対応
//※グラフィックスカードにはNVIDIA Quadro/RTXが必要です
//対応していないPC環境では、プログラムが落ちます

//トラッキングデバイス
const bool use_tracker = false; //◆トラッカーフラグ
const bool use_vicon = true; //◆true:VICON, false:ARマーカー

//#define ZIGSIM
//#define WITMOTION

