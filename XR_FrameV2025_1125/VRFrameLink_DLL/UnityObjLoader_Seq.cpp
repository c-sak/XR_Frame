#include "pch.h"
#include "UnityObjLoader_Seq.h"

using namespace objloader;

//-----------------------------------------------------------------//
// デフォルトコンストラクタ                                        //
//-----------------------------------------------------------------//
uObjLoader_Seq::uObjLoader_Seq()
{
	frameCounter = 0;
	frameTolerance = 3;
	prevTime = std::chrono::system_clock::now();
}

//-----------------------------------------------------------------//
// デストラクタ                                                    //
//-----------------------------------------------------------------//
uObjLoader_Seq::~uObjLoader_Seq()
{
}

//-----------------------------------------------------------------//
// OBJの連番ファイルを読み込みます                                 //
//-----------------------------------------------------------------//
bool uObjLoader_Seq::read_sequence(string objfnm, int framenum, double framerate)
{
	uObjLoader cobj;                               // インスタンス生成
	char buf[128];

	for (int i = 0; i < framenum; i++)
	{		
		sprintf(buf, objfnm.c_str(), i);
		
		if (!cobj.read(string(buf)))
		{
			cout << "OBJの読込みに失敗しました。" << endl;
			return false;
		}
		cout << buf << endl;
		v_objSeq.push_back(cobj);
	}

	frameDuration = 1000.0 / framerate;

	return true;
}
