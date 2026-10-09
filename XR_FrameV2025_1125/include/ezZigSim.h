#pragma once
/*
typedef struct {
	float roll, pitch, yaw;
} euler_t;
typedef struct {
	float x, y, z;
} vector_t;
typedef struct{
	float x, y, z, w;
} quaternion_t;
*/
typedef struct {

	euler_t gyro;//◆ジャイロ実装済
	euler_t angle;//◇
	float zero;//

	float compass;//◆コンパス実装済
	float faceup;//◆フェースアップ実装済
	float north; //起動時の北への方位角

	quat_t qt;//◆クォータニオン実装済

	vector_t accel;
	vector_t gravity;

	float latitude;
	float longitude;

	float pressure;
	float altitude;

	vector_t touch[5];//◆タッチ：実装済
	int n_touch;

} ezZigSimDataT;

class ezZigSim
{
public:
	ezZigSim() {};
	~ezZigSim() {};

	static ezZigSim* get(){
		static ezZigSim zigsim;
		return &zigsim;
	}

	void init();
	void update( bool debug = false );
	void term();

	void zero() {
		buffer.zero = buffer.angle.yaw;
	}

	ezZigSimDataT buffer{};
	ezZigSimDataT data{};
};
