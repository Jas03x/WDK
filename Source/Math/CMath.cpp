#include "CMath.hpp"

#include <cmath>

#include <algorithm>

#include <strsafe.h>

#include "Wdk.hpp"
#include "WdkDef.hpp"
#include "WdkMath.hpp"

using namespace Wdk;

#define SQR(x) ((x) * (x))

#define __DOT2(x0, y0, x1, y1) (((x0) * (x1)) + ((y0) * (y1)))
#define __DOT3(x0, y0, z0, x1, y1, z1) (((x0) * (x1)) + ((y0) * (y1)) + ((z0) * (z1)))
#define __DOT4(x0, y0, z0, w0, x1, y1, z1, w1) (((x0) * (x1)) + ((y0) * (y1)) + ((z0) * (z1)) + ((w0) * (w1)))

#define DOT2(m, m0, m1, r, c) (m)[c][r] = __DOT2((m0)[0][r], (m0)[1][r], (m1)[c][0], (m1)[c][1])
#define DOT3(m, m0, m1, r, c) (m)[c][r] = __DOT3((m0)[0][r], (m0)[1][r], (m0)[2][r], (m1)[c][0], (m1)[c][1], (m1)[c][2])
#define DOT4(m, m0, m1, r, c) (m)[c][r] = __DOT4((m0)[0][r], (m0)[1][r], (m0)[2][r], (m0)[3][r], (m1)[c][0], (m1)[c][1], (m1)[c][2], (m1)[c][3])

#define DET2(a, b, c, d) ((a) * (d) - (b) * (c))
#define DET3(a, b, c, d, e, f, g, h, i) (((a) * DET2(e, f, h, i)) - ((b) * DET2(d, f, g, i)) + ((c) * DET2(d, e, g, h)))

// ------------------------------------ Scalar functions ------------------------------------------

template <typename T>
T Step(T v0, T v1, T step)
{
	T d = v1 - v0;

	if constexpr (std::is_floating_point<T>::value) { return (std::fabs(d) > step) ? (v0 + std::copysign(step, d)) : v1; }
	else { return static_cast<T>((std::abs(static_cast<LONGLONG>(d)) > step) ? (v0 + std::copysign(step, d)) : v1); }
}

template <typename T>
T Clamp(T val, T min, T max)
{
	return std::max(std::min(val, max), min);
}

// ----------------------------- Scalar function instantiations -----------------------------------

#define INSTANTIATE_SCALAR_FUNCTION_TEMPLATES(X) \
	template X Step(X v0, X v1, X step);		 \
    template X Clamp(X val, X min, X max);       \

INSTANTIATE_SCALAR_FUNCTION_TEMPLATES(char);
INSTANTIATE_SCALAR_FUNCTION_TEMPLATES(short);
INSTANTIATE_SCALAR_FUNCTION_TEMPLATES(int);
INSTANTIATE_SCALAR_FUNCTION_TEMPLATES(long);
INSTANTIATE_SCALAR_FUNCTION_TEMPLATES(unsigned char);
INSTANTIATE_SCALAR_FUNCTION_TEMPLATES(unsigned short);
INSTANTIATE_SCALAR_FUNCTION_TEMPLATES(unsigned int);
INSTANTIATE_SCALAR_FUNCTION_TEMPLATES(unsigned long);
INSTANTIATE_SCALAR_FUNCTION_TEMPLATES(float);
INSTANTIATE_SCALAR_FUNCTION_TEMPLATES(double);

// ----------------------------------------- Vector2 ----------------------------------------------

template <typename T> Vector2<T>::Vector2() : Vector2<T>(0, 0) { }
template <typename T> Vector2<T>::Vector2(T t) : Vector2<T>(t, t) { }
template <typename T> Vector2<T>::Vector2(const Vector2<T>& v) : Vector2<T>(v.x, v.y) { }
template <typename T> Vector2<T>::Vector2(T _x, T _y) { x = _x; y = _y; }

template <typename T> Vector2<T>& Vector2<T>::operator = (const Vector2<T>& v)
{
	for (uint32_t i = 0; i < _countof(elements); i++)
	{
		elements[i] = v.elements[i];
	}
	return *this;
}

template <typename T> T& Vector2<T>::operator[] (uint32_t i) { return elements[i]; }
template <typename T> const T& Vector2<T>::operator[] (uint32_t i) const { return elements[i]; }

template <typename T> Vector2<T> Vector2<T>::operator - () const
{
	if constexpr (std::is_signed<T>::value) { return Vector2<T>(-x, -y); }
	else { WdkAssert(false, __FUNCTION__ L": cannot use unary minus operator on unsigned type"); return *this; }
}
template <typename T> Vector2<T> Vector2<T>::operator + () const { return Vector2<T>(+x, +y); }

template <typename T> Vector2<T>  Vector2<T>::operator +  (const Vector2<T>& v) const { return Vector2<T>(x + v.x, y + v.y); }
template <typename T> Vector2<T>  Vector2<T>::operator -  (const Vector2<T>& v) const { return Vector2<T>(x - v.x, y - v.y); }
template <typename T> Vector2<T>& Vector2<T>::operator += (const Vector2<T>& v) { x += v.x; y += v.y; return *this; }
template <typename T> Vector2<T>& Vector2<T>::operator -= (const Vector2<T>& v) { x -= v.x; y -= v.y; return *this; }

template <typename T> Vector2<T>  Vector2<T>::operator *  (T t) const { return Vector2<T>(x * t, y * t); }
template <typename T> Vector2<T>& Vector2<T>::operator *= (T t) { x *= t; y *= t; return *this; }

template <typename T> Vector2<T> operator * (T t, Vector2<T>& v) { return v * t; }

template <typename T> bool Vector2<T>::operator < (const Vector2<T>& v) const
{
	for (uint32_t i = 0; i < _countof(elements); i++)
	{
		if (elements[i] < v.elements[i]) return true;
		else if (elements[i] == v.elements[i]) continue;
		else return false;
	}
	return false;
}

template <typename T> bool Vector2<T>::operator == (const Vector2<T>& v) const { return (x == v.x) && (y == v.y); }
template <typename T> bool Vector2<T>::operator != (const Vector2<T>& v) const { return (x != v.x) || (y != v.y); }

// ----------------------------------------- Vector3 ----------------------------------------------

template <typename T> Vector3<T>::Vector3() : Vector3<T>(0, 0, 0) { }
template <typename T> Vector3<T>::Vector3(T t) : Vector3<T>(t, t, t) { }
template <typename T> Vector3<T>::Vector3(const Vector3<T>& v) : Vector3<T>(v.x, v.y, v.z) { }
template <typename T> Vector3<T>::Vector3(T _x, T _y, T _z) { x = _x; y = _y; z = _z; }

template <typename T> Vector3<T>& Vector3<T>::operator = (const Vector3<T>& v)
{
	for (uint32_t i = 0; i < _countof(elements); i++)
	{
		elements[i] = v.elements[i];
	}
	return *this;
}

template <typename T> T& Vector3<T>::operator[] (uint32_t i) { return elements[i]; }
template <typename T> const T& Vector3<T>::operator[] (uint32_t i) const { return elements[i]; }

template <typename T> Vector3<T> Vector3<T>::operator - () const
{
	if constexpr (std::is_signed<T>::value) { return Vector3<T>(-x, -y, -z); }
	else { WdkAssert(false, __FUNCTION__ L": cannot use unary minus operator on unsigned type"); return *this; }
}
template <typename T> Vector3<T> Vector3<T>::operator + () const { return Vector3<T>(+x, +y, +z); }

template <typename T> Vector3<T>  Vector3<T>::operator +  (const Vector3<T>& v) const { return Vector3<T>(x + v.x, y + v.y, z + v.z); }
template <typename T> Vector3<T>  Vector3<T>::operator -  (const Vector3<T>& v) const { return Vector3<T>(x - v.x, y - v.y, z - v.z); }
template <typename T> Vector3<T>& Vector3<T>::operator += (const Vector3<T>& v) { x += v.x; y += v.y; z += v.z; return *this; }
template <typename T> Vector3<T>& Vector3<T>::operator -= (const Vector3<T>& v) { x -= v.x; y -= v.y; z -= v.z; return *this; }

template <typename T> Vector3<T>  Vector3<T>::operator *  (T t) const { return Vector3<T>(x * t, y * t, z * t); }
template <typename T> Vector3<T>& Vector3<T>::operator *= (T t) { x *= t; y *= t; z *= t; return *this; }

template <typename T> Vector3<T> operator * (T t, Vector3<T>& v) { return v * t; }

template <typename T> bool Vector3<T>::operator < (const Vector3<T>& v) const
{
	for (uint32_t i = 0; i < _countof(elements); i++)
	{
		if (elements[i] < v.elements[i]) return true;
		else if (elements[i] == v.elements[i]) continue;
		else return false;
	}
	return false;
}

template <typename T> bool Vector3<T>::operator == (const Vector3<T>& v) const { return (x == v.x) && (y == v.y) && (z == v.z); }
template <typename T> bool Vector3<T>::operator != (const Vector3<T>& v) const { return (x != v.x) || (y != v.y) || (z != v.z); }

// ----------------------------------------- Vector4 ----------------------------------------------

template <typename T> Vector4<T>::Vector4() : Vector4<T>(0, 0, 0, 0) { }
template <typename T> Vector4<T>::Vector4(T t) : Vector4<T>(t, t, t, t) { }
template <typename T> Vector4<T>::Vector4(const Vector4<T>& v) : Vector4<T>(v.x, v.y, v.z, v.w) { }
template <typename T> Vector4<T>::Vector4(T _x, T _y, T _z, T _w) { x = _x; y = _y; z = _z; w = _w; }

template <typename T> Vector4<T>& Vector4<T>::operator = (const Vector4<T>& v)
{
	for (uint32_t i = 0; i < _countof(elements); i++)
	{
		elements[i] = v.elements[i];
	}
	return *this;
}

template <typename T> T& Vector4<T>::operator[] (uint32_t i) { return elements[i]; }
template <typename T> const T& Vector4<T>::operator[] (uint32_t i) const { return elements[i]; }

template <typename T> Vector4<T> Vector4<T>::operator - () const
{
	if constexpr (std::is_signed<T>::value) { return Vector4<T>(-x, -y, -z, -w); }
	else { WdkAssert(false, __FUNCTION__ L": cannot use unary minus operator on unsigned type"); return *this; }
}
template <typename T> Vector4<T> Vector4<T>::operator + () const { return Vector4<T>(+x, +y, +z, +w); }

template <typename T> Vector4<T>  Vector4<T>::operator +  (const Vector4<T>& v) const { return Vector4<T>(x + v.x, y + v.y, z + v.z, w + v.w); }
template <typename T> Vector4<T>  Vector4<T>::operator -  (const Vector4<T>& v) const { return Vector4<T>(x - v.x, y - v.y, z - v.z, w - v.w); }
template <typename T> Vector4<T>& Vector4<T>::operator += (const Vector4<T>& v) { x += v.x; y += v.y; z += v.z; w += v.w; return *this; }
template <typename T> Vector4<T>& Vector4<T>::operator -= (const Vector4<T>& v) { x -= v.x; y -= v.y; z -= v.z; w -= v.w; return *this; }

template <typename T> Vector4<T>  Vector4<T>::operator *  (T t) const { return Vector4<T>(x * t, y * t, z * t, w * t); }
template <typename T> Vector4<T>& Vector4<T>::operator *= (T t) { x *= t; y *= t; z *= t; w *= t; return *this; }

template <typename T> Vector4<T> operator * (T t, Vector4<T>& v) { return v * t; }

template <typename T> bool Vector4<T>::operator < (const Vector4<T>& v) const
{
	for (uint32_t i = 0; i < _countof(elements); i++)
	{
		if (elements[i] < v.elements[i]) return true;
		else if (elements[i] == v.elements[i]) continue;
		else return false;
	}
	return false;
}

template <typename T> bool Vector4<T>::operator == (const Vector4<T>& v) const { return (x == v.x) && (y == v.y) && (z == v.z) && (w == v.w); }
template <typename T> bool Vector4<T>::operator != (const Vector4<T>& v) const { return (x != v.x) || (y != v.y) || (z != v.z) || (w != v.w); }

// ------------------------------------ Vector functions ------------------------------------------

template <typename T> T Vector::Length(const Vector2<T>& v) { return std::sqrt(SQR(v.x) + SQR(v.y)); }
template <typename T> T Vector::Length(const Vector3<T>& v) { return std::sqrt(SQR(v.x) + SQR(v.y) + SQR(v.z)); }
template <typename T> T Vector::Length(const Vector4<T>& v) { return std::sqrt(SQR(v.x) + SQR(v.y) + SQR(v.z) + SQR(v.w)); }

template <typename T> T Vector::Dot(const Vector2<T>& v0, const Vector2<T>& v1) { return (v0.x * v1.x) + (v0.y + v1.y); }
template <typename T> T Vector::Dot(const Vector3<T>& v0, const Vector3<T>& v1) { return (v0.x * v1.x) + (v0.y + v1.y) + (v0.z * v1.z); }
template <typename T> T Vector::Dot(const Vector4<T>& v0, const Vector4<T>& v1) { return (v0.x * v1.x) + (v0.y + v1.y) + (v0.z * v1.z) + (v0.w * v1.w); }

template <typename T> Vector2<T> Vector::Normalize(const Vector2<T>& v) { return v * (1.0f / Vector::Length(v)); }
template <typename T> Vector3<T> Vector::Normalize(const Vector3<T>& v) { return v * (1.0f / Vector::Length(v)); }
template <typename T> Vector4<T> Vector::Normalize(const Vector4<T>& v) { return v * (1.0f / Vector::Length(v)); }

template <typename T> Vector3<T> Vector::Cross(const Vector3<T>& v0, const Vector3<T>& v1)
{
	T x = (v0.y * v1.z) - (v0.z * v1.y);
	T y = (v0.x * v1.z) - (v0.z * v1.x);
	T z = (v0.x * v1.y) - (v0.y * v1.x);
	return Vector3<T>(x, -y, z);
}

template <typename T> T Vector::Distance(const Vector2<T>& v0, const Vector2<T>& v1) { return std::sqrt(SQR(v1.x - v0.x) + SQR(v1.y - v0.y)); }
template <typename T> T Vector::Distance(const Vector3<T>& v0, const Vector3<T>& v1) { return std::sqrt(SQR(v1.x - v0.x) + SQR(v1.y - v0.y) + SQR(v1.z - v0.z)); }
template <typename T> T Vector::Distance(const Vector4<T>& v0, const Vector4<T>& v1) { return std::sqrt(SQR(v1.x - v0.x) + SQR(v1.y - v0.y) + SQR(v1.z - v0.z) + SQR(v1.w - v0.w)); }

template <typename T> VOID WriteToBuffer(PWCHAR& pBuffer, SIZE_T& size, T t)
{
	SIZE_T pcchRemaining = 0;

	if constexpr (std::is_arithmetic<T>::value)
	{
		if constexpr (std::is_floating_point<T>::value) { StringCchPrintfExW(pBuffer, size, NULL, &pcchRemaining, 0, L"%f", t); }
		else if constexpr (std::is_unsigned<T>::value)  { StringCchPrintfExW(pBuffer, size, NULL, &pcchRemaining, 0, L"%llu", static_cast<ULONGLONG>(t)); }
		else                                            { StringCchPrintfExW(pBuffer, size, NULL, &pcchRemaining, 0, L"%lli", static_cast<LONGLONG>(t)); }
	}
	else                                                { StringCchPrintfExW(pBuffer, size, NULL, &pcchRemaining, 0, L"%s", t); }

	pBuffer += pcchRemaining;
	size -= pcchRemaining;
}

template <typename T> std::wstring VectorToString(T* pElements, SIZE_T nElements)
{
	WCHAR buffer[2048] = {};

	PWCHAR bufPtr = buffer;
	SIZE_T bufSize = 2048;

	WriteToBuffer(bufPtr, bufSize, "<");
	for (uint32_t i = 0; i < nElements; i++)
	{
		WriteToBuffer(bufPtr, bufSize, pElements[i]);
		if (i != (nElements - 1))
		{
			WriteToBuffer(bufPtr, bufSize, ", ");
		}
	}
	WriteToBuffer(bufPtr, bufSize, ">");
	
	*bufPtr = 0; // null-terminate

	return std::wstring(buffer);
}

template <typename T> std::wstring Vector::ToString(const Vector2<T>& v) { return VectorToString(v.elements, _countof(v.elements)); }
template <typename T> std::wstring Vector::ToString(const Vector3<T>& v) { return VectorToString(v.elements, _countof(v.elements)); }
template <typename T> std::wstring Vector::ToString(const Vector4<T>& v) { return VectorToString(v.elements, _countof(v.elements)); }

// ------------------------- Vector template/function instantiations ------------------------------

#define INSTANTIATE_VECTOR_TEMPLATES(X)                                                 \
    template struct Vector2<X>;                                                         \
    template struct Vector3<X>;                                                         \
    template struct Vector4<X>;                                                         \

#define INSTANTIATE_VECTOR_FUNCTION_TEMPLATES(X)										\
	template std::wstring Vector::ToString(const Vector2<X>& v);							\
	template std::wstring Vector::ToString(const Vector3<X>& v);							\
	template std::wstring Vector::ToString(const Vector4<X>& v);							\

#define INSTANTIATE_VECTOR_MATH_TEMPLATES(X)											\
    template X Vector::Length(const Vector2<X>& v);                                     \
    template X Vector::Length(const Vector3<X>& v);                                     \
    template X Vector::Length(const Vector4<X>& v);                                     \
    template X Vector::Dot(const Vector2<X>& v0, const Vector2<X>& v1);                 \
    template X Vector::Dot(const Vector3<X>& v0, const Vector3<X>& v1);                 \
    template X Vector::Dot(const Vector4<X>& v0, const Vector4<X>& v1);                 \
    template Vector2<X> Vector::Normalize(const Vector2<X>& v0);                        \
    template Vector3<X> Vector::Normalize(const Vector3<X>& v0);                        \
    template Vector4<X> Vector::Normalize(const Vector4<X>& v0);                        \
    template Vector3<X> Vector::Cross(const Vector3<X>& v0, const Vector3<X>& v1);      \
	template X Vector::Distance(const Vector2<X>& v0, const Vector2<X>& v1);            \
    template X Vector::Distance(const Vector3<X>& v0, const Vector3<X>& v1);            \
    template X Vector::Distance(const Vector4<X>& v0, const Vector4<X>& v1);            \

INSTANTIATE_VECTOR_TEMPLATES(char);
INSTANTIATE_VECTOR_TEMPLATES(short);
INSTANTIATE_VECTOR_TEMPLATES(int);
INSTANTIATE_VECTOR_TEMPLATES(long);
INSTANTIATE_VECTOR_TEMPLATES(unsigned char);
INSTANTIATE_VECTOR_TEMPLATES(unsigned short);
INSTANTIATE_VECTOR_TEMPLATES(unsigned int);
INSTANTIATE_VECTOR_TEMPLATES(unsigned long);
INSTANTIATE_VECTOR_TEMPLATES(float);
INSTANTIATE_VECTOR_TEMPLATES(double);

INSTANTIATE_VECTOR_FUNCTION_TEMPLATES(char);
INSTANTIATE_VECTOR_FUNCTION_TEMPLATES(short);
INSTANTIATE_VECTOR_FUNCTION_TEMPLATES(int);
INSTANTIATE_VECTOR_FUNCTION_TEMPLATES(long);
INSTANTIATE_VECTOR_FUNCTION_TEMPLATES(unsigned char);
INSTANTIATE_VECTOR_FUNCTION_TEMPLATES(unsigned short);
INSTANTIATE_VECTOR_FUNCTION_TEMPLATES(unsigned int);
INSTANTIATE_VECTOR_FUNCTION_TEMPLATES(unsigned long);
INSTANTIATE_VECTOR_FUNCTION_TEMPLATES(float);
INSTANTIATE_VECTOR_FUNCTION_TEMPLATES(double);

INSTANTIATE_VECTOR_MATH_TEMPLATES(float);
INSTANTIATE_VECTOR_MATH_TEMPLATES(double);

// ----------------------------------------- Matrix2 ----------------------------------------------

template <typename T> Matrix2<T>::Matrix2() : Matrix2<T>(static_cast<T>(1)) { }
template <typename T> Matrix2<T>::Matrix2(T t)
{
	rows[0] = Vector2<T>(1, 0);
	rows[1] = Vector2<T>(0, 1);
}
template <typename T> Matrix2<T>::Matrix2(const Vector2<T>& v0, const Vector2<T>& v1)
{
	rows[0] = v0;
	rows[1] = v1;
}
template <typename T> Matrix2<T>::Matrix2(const Matrix2<T>& m) : Matrix2<T>(m[0], m[1]) { }

template <typename T> Matrix2<T>& Matrix2<T>::operator = (const Matrix2<T>& m)
{
	for (uint32_t i = 0; i < _countof(rows); i++)
	{
		rows[i] = m.rows[i];
	}
	return *this;
}

template <typename T> Vector2<T>& Matrix2<T>::operator[] (uint32_t i) { return rows[i]; }
template <typename T> const Vector2<T>& Matrix2<T>::operator[] (uint32_t i) const { return rows[i]; }

template <typename T> Matrix2<T> Matrix2<T>::operator * (T t) const
{
	return Matrix2<T>(
		t * rows[0],
		t * rows[1]
	);
}

template <typename T> Matrix2<T>& Matrix2<T>::operator *= (T t)
{
	rows[0] *= t;
	rows[1] *= t;
	return *this;
}

template <typename T> Matrix2<T> Matrix2<T>::operator + (const Matrix2<T>& m) const
{
	return Matrix2<T>(
		rows[0] + m.rows[0],
		rows[1] + m.rows[1]
	);
}

template <typename T> Matrix2<T> Matrix2<T>::operator - (const Matrix2<T>& m) const
{
	return Matrix2<T>(
		rows[0] - m.rows[0],
		rows[1] - m.rows[1]
	);
}

template <typename T> Matrix2<T>& Matrix2<T>::operator += (const Matrix2<T>& m)
{
	for (uint32_t i = 0; i < _countof(rows); i++)
	{
		rows[i] += m.rows[i];
	}
	return *this;
}

template <typename T> Matrix2<T>& Matrix2<T>::operator -= (const Matrix2<T>& m)
{
	for (uint32_t i = 0; i < _countof(rows); i++)
	{
		rows[i] -= m.rows[i];
	}
	return *this;
}

template <typename T> Matrix2<T> Matrix2<T>::operator * (const Matrix2<T>& m) const
{
	Matrix2<T> r;
	DOT2(r.rows, rows, m.rows, 0, 0);
	DOT2(r.rows, rows, m.rows, 0, 1);
	DOT2(r.rows, rows, m.rows, 1, 0);
	DOT2(r.rows, rows, m.rows, 1, 1);
	return r;
}

template <typename T> Matrix2<T>& Matrix2<T>::operator *= (const Matrix2<T>& m)
{
	Matrix2<T> c(*this);
	DOT2(rows, c.rows, m.rows, 0, 0);
	DOT2(rows, c.rows, m.rows, 0, 1);
	DOT2(rows, c.rows, m.rows, 1, 0);
	DOT2(rows, c.rows, m.rows, 1, 1);
	return *this;
}

template <typename T> Vector2<T> Matrix2<T>::operator * (const Vector2<T>& v) const
{
	return Vector2<T>(
		Vector::Dot<T>(v, rows[0]),
		Vector::Dot<T>(v, rows[1])
	);
}

// ----------------------------------------- Matrix3 ----------------------------------------------

template <typename T> Matrix3<T>::Matrix3() : Matrix3<T>(static_cast<T>(1)) { }
template <typename T> Matrix3<T>::Matrix3(T t)
{
	rows[0] = Vector3<T>(1, 0, 0);
	rows[1] = Vector3<T>(0, 1, 0);
	rows[2] = Vector3<T>(0, 0, 1);
}
template <typename T> Matrix3<T>::Matrix3(const Vector3<T>& v0, const Vector3<T>& v1, const Vector3<T>& v2)
{
	rows[0] = v0;
	rows[1] = v1;
	rows[2] = v2;
}
template <typename T> Matrix3<T>::Matrix3(const Matrix3<T>& m) : Matrix3<T>(m[0], m[1], m[2]) { }

template <typename T> Matrix3<T>& Matrix3<T>::operator = (const Matrix3<T>& m)
{
	for (uint32_t i = 0; i < _countof(rows); i++)
	{
		rows[i] = m.rows[i];
	}
}

template <typename T> Vector3<T>& Matrix3<T>::operator[] (uint32_t i) { return rows[i]; }
template <typename T> const Vector3<T>& Matrix3<T>::operator[] (uint32_t i) const { return rows[i]; }

template <typename T> Matrix3<T> Matrix3<T>::operator * (T t) const
{
	return Matrix3<T>(
		t * rows[0],
		t * rows[1],
		t * rows[2]
	);
}

template <typename T> Matrix3<T>& Matrix3<T>::operator *= (T t)
{
	rows[0] *= t;
	rows[1] *= t;
	rows[2] *= t;
	return *this;
}

template <typename T> Matrix3<T> Matrix3<T>::operator + (const Matrix3<T>& m) const
{
	return Matrix3<T>(
		rows[0] + m.rows[0],
		rows[1] + m.rows[1],
		rows[2] + m.rows[2]
	);
}

template <typename T> Matrix3<T> Matrix3<T>::operator - (const Matrix3<T>& m) const
{
	return Matrix3<T>(
		rows[0] - m.rows[0],
		rows[1] - m.rows[1],
		rows[2] - m.rows[2]
	);
}

template <typename T> Matrix3<T>& Matrix3<T>::operator += (const Matrix3<T>& m)
{
	for (uint32_t i = 0; i < _countof(rows); i++)
	{
		rows[i] += m.rows[i];
	}
	return *this;
}

template <typename T> Matrix3<T>& Matrix3<T>::operator -= (const Matrix3<T>& m)
{
	for (uint32_t i = 0; i < _countof(rows); i++)
	{
		rows[i] -= m.rows[i];
	}
	return *this;
}

template <typename T> Matrix3<T> Matrix3<T>::operator * (const Matrix3<T>& m) const
{
	Matrix3<T> r;
	DOT3(r.rows, rows, m.rows, 0, 0);
	DOT3(r.rows, rows, m.rows, 0, 1);
	DOT3(r.rows, rows, m.rows, 0, 2);
	DOT3(r.rows, rows, m.rows, 1, 0);
	DOT3(r.rows, rows, m.rows, 1, 1);
	DOT3(r.rows, rows, m.rows, 1, 2);
	DOT3(r.rows, rows, m.rows, 2, 0);
	DOT3(r.rows, rows, m.rows, 2, 1);
	DOT3(r.rows, rows, m.rows, 2, 2);
	return r;
}

template <typename T> Matrix3<T>& Matrix3<T>::operator *= (const Matrix3<T>& m)
{
	Matrix3<T> c(*this);
	DOT3(rows, c.rows, m.rows, 0, 0);
	DOT3(rows, c.rows, m.rows, 0, 1);
	DOT3(rows, c.rows, m.rows, 0, 2);
	DOT3(rows, c.rows, m.rows, 1, 0);
	DOT3(rows, c.rows, m.rows, 1, 1);
	DOT3(rows, c.rows, m.rows, 1, 2);
	DOT3(rows, c.rows, m.rows, 2, 0);
	DOT3(rows, c.rows, m.rows, 2, 1);
	DOT3(rows, c.rows, m.rows, 2, 2);
	return *this;
}

template <typename T> Vector3<T> Matrix3<T>::operator * (const Vector3<T>& v) const
{
	return Vector3<T>(
		Vector::Dot<T>(v, rows[0]),
		Vector::Dot<T>(v, rows[1]),
		Vector::Dot<T>(v, rows[2])
	);
}

// ----------------------------------------- Matrix4 ----------------------------------------------

template <typename T> Matrix4<T>::Matrix4() : Matrix4<T>(static_cast<T>(1)) { }
template <typename T> Matrix4<T>::Matrix4(T t)
{
	rows[0] = Vector4<T>(1, 0, 0, 0);
	rows[1] = Vector4<T>(0, 1, 0, 0);
	rows[2] = Vector4<T>(0, 0, 1, 0);
	rows[3] = Vector4<T>(0, 0, 0, 1);
}
template <typename T> Matrix4<T>::Matrix4(const Vector4<T>& v0, const Vector4<T>& v1, const Vector4<T>& v2, const Vector4<T>& v3)
{
	rows[0] = v0;
	rows[1] = v1;
	rows[2] = v2;
	rows[3] = v3;
}
template <typename T> Matrix4<T>::Matrix4(const Matrix4<T>& m) : Matrix4<T>(m[0], m[1], m[2], m[3]) { }

template <typename T> Matrix4<T>& Matrix4<T>::operator = (const Matrix4<T>& m)
{
	for (uint32_t i = 0; i < _countof(rows); i++)
	{
		rows[i] = m.rows[i];
	}
}

template <typename T> Vector4<T>& Matrix4<T>::operator[] (uint32_t i) { return rows[i]; }
template <typename T> const Vector4<T>& Matrix4<T>::operator[] (uint32_t i) const { return rows[i]; }

template <typename T> Matrix4<T> Matrix4<T>::operator * (T t) const
{
	return Matrix4<T>(
		t * rows[0],
		t * rows[1],
		t * rows[2],
		t * rows[3]
	);
}

template <typename T> Matrix4<T>& Matrix4<T>::operator *= (T t)
{
	rows[0] *= t;
	rows[1] *= t;
	rows[2] *= t;
	rows[3] *= t;
	return *this;
}

template <typename T> Matrix4<T> Matrix4<T>::operator + (const Matrix4<T>& m) const
{
	return Matrix4<T>(
		rows[0] + m.rows[0],
		rows[1] + m.rows[1],
		rows[2] + m.rows[2],
		rows[3] + m.rows[3]
	);
}

template <typename T> Matrix4<T> Matrix4<T>::operator - (const Matrix4<T>& m) const
{
	return Matrix4<T>(
		rows[0] - m.rows[0],
		rows[1] - m.rows[1],
		rows[2] - m.rows[2],
		rows[3] - m.rows[3]
	);
}

template <typename T> Matrix4<T>& Matrix4<T>::operator += (const Matrix4<T>& m)
{
	for (uint32_t i = 0; i < _countof(rows); i++)
	{
		rows[i] += m.rows[i];
	}
	return *this;
}

template <typename T> Matrix4<T>& Matrix4<T>::operator -= (const Matrix4<T>& m)
{
	for (uint32_t i = 0; i < _countof(rows); i++)
	{
		rows[i] -= m.rows[i];
	}
	return *this;
}

template <typename T> Matrix4<T> Matrix4<T>::operator * (const Matrix4<T>& m) const
{
	Matrix4<T> r;
	DOT4(r.rows, rows, m.rows, 0, 0);
	DOT4(r.rows, rows, m.rows, 0, 1);
	DOT4(r.rows, rows, m.rows, 0, 2);
	DOT4(r.rows, rows, m.rows, 0, 3);
	DOT4(r.rows, rows, m.rows, 1, 0);
	DOT4(r.rows, rows, m.rows, 1, 1);
	DOT4(r.rows, rows, m.rows, 1, 2);
	DOT4(r.rows, rows, m.rows, 1, 3);
	DOT4(r.rows, rows, m.rows, 2, 0);
	DOT4(r.rows, rows, m.rows, 2, 1);
	DOT4(r.rows, rows, m.rows, 2, 2);
	DOT4(r.rows, rows, m.rows, 2, 3);
	DOT4(r.rows, rows, m.rows, 3, 0);
	DOT4(r.rows, rows, m.rows, 3, 1);
	DOT4(r.rows, rows, m.rows, 3, 2);
	DOT4(r.rows, rows, m.rows, 3, 3);
	return r;
}

template <typename T> Matrix4<T>& Matrix4<T>::operator *= (const Matrix4<T>& m)
{
	Matrix4<T> c(*this);
	DOT4(rows, c.rows, m.rows, 0, 0);
	DOT4(rows, c.rows, m.rows, 0, 1);
	DOT4(rows, c.rows, m.rows, 0, 2);
	DOT4(rows, c.rows, m.rows, 0, 3);
	DOT4(rows, c.rows, m.rows, 1, 0);
	DOT4(rows, c.rows, m.rows, 1, 1);
	DOT4(rows, c.rows, m.rows, 1, 2);
	DOT4(rows, c.rows, m.rows, 1, 3);
	DOT4(rows, c.rows, m.rows, 2, 0);
	DOT4(rows, c.rows, m.rows, 2, 1);
	DOT4(rows, c.rows, m.rows, 2, 2);
	DOT4(rows, c.rows, m.rows, 2, 3);
	DOT4(rows, c.rows, m.rows, 3, 0);
	DOT4(rows, c.rows, m.rows, 3, 1);
	DOT4(rows, c.rows, m.rows, 3, 2);
	DOT4(rows, c.rows, m.rows, 3, 3);
	return *this;
}

template <typename T> Vector4<T> Matrix4<T>::operator * (const Vector4<T>& v) const
{
	return Vector4<T>(
		Vector::Dot<T>(v, rows[0]),
		Vector::Dot<T>(v, rows[1]),
		Vector::Dot<T>(v, rows[2]),
		Vector::Dot<T>(v, rows[3])
	);
}

// ------------------------------------ Matrix functions ------------------------------------------

template <typename T> Matrix2<T> Matrix::Inverse(const Matrix2<T>& m)
{
	Matrix2<T> r;
	T det = static_cast<T>(1) / DET2(m[0][0], m[0][1], m[1][0], m[1][1]);
	
	r[0][0] = m[1][1];
	r[1][1] = m[0][0];
	r[0][1] = -m[0][1];
	r[1][0] = -m[1][0];

	return (r * det);
}

template <typename T> Matrix3<T> Matrix::Inverse(const Matrix3<T>& m)
{
	Matrix3<T> r;
	T det = static_cast<T>(1) / DET3(m[0][0], m[0][1], m[0][2], m[1][0], m[1][1], m[1][2], m[2][0], m[2][1], m[2][2]);
	
	r[0][0] = +DET2(m[1][1], m[1][2], m[2][1], m[2][2]);
	r[1][0] = -DET2(m[1][0], m[1][2], m[2][0], m[2][2]);
	r[2][0] = +DET2(m[1][0], m[1][1], m[2][0], m[2][1]);
	r[0][1] = -DET2(m[0][1], m[0][2], m[2][1], m[2][2]);
	r[1][1] = +DET2(m[0][0], m[0][2], m[2][0], m[2][2]);
	r[2][1] = -DET2(m[0][0], m[0][1], m[2][0], m[2][1]);
	r[0][2] = +DET2(m[0][1], m[0][2], m[1][1], m[1][2]);
	r[1][2] = -DET2(m[0][0], m[0][2], m[1][0], m[1][2]);
	r[2][2] = +DET2(m[0][0], m[0][1], m[1][0], m[1][1]);

	return (r * det);
}

template <typename T> Matrix4<T> Matrix::Inverse(const Matrix4<T>& m)
{
	Matrix4F r;

	float m0 = m[0][0] * DET3(m[1][1], m[1][2], m[1][3], m[2][1], m[2][2], m[2][3], m[3][1], m[3][2], m[3][3]);
	float m1 = m[0][1] * DET3(m[1][0], m[1][2], m[1][3], m[2][0], m[2][2], m[2][3], m[3][0], m[3][2], m[3][3]);
	float m2 = m[0][2] * DET3(m[1][0], m[1][1], m[1][3], m[2][0], m[2][1], m[2][3], m[3][0], m[3][1], m[3][3]);
	float m3 = m[0][3] * DET3(m[1][0], m[1][1], m[1][2], m[2][0], m[2][1], m[2][2], m[3][0], m[3][1], m[3][2]);
	T det = static_cast<T>(1) / (m0 - m1 + m2 - m3);

	r[0][0] = +DET3(m[1][1], m[1][2], m[1][3], m[2][1], m[2][2], m[2][3], m[3][1], m[3][2], m[3][3]);
	r[1][0] = -DET3(m[1][0], m[1][2], m[1][3], m[2][0], m[2][2], m[2][3], m[3][0], m[3][2], m[3][3]);
	r[2][0] = +DET3(m[1][0], m[1][1], m[1][3], m[2][0], m[2][1], m[2][3], m[3][0], m[3][1], m[3][3]);
	r[3][0] = -DET3(m[1][0], m[1][1], m[1][2], m[2][0], m[2][1], m[2][2], m[3][0], m[3][1], m[3][2]);

	r[0][1] = -DET3(m[0][1], m[0][2], m[0][3], m[2][1], m[2][2], m[2][3], m[3][1], m[3][2], m[3][3]);
	r[1][1] = +DET3(m[0][0], m[0][2], m[0][3], m[2][0], m[2][2], m[2][3], m[3][0], m[3][2], m[3][3]);
	r[2][1] = -DET3(m[0][0], m[0][1], m[0][3], m[2][0], m[2][1], m[2][3], m[3][0], m[3][1], m[3][3]);
	r[3][1] = +DET3(m[0][0], m[0][1], m[0][2], m[2][0], m[2][1], m[2][2], m[3][0], m[3][1], m[3][2]);

	r[0][2] = +DET3(m[0][1], m[0][2], m[0][3], m[1][1], m[1][2], m[1][3], m[3][1], m[3][2], m[3][3]);
	r[1][2] = -DET3(m[0][0], m[0][2], m[0][3], m[1][0], m[1][2], m[1][3], m[3][0], m[3][2], m[3][3]);
	r[2][2] = +DET3(m[0][0], m[0][1], m[0][3], m[1][0], m[1][1], m[1][3], m[3][0], m[3][1], m[3][3]);
	r[3][2] = -DET3(m[0][0], m[0][1], m[0][2], m[1][0], m[1][1], m[1][2], m[3][0], m[3][1], m[3][2]);

	r[0][3] = -DET3(m[0][1], m[0][2], m[0][3], m[1][1], m[1][2], m[1][3], m[2][1], m[2][2], m[2][3]);
	r[1][3] = +DET3(m[0][0], m[0][2], m[0][3], m[1][0], m[1][2], m[1][3], m[2][0], m[2][2], m[2][3]);
	r[2][3] = -DET3(m[0][0], m[0][1], m[0][3], m[1][0], m[1][1], m[1][3], m[2][0], m[2][1], m[2][3]);
	r[3][3] = +DET3(m[0][0], m[0][1], m[0][2], m[1][0], m[1][1], m[1][2], m[2][0], m[2][1], m[2][2]);

	return (r * det);
}

template <typename T> Matrix2<T> Matrix::Transpose(const Matrix2<T>& m)
{
	return Matrix2<T> {
		{ m[0][0], m[1][0] },
		{ m[0][1], m[1][1] }
	};
}

template <typename T> Matrix3<T> Matrix::Transpose(const Matrix3<T>& m)
{
	return Matrix3<T> {
		{ m[0][0], m[1][0], m[2][0] },
		{ m[0][1], m[1][1], m[2][1] },
		{ m[0][2], m[1][2], m[2][2] }
	};
}

template <typename T> Matrix4<T> Matrix::Transpose(const Matrix4<T>& m)
{
	return Matrix4<T> {
		{ m[0][0], m[1][0], m[2][0], m[3][0] },
		{ m[0][1], m[1][1], m[2][1], m[3][1] },
		{ m[0][2], m[1][2], m[2][2], m[3][2] },
		{ m[0][3], m[1][3], m[2][3], m[3][3] }
	};
}

// ------------------------- Matrix template/function instantiations ------------------------------

#define INSTANTIATE_MATRIX_TEMPLATES(X)                                                 \
    template struct Matrix2<X>;                                                         \
    template struct Matrix2<X>;                                                         \
    template struct Matrix2<X>;                                                         \

#define INSTANTIATE_MATRIX_FUNCTION_TEMPLATES(X)                                        \
	template Matrix2<X> Matrix::Inverse(const Matrix2<X>& m);							\
	template Matrix3<X> Matrix::Inverse(const Matrix3<X>& m);							\
	template Matrix4<X> Matrix::Inverse(const Matrix4<X>& m);							\
	template Matrix2<X> Matrix::Transpose(const Matrix2<X>& m);							\
	template Matrix3<X> Matrix::Transpose(const Matrix3<X>& m);							\
	template Matrix4<X> Matrix::Transpose(const Matrix4<X>& m);							\

INSTANTIATE_MATRIX_TEMPLATES(float);
