//#pragma once
#ifndef EZMIDI_H
#define EZMIDI_H
#include <windows.h>
//#include <mmeapi.h> //// 20220925追加
#include <mmsystem.h> //// 20221114追加
#include <stdio.h>

/*
No.HEX

//ドラムセット割り当て
35　Acoustic Bass Drum	
38　Acoustic Snare
41　Low Floor Tom
45　Low Tom
47　Low Mid Tom
44　Pedal Hi-Hat
49　Crash Cymbal 1
51　Ride Cymbal 1

// #01 PIANO

1 00 Acoustic Piano アコースティックピアノ 
2 01 Bright Piano ブライトピアノ 
3 02 Electric Grand Piano エレクトリックグランドピアノ 
4 03 Honky-tonk Piano ホンキートンクピアノ 
5 04 Electric Piano エレクトリックピアノ 
6 05 Electric Piano 2 エレクトリックピアノ2 
7 06 Harpsichord ハープシコード 
8 07 Clavi クラビネット 

// #02 Chromatic Percussion

9 08 Celesta チェレスタ 
10 09 Glockenspiel グロッケンシュピール 
11 0A Musical box オルゴール 
12 0B Vibraphone ヴィブラフォン 
13 0C Marimba マリンバ 
14 0D Xylophone シロフォン 
15 0E Tubular Bell チューブラーベル 
16 0F Dulcimer ダルシマー 

// #03 Organ

17 10 Drawbar Organ ドローバーオルガン 
18 11 Percussive Organ パーカッシブオルガン 
19 12 Rock Organ ロックオルガン 
20 13 Church organ チャーチオルガン 
21 14 Reed organ リードオルガン 
22 15 Accordion アコーディオン 
23 16 Harmonica ハーモニカ 
24 17 Tango Accordion タンゴアコーディオン 

// #04 Guitar

25 18 Acoustic Guitar (nylon) アコースティックギター（ナイロン弦） 
26 19 Acoustic Guitar (steel) アコースティックギター（スチール弦） 
27 1A Electric Guitar (jazz) ジャズギター 
28 1B Electric Guitar (clean) クリーンギター 
29 1C Electric Guitar (muted) ミュートギター 
30 1D Overdriven Guitar オーバードライブギター 
31 1E Distortion Guitar ディストーションギター 
32 1F Guitar harmonics ギターハーモニクス 

// #05 Bass

33 20 Acoustic Bass アコースティックベース 
34 21 Electric Bass (finger) フィンガー・ベース 
35 22 Electric Bass (pick) ピック・ベース 
36 23 Fretless Bass フレットレスベース 
37 24 Slap Bass 1 スラップベース 1 
38 25 Slap Bass 2 スラップベース 2 
39 26 Synth Bass 1 シンセベース 1 
40 27 Synth Bass 2 シンセベース 2 

// #06 Strings

41 28 Violin ヴァイオリン 
42 29 Viola ヴィオラ 
43 2A Cello チェロ 
44 2B Double bass コントラバス 
45 2C Tremolo Strings トレモロ 
46 2D Pizzicato Strings ピッチカート 
47 2E Orchestral Harp ハープ 
48 2F Timpani ティンパニ 

// #07 Ensemble

49 30 String Ensemble 1 ストリングアンサンブル 1 
50 31 String Ensemble 2 ストリングアンサンブル 2 
51 32 Synth Strings 1 シンセストリングス 1 
52 33 Synth Strings 2 シンセストリングス 2 
53 34 Voice Aahs 声「あー」 
54 35 Voice Oohs 声「おー」 
55 36 Synth Voice シンセヴォイス 
56 37 Orchestra Hit オーケストラヒット 

// #08 Brass

57 38 Trumpet トランペット 
58 39 Trombone トロンボーン 
59 3A Tuba チューバ 
60 3B Muted Trumpet ミュートトランペット 
61 3C French horn フレンチ・ホルン 
62 3D Brass Section ブラスセクション 
63 3E Synth Brass 1 シンセブラス 1 
64 3F Synth Brass 2 シンセブラス 2 

// #09 Reed

65 40 Soprano Sax ソプラノサックス 
66 41 Alto Sax アルトサックス 
67 42 Tenor Sax テナーサックス 
68 43 Baritone Sax バリトンサックス 
69 44 Oboe オーボエ 
70 45 English Horn イングリッシュホルン 
71 46 Bassoon ファゴット 
72 47 Clarinet クラリネット 

// #10 Pipe

73 48 Piccolo ピッコロ 
74 49 Flute フルート 
75 4A Recorder リコーダー 
76 4B Pan Flute パンフルート 
77 4C Blown Bottle 茶瓶 
78 4D Shakuhachi 尺八 
79 4E Whistle 口笛 
80 4F Ocarina オカリナ 

// #11 Synth Lead

81 50 Lead 1 (square) 矩形波 
82 51 Lead 2 (sawtooth) ノコギリ波 
83 52 Lead 3 (calliope) カリオペ 
84 53 Lead 4 (chiff) チフ 
85 54 Lead 5 (charang) チャランゴ 
86 55 Lead 6 (voice) 声 
87 56 Lead 7 (fifths) フィフスズ 
88 57 Lead 8 (bass + lead) バス + リード 

// #12 Synth Pad

89 58 Pad 1 (Fantasia) ファンタジア 
90 59 Pad 2 (warm) ウォーム 
91 5A Pad 3 (polysynth) ポリシンセ 
92 5B Pad 4 (choir) クワイア 
93 5C Pad 5 (bowed) ボウ 
94 5D Pad 6 (metallic) メタリック 
95 5E Pad 7 (halo) ハロー 
96 5F Pad 8 (sweep) スウィープ 

// #13 Synth Effects

97 60 FX 1 (rain) 雨 
98 61 FX 2 (soundtrack) サウンドトラック 
99 62 FX 3 (crystal) クリスタル 
100 63 FX 4 (atmosphere) アトモスフィア 
101 64 FX 5 (brightness) ブライトネス 
102 65 FX 6 (goblins) ゴブリン 
103 66 FX 7 (echoes) エコー 
104 67 FX 8 (sci-fi) サイファイ 

// #14 Ethnic

105 68 Sitar シタール 
106 69 Banjo バンジョー 
107 6A Shamisen 三味線 
108 6B Koto 琴 
109 6C Kalimba カリンバ 
110 6D Bagpipe バグパイプ 
111 6E Fiddle フィドル 
112 6F Shanai シャハナーイ 

// #15 Percussive

113 70 Tinkle Bell ティンクルベル 
114 71 Agogo アゴゴ 
115 72 Steel Drums スチールドラム 
116 73 Woodblock ウッドブロック 
117 74 Taiko Drum 太鼓 
118 75 Melodic Tom メロディックタム 
119 76 Synth Drum シンセドラム 
120 77 Reverse Cymbal 逆シンバル 

// #16 Sound effects

121 78 Guitar Fret Noise ギターフレットノイズ 
122 79 Breath Noise ブレスノイズ 
123 7A Seashore 海岸 
124 7B Bird Tweet 鳥の囀り 
125 7C Telephone Ring 電話のベル 
126 7D Helicopter ヘリコプター 
127 7E Applause 拍手 
128 7F Gunshot 銃声 
*/
/*-----------------------------------------------------------------*/
class ezMIDI{
  public:
  /*----------------*/
  ezMIDI( int ch = 0, int tone = 0x60 ){
    _channel = ch;
	_tone = tone;
    _volume = 0x60;
	Send( 0xc0, _tone, 0x00 );
  }
  ~ezMIDI(){};

  /*----------------*/
  /* midiデバイスオープン*/
  static int Open( bool poly = false ){
    int ret;
    ret = midiOutOpen( &_hMidi, MIDIMAPPER, 0, 0, 0 );
    if( ret != 0 ) printf("error to open midi\n");
	polyphonic = poly;

    return ret;
  }
  /*----------------*/
  /* midiデバイスクローズ*/
  static int Close(){
    int ret;
    ret = midiOutClose( _hMidi);
    if( ret != 0 ) printf("error to close midi\n");
    return ret;
  }
  /*----------------*/
  /* midiデバイスの修了 */
  static int Reset(){
	  int ret;
	  ret = midiOutReset( _hMidi );
      if( ret != 0 ) printf("error to close midi\n");
	  return ret;
  }
  /*----------------*/
  /* 音を鳴らす　midiデバイスにmidiコードを転送 */
  int Send( int stat, int data1, int data2 ){
    midiOutShortMsg( _hMidi, _PackMIDI( stat, data1 ,data2 ));
    return 0;
  }
  /*----------------*/
  /*----------------*/
  void Play( int pitch, int volume = 0x60 )
  {
      if( !polyphonic ){
	      Send( 0x90, _pitch, 0x00 );
	  }
	  else{

	  }
      Send( 0x90, pitch, 0x00 );
	  if( volume == 0 ) Send( 0x90, pitch, 0 );
	  else Send( 0x90, pitch, _volume );
	  _pitch = pitch;
  }
  int Mute( void )
  {
  }

  /*----------------*/
  void SetVolume( int vol ){
    _volume = vol;
  }
  static HMIDIOUT _hMidi;/* midiデバイスハンドラ*/
  static bool polyphonic;



  private:
  int _channel;
  int _tone;
  int _pitch;
  int _volume;
  int _panpot;

  /*----------------*/
  DWORD _PackMIDI( int stat, int data1, int data2 ){
    return (DWORD)( stat|_channel |(data1 << 8)|(data2 << 16));
  }
};
#endif
