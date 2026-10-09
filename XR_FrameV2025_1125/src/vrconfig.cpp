#include "vrconfig.h"

#include "VRFLInterface.h"


std::vector<vrconfigdata> vrconfig::m_datalist;

bool vrconfig::LoadConfig(const char* filename)
{
	/// 設定ファイル(csv)の読込
	/// bool VRFL::LoadConfig(const char* filename, const char* relativebase);
	/// filename     : 設定ファイル名、記述方法によって読込むファイルが変わる
	///                - 絶対パス : そのまま利用
	///                - 相対パス : 基準フォルダからの相対パス
	///　　　　　　    - NULL or 空の文字列 : 基準フォルダにある"VRConfig.csv"を読込む
	/// relativebase : 相対パスの基準となるフォルダの設定
	///                - NULL or 空の文字列 : 実行ファイル(exe)と同じフォルダが基準となる
	///                - パス文字列 : ここに設定したパスが基準となる
	if (!VRFL::LoadConfig(filename, NULL)) return false;

	m_datalist.clear();

	int cnt = VRFL::GetConfigCount();
	int maxsize = VRFL::GetMaxStringLength();

	for (int i = 0; i < cnt; i++)
	{
		vrconfigdata cdata;

		cdata.node.resize(maxsize, 0);
		cdata.file.resize(maxsize, 0);

		VRFL::GetConfig(i, 
			&cdata.id, &cdata.scale, 
			(char*)cdata.node.c_str(), (char*)cdata.file.c_str(), maxsize);

		int attrCount = VRFL::GetConfigAttributeCount(i);
		for (int j = 0; j < attrCount; j++)
		{
			std::string attrName, attrValue;
			attrName.resize(maxsize, 0);
			attrValue.resize(maxsize, 0);

			VRFL::GetConfigAttribute(i, j, (char*)attrName.c_str(), (char*)attrValue.c_str(), maxsize);

			cdata.attributes.push_back(std::make_pair(attrName, attrValue));
		}

		m_datalist.push_back(cdata);
	}

	return true;
}

int vrconfig::GetID(const char* nodename)
{
	if (nodename == NULL) return -1;

	int cnt = m_datalist.size();

	for (int i = 0; i < cnt; i++)
	{
		if (strcmp(m_datalist[i].node.c_str(), nodename) == 0)
			return m_datalist[i].id;
	}
	
	return -1;
}

const char* vrconfig::GetFileName(const char* nodename)
{
	if (nodename == NULL) return NULL;

	int cnt = m_datalist.size();

	for (int i = 0; i < cnt; i++)
	{
		if (strcmp(m_datalist[i].node.c_str(), nodename) == 0)
			return m_datalist[i].file.c_str();
	}

	return NULL;
}

const char* vrconfig::GetFileName(int id)
{
	if (id < 0) return NULL;

	int cnt = m_datalist.size();

	for (int i = 0; i < cnt; i++)
	{
		if (m_datalist[i].id == id)
			return m_datalist[i].file.c_str();
	}

	return NULL;
}

float vrconfig::GetScale(const char* nodename)
{
	if (nodename == NULL) return 1.0f;

	int cnt = m_datalist.size();

	for (int i = 0; i < cnt; i++)
	{
		if (strcmp(m_datalist[i].node.c_str(), nodename) == 0)
			return m_datalist[i].scale;
	}

	return 1.0f;
}

const std::vector<std::pair<std::string, std::string>>& vrconfig::GetAttributes(const char* nodename)
{
	static std::vector<std::pair<std::string, std::string>> emptyAttributes;

	if (nodename == NULL) return emptyAttributes;

	int cnt = m_datalist.size();

	for (int i = 0; i < cnt; i++)
	{
		if (strcmp(m_datalist[i].node.c_str(), nodename) == 0)
			return m_datalist[i].attributes;
	}

	return emptyAttributes;
}