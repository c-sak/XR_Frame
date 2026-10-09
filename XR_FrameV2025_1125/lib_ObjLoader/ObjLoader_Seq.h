#pragma once
#include "ObjLoader.h"

#include <vector>
#include <sstream>
#include <chrono>

using namespace std::chrono;

namespace objloader
{
	class CObjLoader_Seq
	{
	/*--------------*/
	/* 公開メンバ   */
	/*--------------*/
	public:
		//*******************************************************************
		/*!
		*  @brief  デフォルトコンストラクタ
		*  @date  2020/03/10 ... 新規作成
		*/
		//********************************************************************
		CObjLoader_Seq();

		//*******************************************************************
		/*!
		*  @brief  デストラクタ
		*  @date  2020/03/10 ... 新規作成
		*/
		//********************************************************************
		~CObjLoader_Seq();

		//*******************************************************************
		/*!
		*  @brief  引数であるOBJファイル名とフレーム数を使用して、OBJの連番ファイルを読込みます。
		*  @param  objfnm ... [In] OBJファイル名、連番部分は書式指定文字(%i)を入力
		*  @param  framenum ... [In] フレーム数
		*  @param  framerate ... [In] フレームレート
		*  @retval  true  ... 正常終了
		*  @retval  false ... 異常終了
		*  @date  2020/03/10 ... 新規作成
		*/
		//********************************************************************
		bool read_sequence(string objfnm, int framenum, double framerate);

		//*******************************************************************
		/*!
		*  @brief  read_sequence()で読込み済みのOBJ連番ファイルを描画します。
		*  @attention  事前にread_sequence()を呼び出している必要があります。
		*  @param  enlarge ... [In] 拡大比率
		*  @date  2020/03/10 ... 新規作成
		*/
		//********************************************************************
		void draw_sequence(double enlarge);

		//*******************************************************************
		/*!
		*  @brief  read_sequence()で読込み済みのOBJ連番ファイルをライン描画します。
		*  @attention  事前にread_sequence()を呼び出している必要があります。
		*  @param  enlarge ... [In] 拡大比率
		*  @date  2020/03/10 ... 新規作成
		*/
		//********************************************************************
		void drawLine_sequence(double enlarge);

		//*******************************************************************
		/*!
		*  @brief  read_sequence()で読込み済みのOBJ連番ファイルの法線を描画します。
		*  @attention  事前にread_sequence()を呼び出している必要があります。
		*  @param  enlarge ... [In] 拡大比率
		*  @date  2020/03/10 ... 新規作成
		*/
		//********************************************************************
		void drawNormal_sequence(double enlarge);

	/*--------------*/
	/* 非公開メンバ */
	/*--------------*/
	private:
		int frameCounter;
		vector <CObjLoader> v_objSeq;

		system_clock::time_point prevTime;
		double frameDuration;
		double frameTolerance;
	};
}
