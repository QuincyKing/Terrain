#ifndef TERRAINMODEL_H
#define TERRAINMODEL_H

#include "BaseData.h"
#include <vector>
#include <map>
#include <cassert>
using namespace std;

//点集
class Vertices
{
public:
	Vertices(void) = default;
	Vertices(const Vertices &ver)
	{
		m_vertices.clear();
		m_vertices.insert(m_vertices.begin(), ver.m_vertices.begin(),
											  ver.m_vertices.end());
	}

	Vertices& operator= (const Vertices &other)
	{
		assert(this != &other);
		m_vertices = other.m_vertices;
		return *this;
	}

	void push(Point3D data)
	{
		m_vertices.push_back(data);
	}

	Point3D& operator[] (size_t index)
	{
		return m_vertices.at(index);
	}

	void clear()
	{
		m_vertices.clear();
	}

public:
	vector<Point3D> m_vertices;
};

//三角形集
class TriangleSet
{
public:
	TriangleSet(void) = default;
	TriangleSet(const TriangleSet &tripset)
	{
		m_triangleSet.clear();
		m_triangleSet.insert(m_triangleSet.begin(), tripset.m_triangleSet.begin(),
													tripset.m_triangleSet.end());
	}

	TriangleSet& operator= (const TriangleSet &other)
	{
		assert(this != &other);
		m_triangleSet = other.m_triangleSet;
		return *this;
	}

	void push(Triangle data)
	{
		m_triangleSet.push_back(data);
	}
	Triangle& operator[] (size_t index)
	{
		return m_triangleSet.at(index);
	}

	void clear()
	{
		m_triangleSet.clear();
	}
public:
	vector<Triangle> m_triangleSet;
};

//GTP集
class GTPSet
{
public:
	GTPSet(void) = default;
	GTPSet(const GTPSet &gtpset)
	{
		m_GTPSet.clear();
		m_GTPSet.insert(gtpset.m_GTPSet.begin(), gtpset.m_GTPSet.end());
	}

	GTPSet& operator= (const GTPSet &other)
	{
		assert(this != &other);
		m_GTPSet = other.m_GTPSet;
		return *this;
	}

	void push(GTP _gtp, int level)
	{
		m_GTPSet.insert(pair<int, GTP>(level, _gtp));
	}
	
	void clear()
	{
		m_GTPSet.clear();
	}
public:
	multimap<int, GTP> m_GTPSet;
};

//地形集合
class TerrainModel
{
public:
	int					m_x;
	int					m_y;
	int					m_z;
	Vertices			m_ver;
	TriangleSet			m_ts;
	GTPSet				m_gtp;

public:
	TerrainModel(void) = default;

	TerrainModel(int x, int y, int z, Vertices ver, TriangleSet ts, GTPSet gtp)
		:m_x(x), m_y(y), m_z(z)
	{
		m_ver = ver;
		m_ts = ts;
		m_gtp = gtp;
	}

	TerrainModel(const TerrainModel &terrainmodel)
		:m_x(terrainmodel.m_x), m_y(terrainmodel.m_y), m_z(terrainmodel.m_z)
	{
		m_ver = terrainmodel.m_ver;
		m_ts = terrainmodel.m_ts;
		m_gtp = terrainmodel.m_gtp;
	}

	TerrainModel& operator= (const TerrainModel &other)
	{
		assert(this != &other);
		m_x = other.m_x;
		m_y = other.m_y;
		m_z = other.m_z;
		m_ver = other.m_ver;
		m_ts = other.m_ts;
		m_gtp = other.m_gtp;
		return *this;
	}

	void clear()
	{
		m_x = 0;
		m_y = 0;
		m_z = 0;
		m_ver.clear();
		m_ts.clear();
		m_gtp.clear();
	}
};

#endif