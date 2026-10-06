#ifndef __EZ_TRACK_VICON_H__
#define __EZ_TRACK_VICON_H__

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "ezTrack.h"

#include "Vicon/DataStreamClient.h"

#define OUTPUT_FLAG	1/* (1 GroblTranslationとGrobalEulerXYZデータのみ表示) (0 すべて表示)*/
#define EZ_DEGREE (180.0/3.14159)

class ezTracker_Vicon :
    public ezTracker
{
public:
	ezTracker_Vicon(bool use = true);
	~ezTracker_Vicon();
    void init();
	bool open(char* key, bool w);
	void read();
	void write();
	void close();

	//- 1 Subject = 1 ezTracker (骨格データ) を取得するためのAPI
	int getSubjectCount() const;
	const char* getSubjectName(int index) const;
	ezTracker* getSubject(const char* name);
	ezTracker* getSubject(int index);

private:
	void getRot(const double src[], float* roll, float* pitch, float* yaw);

	//- 1 Subject分のデータ (Clientは持たない)
	struct SubjectT {
		std::string name;						// Subject名
		ezTracker tracker;						// 骨(Segment)の配列
		std::vector<std::string> segmentNames;	// index -> Segment名
		std::vector<int> parent;				// index -> 親index (-1 = root)
		bool hierarchyValid;					// 親子キャッシュの有効性
		//- 従来のSubject単位ビュー互換用 (直近フレームの最終Segment生値)
		double lastTranslation[3];
		double lastRotationMatrix[9];
		bool hasLast;
	};

	SubjectT* findSubject(const std::string& name);
	SubjectT* addSubject(const std::string& name);
	void updateSubject(SubjectT* subject);
	void rebuildHierarchy(SubjectT* subject);
	void clearSubjects();

	std::vector<SubjectT*> subjects_;

	//- for ViconDataStreamSDK (接続は1つだけ)
	ViconDataStreamSDK::CPP::Client MyClient;
};

#endif //__EZ_TRACK_VICON_H__
