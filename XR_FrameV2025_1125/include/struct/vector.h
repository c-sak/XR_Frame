#pragma once
#include <cmath>
#include <iostream>

/*3次元ベクトル*/
//-------------//
//・使い方 (例)
// (x, y, z) = (2.0, 3.0, 4.0)のベクトルを作成
// vector_t v(2.0, 3.0, 4.0);
//
// 標準的なvector_tを取得
// vector_t::Zero();　	(  0,  0,  0)
// vector_t::Up();　	(  0,  1,  0)
// vector_t::Down();　	(  0, -1,  0)
// vector_t::Forward(); (  0,  0, -1)
// vector_t::Back();　	(  0,  1,  1)
// vector_t::Right();　	(  1,  0,  0)
// vector_t::Left();　	( -1,  0,  0)
//-------------//
struct vector_t {
	float x, y, z;

	inline vector_t() : x(0.0), y(0.0), z(0.0) {}

	inline vector_t(float _x, float _y, float _z)
		: x(_x), y(_y), z(_z) {
	}

	//ベクトルの足し算
	inline vector_t operator + (vector_t& _v) {
		return vector_t(this->x + _v.x,
						this->y + _v.y,
						this->z + _v.z);
	}

	//ベクトルの引き算
	inline vector_t operator - (vector_t& _v) {
		return vector_t(this->x - _v.x,
						this->y - _v.y,
						this->z - _v.z);
	}

	//ベクトル * スカラー
	inline friend vector_t operator * (vector_t& _v, float _f) {
		return vector_t(_v.x * _f,
						_v.y * _f,
						_v.z * _f);
	}

	//スカラー * ベクトル
	inline friend vector_t operator * (float _f, vector_t& _v) {
		return vector_t(_v.x * _f,
						_v.y * _f,
						_v.z * _f);
	}

	//ベクトル / スカラー
	inline vector_t operator / (float _f) {
		return vector_t(this->x / _f,
						this->y / _f,
						this->z / _f);
	}

	inline vector_t& operator += (vector_t& _v) {
		this->x += _v.x;
		this->y += _v.y;
		this->z += _v.z;
		return *this;
	}

	inline vector_t& operator -= (vector_t& _v) {
		this->x -= _v.x;
		this->y -= _v.y;
		this->z -= _v.z;
		return *this;
	}

	inline vector_t& operator *= (float _f) {
		this->x *= _f;
		this->y *= _f;
		this->z *= _f;
		return *this;
	}

	inline vector_t& operator /= (float _f) {
		this->x /= _f;
		this->y /= _f;
		this->z /= _f;
		return *this;
	}


	inline friend bool operator == (vector_t _v1, vector_t _v2) {
		if (_v1.x == _v2.x && _v1.y == _v2.y && _v1.z == _v2.z) return true;
		return false;
	}

	inline friend bool operator != (vector_t _v1, vector_t _v2) {
		return !(_v1 == _v2);
	}


	//0ベクトル
	inline static vector_t Zero() {
		return vector_t(0.0, 0.0, 0.0);
	}

	inline static vector_t Up() {
		return vector_t(0.0, 1.0, 0.0);
	}

	inline static vector_t Down() {
		return vector_t(0.0, -1.0, 0.0);
	}

	inline static vector_t Forward() {
		return vector_t(0.0, 0.0, -1.0);
	}

	inline static vector_t Back() {
		return vector_t(0.0, 0.0, 1.0);
	}

	inline vector_t Right() {
		return vector_t(1.0, 0.0, 0.0);
	}

	inline vector_t Left() {
		return vector_t(-1.0, 0.0, 0.0);
	}

	//出力用
	inline friend std::ostream& operator << (std::ostream& _os, const vector_t& _v) {
		_os << "(" << _v.x << ", " << _v.y << ", " << _v.z << ")";
		return _os;
	}

	
};

//正規化したベクトルを新たに生成
vector_t VectorNormalized(const vector_t& _v);

//二点間の距離
float VectorDistance(const vector_t& _v, const vector_t& _w);

//ベクトルのそれぞれの要素をかけたベクトルを作成
vector_t VectorScale(const vector_t& _v, const vector_t& _w);

//外積
vector_t VectorCross(const vector_t& _v, const vector_t& _w);

//内積
float VectorDot(const vector_t& _v, const vector_t& _w);
//大きさ
float VectorMagnitude(const vector_t& _v);
//大きさの二乗
float VectorSqrMagnitude(const vector_t& _v);