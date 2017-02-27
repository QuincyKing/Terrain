#ifndef BASEDATA_H
#define BASEDATA_H

#include <algorithm>
#include <vector>
#include <cstdint>

//点
template<typename T>
class TPoint3D
{
public:
	TPoint3D() : x(0), y(0), z(0), u(0), v(0){}
	TPoint3D(T xx, T yy, T zz, float uu, float vv) : x(xx), y(yy), z(zz), u(uu), v(vv){}
	TPoint3D(T xx, T yy, T zz) : x(xx), y(yy), z(zz), u(0), v(0){}
	TPoint3D(const TPoint3D &TP)
	{
		x = TP.x;
		y = TP.y;
		z = TP.z;
		u = TP.u;
		v = TP.v;
	}
	TPoint3D(const TPoint3D &TP, float _u, float _v)
	{
		x = TP.x;
		y = TP.y;
		z = TP.z;
		u = _u;
		v = _v;
	}
	TPoint3D& operator=(const TPoint3D &TP)
	{
		this->x = TP.x;
		this->y = TP.y;
		this->z = TP.z;
		this->u = TP.u;
		this->v = TP.v;
		return *this;
	}
	void SetX(T x)
	{
		x = T(x);
	}
	void SetY(T y)
	{
		y = T(y);
	}
	void SetZ(T z)
	{
		this->z = T(z);
	}
	void SetU(T u)
	{
		u = T(u);
	}
	void SetV(T v)
	{
		v = T(v);
	}
public:
	T x;
	T y;
	T z;
	typedef T type;
	float u;
	float v;
};

typedef TPoint3D<float> Point3D;

//三角形
class Triangle
{
public:
	Triangle(){}
	Triangle(uint16_t _coordinate1, uint16_t _coordinate2, uint16_t _coordinate3)
	{
		coordinate[0] = _coordinate1;
		coordinate[1] = _coordinate2;
		coordinate[2] = _coordinate3;
	}
	uint16_t operator[] (int index) const
	{
		return coordinate[index];
	}
public:
	uint16_t coordinate[3];
};

//GTP模型
class GTP
{
public:
	GTP(){}
	GTP(uint32_t _triangle1, uint32_t _triangle2, uint32_t _triangle3, uint32_t _triangle4,
		uint32_t _triangle5, uint32_t _triangle6, uint32_t _triangle7, uint32_t _triangle8)
	{
		triangles[0] = _triangle1; triangles[1] = _triangle2;
		triangles[2] = _triangle3; triangles[3] = _triangle4;
		triangles[4] = _triangle5; triangles[5] = _triangle6;
		triangles[6] = _triangle7; triangles[7] = _triangle8;
	}
	GTP(uint32_t *_triangles)
	{
		memcpy(triangles, _triangles, sizeof(triangles));
	}
	uint32_t operator[] (int index) const 
	{
		return triangles[index];
	}
public:
	uint32_t triangles[8];
	static uint8_t GTPPoints;
};
uint8_t GTP::GTPPoints = 24;

//信息
class Info
{
public:
	uint16_t countx;
	uint16_t county;
	uint16_t countz;
public:
	Info() {}
	Info(uint16_t _countx, uint16_t _county, uint16_t _countz) :countx(_countx), county(_county), countz(_countz){ }
	void Put(uint16_t _countx, uint16_t _county, uint16_t _countz)
	{
		countx = _countx;
		county = _county;
		countz = _countz;
	}
};

class Para
{
private:
	std::vector<double> mE;
	std::vector<double> mV;
	std::vector<double> mK;
public:
	Para() {}
	Para(std::vector<double> E, std::vector<double> V, std::vector<double> K)
		:mE(E), mV(V), mK(K)
	{
	}
	void PutE(std::vector<double> E)  {  mE = E; }
	void PutV(std::vector<double> V)  {  mV = V; }
	void PutK(std::vector<double> K)  {  mK = K; }

	std::vector<double> GetE() { return mE; }
	std::vector<double> GetV() { return mV; }
	std::vector<double> GetK() { return mK; }
};
#endif