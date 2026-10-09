#pragma once
#include "UnityObjLoader.h"

#include <vector>
#include <sstream>
#include <chrono>

using namespace std::chrono;

namespace objloader
{
	class uObjLoader_Seq
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
		uObjLoader_Seq();

		//*******************************************************************
		/*!
		*  @brief  デストラクタ
		*  @date  2020/03/10 ... 新規作成
		*/
		//********************************************************************
		~uObjLoader_Seq();

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

	/*--------------*/
	/* 非公開メンバ */
	/*--------------*/
	private:
		int frameCounter;
		vector <uObjLoader> v_objSeq;

		system_clock::time_point prevTime;
		double frameDuration;
		double frameTolerance;
	};
}
