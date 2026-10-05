#include <stdlib.h>
#include <string.h>

#include "ezTrack_Vicon.h"

using namespace std;
using namespace ViconDataStreamSDK::CPP;

/*===========================================================================*/
ezTracker_Vicon::ezTracker_Vicon(bool use)
{
	this->use = use;	
}

ezTracker_Vicon::~ezTracker_Vicon()
{
	clearSubjects();
}

/// <summary>
/// ezTracker_Viconの初期化
/// </summary>
void ezTracker_Vicon::init() 
{
	return;
}

#ifdef PLATFORM_WINDOWS
/// <summary>
/// VICONシステムに接続
/// </summary>
/// <param name="key">Viconシステムのホスト名:Port番号(例 127.0.0.1:801)</param>
/// <param name="w">使用しない</param>
/// <returns></returns>
bool ezTracker_Vicon::open(char* key, bool w)
{
	//- VICONに接続
	if (use) {
		cout << "Connecting to VICON " << key << " ..." << flush;
		while (!MyClient.IsConnected().Connected) {
			// Direct connection
			const Output_Connect ConnectResult = MyClient.Connect(key);
			const bool ok = (ConnectResult.Result == Result::Success);

			if (!ok)
			{
				cout << "Warning - connect failed... ";
				switch (ConnectResult.Result)
				{
				case Result::ClientAlreadyConnected:
					cout << "Client Already Connected" << endl;
					break;
				case Result::InvalidHostName:
					cout << "Invalid Host Name" << endl;
					break;
				case Result::ClientConnectionFailed:
					cout << "Client Connection Failed" << endl;
					break;
				default:
					cout << "Unrecognized Error: " << ConnectResult.Result << endl;
					break;
				}

				return false;
			}

			cout << ".";
#ifdef WIN32
			Sleep(1000);
#else
			sleep(1);
#endif

			// Segment(≒リジッドボディ)情報のみ有効化
			MyClient.EnableSegmentData();

			// 座標系をZ-UPに設定
			// ezTrackDataTへ格納する際に座標変換は行う
			MyClient.SetAxisMapping(Direction::Forward,
				Direction::Left,
				Direction::Up); // Z-up

			// [参考] Y-UPに設定する場合
			//MyClient.SetAxisMapping(Direction::Forward,
			//	Direction::Up,
			//	Direction::Right); // Y-up

			// Clientのバージョン情報を表示
			Output_GetVersion _Output_GetVersion = MyClient.GetVersion();
			cout << "ViconDataStream Client Version: " << _Output_GetVersion.Major << "."
				<< _Output_GetVersion.Minor << "."
				<< _Output_GetVersion.Point << "."
				<< _Output_GetVersion.Revision << endl;

		}

		return true;
	}
	else {
		return true; // useがfalseのときには、ダミーでtrueを返す
	}
}

/// <summary>
/// VICONシステムから最新データを取得して、トラッキング情報を更新する
/// - GetFrame()は1周期に1回だけ呼び、全Subjectを同一フレームから更新する
/// - Subjectごとの骨格は ezTracker (subjects_) に格納する
/// </summary>
void ezTracker_Vicon::read()
{
	if (!use) return;

	//- VICONから最新データを取得 (この周期で1回だけ)
	if (MyClient.GetFrame().Result != Result::Success)
		return;

	//- 従来のSubject単位ビューを初期化
	n_tracks = 0;
	for (int i = 0; i < _n_tracks; i++) {
		trackarray.data[i].id = -1;
	}

	const unsigned int SubjectCount = MyClient.GetSubjectCount().SubjectCount;
	if (OUTPUT_FLAG == 0) {
		cout << "Subjects (" << SubjectCount << "):" << endl;
	}

	for (unsigned int SubjectIndex = 0; SubjectIndex < SubjectCount; ++SubjectIndex)
	{
		const string SubjectName = MyClient.GetSubjectName(SubjectIndex).SubjectName;

		//- Subjectごとの骨格データを更新 (1 Subject = 1 ezTracker)
		SubjectT* subject = findSubject(SubjectName);
		if (subject == NULL) subject = addSubject(SubjectName);
		updateSubject(subject);

		//- 従来のSubject単位ビュー (最後のSegmentの値、Subject名で検索できる)
		if (SubjectIndex < (unsigned int)_n_tracks && subject->hasLast) {
			ezTrackDataT* data = &trackarray.data[SubjectIndex];
			n_tracks = (int)SubjectIndex;
			data->id = (int)SubjectIndex;
			data->parent = -1;
			strncpy(data->name, SubjectName.c_str(), sizeof(data->name) - 1);
			data->name[sizeof(data->name) - 1] = '\0';
			data->x = (float)(-subject->lastTranslation[1] * 0.001); // [mm] -> [m]
			data->y = (float)( subject->lastTranslation[2] * 0.001); // [mm] -> [m]
			data->z = (float)(-subject->lastTranslation[0] * 0.001); // [mm] -> [m]
			getRot(subject->lastRotationMatrix, &data->roll, &data->pitch, &data->yaw);

			if (OUTPUT_FLAG == 0) {
				printf("POS: %f\t%f\t%f\n", data->x, data->y, data->z);
				printf("ROT: \n%f\n%f\n%f\n", data->roll, data->pitch, data->yaw);
			}
		}
	}
}

/// <summary>
/// 1 Subject分の骨格データを更新する
/// Segment 1つ = ezTrackDataT 1つ。parentは同一Subject内の親index (-1 = root)
/// </summary>
void ezTracker_Vicon::updateSubject(SubjectT* subject)
{
	const unsigned int SegmentCount = MyClient.GetSegmentCount(subject->name).SegmentCount;

	//- 骨の構成が変わったときだけ親子関係を再構築
	if (!subject->hierarchyValid || subject->segmentNames.size() != SegmentCount) {
		rebuildHierarchy(subject);
	}

	ezTracker* trk = &subject->tracker;
	for (int i = 0; i < _n_tracks; ++i) {
		trk->getTrackData(i)->id = -1;
	}
	subject->hasLast = false;

	for (unsigned int SegmentIndex = 0;
		SegmentIndex < SegmentCount && SegmentIndex < (unsigned int)_n_tracks;
		++SegmentIndex)
	{
		const string& SegmentName = subject->segmentNames[SegmentIndex];

		const Output_GetSegmentGlobalTranslation t =
			MyClient.GetSegmentGlobalTranslation(subject->name, SegmentName);
		const Output_GetSegmentGlobalRotationMatrix r =
			MyClient.GetSegmentGlobalRotationMatrix(subject->name, SegmentName);
		if (t.Result != Result::Success || r.Result != Result::Success)
			continue;

		//- 従来のSubject単位ビュー用に最終Segmentの生値を保持
		memcpy(subject->lastTranslation, t.Translation, sizeof(subject->lastTranslation));
		memcpy(subject->lastRotationMatrix, r.Rotation, sizeof(subject->lastRotationMatrix));
		subject->hasLast = true;

		//- 骨1本分のデータを格納
		ezTrackDataT* data = trk->getTrackData((int)SegmentIndex);
		data->id = (int)SegmentIndex;
		data->parent = subject->parent[SegmentIndex];
		strncpy(data->name, SegmentName.c_str(), sizeof(data->name) - 1);
		data->name[sizeof(data->name) - 1] = '\0';

		//- 遮蔽フレームでは前回値を保持する (全0の行列でgetRot()がNaNになるのを防ぐ)
		if (!t.Occluded) {
			data->x = (float)(-t.Translation[1] * 0.001); // [mm] -> [m]
			data->y = (float)( t.Translation[2] * 0.001); // [mm] -> [m]
			data->z = (float)(-t.Translation[0] * 0.001); // [mm] -> [m]
			getRot(r.Rotation, &data->roll, &data->pitch, &data->yaw);
		}
	}
}

/// <summary>
/// Subject内のSegment名から親子関係 (parent index) を構築する
/// 親が子より後ろに並んでいても解決できるよう2パスで行う
/// </summary>
void ezTracker_Vicon::rebuildHierarchy(SubjectT* subject)
{
	const unsigned int SegmentCount = MyClient.GetSegmentCount(subject->name).SegmentCount;

	subject->segmentNames.clear();
	subject->parent.assign(SegmentCount, -1);

	//- パス1: Segment名 -> index
	map<string, int> indexOf;
	for (unsigned int i = 0; i < SegmentCount; ++i) {
		const string name = MyClient.GetSegmentName(subject->name, i).SegmentName;
		subject->segmentNames.push_back(name);
		indexOf[name] = (int)i;
	}

	//- パス2: 親名 -> 親index (空文字はroot)
	for (unsigned int i = 0; i < SegmentCount; ++i) {
		const string parentName =
			MyClient.GetSegmentParentName(subject->name, subject->segmentNames[i]).SegmentName;
		if (parentName.empty()) continue;
		map<string, int>::iterator it = indexOf.find(parentName);
		if (it != indexOf.end()) subject->parent[i] = it->second;
	}

	subject->hierarchyValid = true;
}

/// <summary>
/// 
/// </summary>
void ezTracker_Vicon::write()
{
	return;
}

/// <summary>
/// VICONシステムからの切断
/// </summary>
void ezTracker_Vicon::close()
{
	if (use && MyClient.IsConnected().Connected)
	{
		cout << " Disconnecting..." << endl;
		MyClient.Disconnect();
	}

	clearSubjects();
	return;
}
#else
bool ezTracker_Vicon::open(char* key, bool w) { return false; }
void ezTracker_Vicon::read() {}
void ezTracker_Vicon::write() {}
void ezTracker_Vicon::close() {}
#endif

/// <summary>
/// Subject名でSubjectTを探す
/// </summary>
ezTracker_Vicon::SubjectT* ezTracker_Vicon::findSubject(const string& name)
{
	for (size_t i = 0; i < subjects_.size(); ++i) {
		if (subjects_[i]->name == name) return subjects_[i];
	}
	return NULL;
}

/// <summary>
/// SubjectTを新規作成して登録する
/// </summary>
ezTracker_Vicon::SubjectT* ezTracker_Vicon::addSubject(const string& name)
{
	SubjectT* subject = new SubjectT();
	subject->name = name;
	subject->hierarchyValid = false;
	subject->hasLast = false;
	memset(subject->lastTranslation, 0, sizeof(subject->lastTranslation));
	memset(subject->lastRotationMatrix, 0, sizeof(subject->lastRotationMatrix));

	ezTracker* trk = &subject->tracker;
	for (int i = 0; i < _n_tracks; ++i) {
		ezTrackDataT* data = trk->getTrackData(i);
		memset(data, 0, sizeof(ezTrackDataT));
		data->id = -1;
		data->parent = -1;
	}

	subjects_.push_back(subject);
	return subject;
}

/// <summary>
/// 保持しているSubjectTをすべて破棄する
/// </summary>
void ezTracker_Vicon::clearSubjects()
{
	for (size_t i = 0; i < subjects_.size(); ++i) {
		delete subjects_[i];
	}
	subjects_.clear();
}

/// <summary>
/// Subject数
/// </summary>
int ezTracker_Vicon::getSubjectCount() const
{
	return (int)subjects_.size();
}

/// <summary>
/// indexからSubject名を取得 (範囲外は空文字)
/// </summary>
const char* ezTracker_Vicon::getSubjectName(int index) const
{
	if (index < 0 || index >= (int)subjects_.size()) return "";
	return subjects_[index]->name.c_str();
}

/// <summary>
/// Subject名から骨格データ (1 Subject = 1 ezTracker) を取得
/// </summary>
ezTracker* ezTracker_Vicon::getSubject(const char* name)
{
	if (name == NULL) return NULL;
	SubjectT* subject = findSubject(string(name));
	return (subject != NULL) ? &subject->tracker : NULL;
}

/// <summary>
/// indexから骨格データ (1 Subject = 1 ezTracker) を取得
/// </summary>
ezTracker* ezTracker_Vicon::getSubject(int index)
{
	if (index < 0 || index >= (int)subjects_.size()) return NULL;
	return &subjects_[index]->tracker;
}

/// <summary>
/// 回転行列からRoll, Pitch, Yawに変換
/// </summary>
/// <param name="src"></param>
/// <param name="roll"></param>
/// <param name="pitch"></param>
/// <param name="yaw"></param>
void ezTracker_Vicon::getRot(const double src[],
	float* roll, float* pitch, float* yaw)
{
	float x[3], y[3], z[3];
	float buf[3];
	double m[3][3];

	memcpy((void*)m, (void*)src, sizeof(m));

	x[0] = m[1][1];
	x[1] = -m[2][1];
	x[2] = m[0][1];

	y[0] = -m[1][2];
	y[1] = m[2][2];
	y[2] = -m[0][2];

	z[0] = m[1][0];
	z[1] = -m[2][0];
	z[2] = m[0][0];

	/*---- PITCH ----*/
	*pitch = EZ_DEGREE * asinf(-z[1]); // [-90,90]
	/*---- YAW & ROLL ----*/
	if (-90.0 < *pitch && *pitch < 90.0) {
		*yaw = EZ_DEGREE * atan2f(z[0], z[2]); // [-180,180]
		buf[0] = z[2];
		buf[1] = 0.;
		buf[2] = -z[0];
		float dot = x[0] * buf[0] + x[1] * buf[1] + x[2] * buf[2];
		float norm = sqrtf((x[0] * x[0] + x[1] * x[1] + x[2] * x[2])
			* (buf[0] * buf[0] + buf[1] * buf[1] + buf[2] * buf[2]));
		float c = dot / norm;
		if (c < -1.) c = -1.;
		if (1. < c) c = 1.;
		*roll = EZ_DEGREE * acosf(c);
		if (x[1] < 0.) *roll = -*roll;
	}
	else {
		if (z[1] < 0.) *yaw = EZ_DEGREE * atan2f(y[0], y[2]);
		else            *yaw = EZ_DEGREE * atan2f(-y[0], -y[2]);
		*roll = 0.;
	}
	if (*roll == -0.) *roll = 0.;
	if (*pitch == -0.) *pitch = 0.;
	if (*yaw == -0.) *yaw = 0.;
	return;

}