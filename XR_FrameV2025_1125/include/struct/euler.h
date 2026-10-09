#pragma once
//▼オイラー角による回転の表現
typedef struct euler_t{
	float roll, pitch, yaw;

	euler_t(): roll(0), pitch(0), yaw(0) {
	}

	euler_t(float roll_, float pitch_, float yaw_) : roll(roll_), pitch(pitch_), yaw(yaw_) {
	}
} euler_t;