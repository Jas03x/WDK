#ifndef WDK_MATH__HPP
#define WDK_MATH__HPP

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES // for M_PI
#endif

#include <cmath>

#include <cstdint>

#include <string>

/* Notes: */
/* All rotations are in radians */
/* All matrices are row major */
/* The coordinate system is left handed (+X is to the right, +Y is upwards, +Z is forwards) */

#define INF INFINITY

// ----------------------------------------- Scalar ----------------------------------------------
template <typename T>
T Step(T v0, T v1, T step);

template <typename T>
T Clamp(T val, T min, T max);

// ----------------------------------------- Vector ----------------------------------------------

template <typename T>
struct Vector2
{
	union
	{
		T elements[2];
		struct { T x, y; };
	};

	Vector2<T>();
	Vector2<T>(T t);
	Vector2<T>(T _x, T _y);
	Vector2<T>(const Vector2<T>& v);

	Vector2<T>& operator = (const Vector2<T>& v);

	T& operator[] (UINT32 i);
	const T& operator[] (UINT32 i) const;

	Vector2<T>  operator + () const;
	Vector2<T>  operator - () const;

	Vector2<T>  operator +  (const Vector2<T>& v) const;
	Vector2<T>  operator -  (const Vector2<T>& v) const;
	Vector2<T>& operator += (const Vector2<T>& v);
	Vector2<T>& operator -= (const Vector2<T>& v);

	Vector2<T>  operator *  (T t) const;
	Vector2<T>& operator *= (T t);

	bool        operator < (const Vector2<T>& v) const;

	bool        operator == (const Vector2<T>& v) const;
	bool        operator != (const Vector2<T>& v) const;
};

template <typename T>
struct Vector3
{
	union
	{
		T elements[3];
		struct { T x, y, z; };
		union // sub-vectors
		{
			Vector2<T> xy;
		};
	};

	Vector3<T>();
	Vector3<T>(T t);
	Vector3<T>(T _x, T _y, T _z);
	Vector3<T>(const Vector3<T>& v);

	Vector3<T>& operator = (const Vector3<T>& v);

	T& operator[] (UINT32 i);
	const T& operator[] (UINT32 i) const;

	Vector3<T>  operator + () const;
	Vector3<T>  operator - () const;

	Vector3<T>  operator +  (const Vector3<T>& v) const;
	Vector3<T>  operator -  (const Vector3<T>& v) const;
	Vector3<T>& operator += (const Vector3<T>& v);
	Vector3<T>& operator -= (const Vector3<T>& v);

	Vector3<T>  operator *  (T t) const;
	Vector3<T>& operator *= (T t);

	bool        operator < (const Vector3<T>& v) const;

	bool        operator == (const Vector3<T>& v) const;
	bool        operator != (const Vector3<T>& v) const;
};

template <typename T>
struct Vector4
{
	union
	{
		T elements[4];
		struct { T x, y, z, w; };
		union // sub-vectors
		{
			Vector2<T> xy;
			Vector3<T> xyz;
		};
	};

	Vector4<T>();
	Vector4<T>(T t);
	Vector4<T>(T _x, T _y, T _z, T _w);
	Vector4<T>(const Vector4<T>& v);

	Vector4<T>& operator = (const Vector4<T>& v);

	T& operator[] (UINT32 i);
	const T& operator[] (UINT32 i) const;

	Vector4<T>  operator + () const;
	Vector4<T>  operator - () const;

	Vector4<T>  operator +  (const Vector4<T>& v) const;
	Vector4<T>  operator -  (const Vector4<T>& v) const;
	Vector4<T>& operator += (const Vector4<T>& v);
	Vector4<T>& operator -= (const Vector4<T>& v);

	Vector4<T>  operator *  (T t) const;
	Vector4<T>& operator *= (T t);

	bool        operator < (const Vector4<T>& v) const;

	bool        operator == (const Vector4<T>& v) const;
	bool        operator != (const Vector4<T>& v) const;
};

typedef Vector2<FLOAT>  Vector2F;
typedef Vector3<FLOAT>  Vector3F;
typedef Vector4<FLOAT>  Vector4F;
typedef Vector2<INT32>  Vector2I;
typedef Vector3<INT32>  Vector3I;
typedef Vector4<INT32>  Vector4I;
typedef Vector2<UINT32> Vector2U;
typedef Vector3<UINT32> Vector3U;
typedef Vector4<UINT32> Vector4U;

namespace Vector
{
	template <typename T> T Length(const Vector2<T>& v);
	template <typename T> T Length(const Vector3<T>& v);
	template <typename T> T Length(const Vector4<T>& v);

	template <typename T> T Dot(const Vector2<T>& v0, const Vector2<T>& v1);
	template <typename T> T Dot(const Vector3<T>& v0, const Vector3<T>& v1);
	template <typename T> T Dot(const Vector4<T>& v0, const Vector4<T>& v1);

	template <typename T> Vector2<T> Normalize(const Vector2<T>& v);
	template <typename T> Vector3<T> Normalize(const Vector3<T>& v);
	template <typename T> Vector4<T> Normalize(const Vector4<T>& v);

	template <typename T> Vector3<T> Cross(const Vector3<T>& v0, const Vector3<T>& v1);

	template <typename T> T Distance(const Vector2<T>& v0, const Vector2<T>& v1);
	template <typename T> T Distance(const Vector3<T>& v0, const Vector3<T>& v1);
	template <typename T> T Distance(const Vector4<T>& v0, const Vector4<T>& v1);

	template <typename T> std::wstring ToString(const Vector2<T>& v);
	template <typename T> std::wstring ToString(const Vector3<T>& v);
	template <typename T> std::wstring ToString(const Vector4<T>& v);
}

// --------------------------------------- Quaternion --------------------------------------------

// prototype for Quaternion::Quaternion(const struct Matrix3<T>&)
template <typename T> struct Matrix3;

template <typename T = FLOAT>
struct Quaternion
{
	union
	{
		T elements[4];
		struct { T x, y, z, w; };
	};

	Quaternion();
	Quaternion(const Vector3<T>& v);
	Quaternion(const Vector4<T>& v);
	Quaternion(T _x, T _y, T _z);
	Quaternion(T _x, T _y, T _z, T _w);
	Quaternion(const struct Matrix3<T>& m);

	Quaternion  operator *  (const Quaternion& q) const;
	Quaternion& operator *= (const Quaternion& q);

	Quaternion& operator = (const Vector4<T>& v);
	Quaternion& operator = (const Quaternion& q);
};

// ----------------------------------------- Matrix ----------------------------------------------

template <typename T>
struct Matrix2
{
	union
	{
		Vector2<T> rows[2];
		FLOAT      elements[2][2];
		struct { Vector2<T> v0, v1; };
	};

	Matrix2<T>();
	Matrix2<T>(T t);
	Matrix2<T>(const Vector2<T>& v0, const Vector2<T>& v1);
	Matrix2<T>(const Matrix2& m);

	Matrix2<T>& operator = (const Matrix2<T>& m);

	Vector2<T>& operator[] (UINT32 i);
	const Vector2<T>& operator[] (UINT32 i) const;

	Matrix2<T>  operator *  (T t) const;
	Matrix2<T>& operator *= (T t);

	Matrix2<T>  operator +  (const Matrix2<T>& m) const;
	Matrix2<T>  operator -  (const Matrix2<T>& m) const;
	Matrix2<T>& operator += (const Matrix2<T>& m);
	Matrix2<T>& operator -= (const Matrix2<T>& m);

	Matrix2<T>  operator *  (const Matrix2<T>& m) const;
	Matrix2<T>& operator *= (const Matrix2<T>& m);

	Vector2<T>  operator  * (const Vector2<T>& v) const;
};

template <typename T>
struct Matrix3
{
	union
	{
		Vector3<T> rows[3];
		FLOAT      elements[3][3];
		struct { Vector3<T> v0, v1, v2; };
	};

	Matrix3<T>();
	Matrix3<T>(T t);
	Matrix3<T>(const Vector3<T>& v0, const Vector3<T>& v1, const Vector3<T>& v2);
	Matrix3<T>(const Matrix3& m);

	Matrix3<T>& operator = (const Matrix3<T>& m);

	Vector3<T>& operator[] (UINT32 i);
	const Vector3<T>& operator[] (UINT32 i) const;

	Matrix3<T>  operator *  (T t) const;
	Matrix3<T>& operator *= (T t);

	Matrix3<T>  operator +  (const Matrix3<T>& m) const;
	Matrix3<T>  operator -  (const Matrix3<T>& m) const;
	Matrix3<T>& operator += (const Matrix3<T>& m);
	Matrix3<T>& operator -= (const Matrix3<T>& m);

	Matrix3<T>  operator *  (const Matrix3<T>& m) const;
	Matrix3<T>& operator *= (const Matrix3<T>& m);

	Vector3<T>  operator  * (const Vector3<T>& v) const;
};

template <typename T>
struct Matrix4
{
	union
	{
		Vector4<T> rows[4];
		FLOAT      elements[4][4];
		struct { Vector4<T> v0, v1, v2, v3; };
	};

	Matrix4<T>();
	Matrix4<T>(T t);
	Matrix4<T>(const Vector4<T>& v0, const Vector4<T>& v1, const Vector4<T>& v2, const Vector4<T>& v3);
	Matrix4<T>(const Matrix4& m);
	Matrix4<T>(const Quaternion<T>& q);

	Matrix4<T>& operator = (const Matrix4<T>& m);

	Vector4<T>& operator[] (UINT32 i);
	const Vector4<T>& operator[] (UINT32 i) const;

	Matrix4<T>  operator *  (T t) const;
	Matrix4<T>& operator *= (T t);

	Matrix4<T>  operator +  (const Matrix4<T>& m) const;
	Matrix4<T>  operator -  (const Matrix4<T>& m) const;
	Matrix4<T>& operator += (const Matrix4<T>& m);
	Matrix4<T>& operator -= (const Matrix4<T>& m);

	Matrix4<T>  operator *  (const Matrix4<T>& m) const;
	Matrix4<T>& operator *= (const Matrix4<T>& m);

	Vector4<T>  operator  * (const Vector4<T>& v) const;
};

typedef Matrix2<FLOAT> Matrix2F;
typedef Matrix3<FLOAT> Matrix3F;
typedef Matrix4<FLOAT> Matrix4F;

namespace Matrix
{
	template <typename T> Matrix2<T> Inverse(const Matrix2<T>& m);
	template <typename T> Matrix3<T> Inverse(const Matrix3<T>& m);
	template <typename T> Matrix4<T> Inverse(const Matrix4<T>& m);

	template <typename T> Matrix2<T> Transpose(const Matrix2<T>& m);
	template <typename T> Matrix3<T> Transpose(const Matrix3<T>& m);
	template <typename T> Matrix4<T> Transpose(const Matrix4<T>& m);

	template <typename T> Matrix4<T> Translate(const Vector3<T>& v);
	template <typename T> Matrix4<T> Scale(const Vector3<T>& v);
	template <typename T> Matrix4<T> Rotate(const Vector3<T>& v);

	template <typename T> std::wstring ToString(const Matrix2<T>& m);
	template <typename T> std::wstring ToString(const Matrix3<T>& m);
	template <typename T> std::wstring ToString(const Matrix4<T>& m);
}

// Global vector operators
template <typename T> Vector2<T> operator * (T t, const Vector2<T>& v);
template <typename T> Vector3<T> operator * (T t, const Vector3<T>& v);
template <typename T> Vector4<T> operator * (T t, const Vector4<T>& v);

#endif // WDK_MATH__HPP