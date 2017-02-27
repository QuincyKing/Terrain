#pragma once

#include "Matrix.h"
#include "Numeric.h"
#include "Interpolater.h"
#include "BaseData.h"
#include "RW/RWBuilder.h"
#include <string>
#include <vector>
#include <algorithm>
#include <tchar.h>
#include <iostream>
#include <math.h>
#include <fstream>
#include <sstream>

#pragma region 声明
extern std::vector<double> H;
extern double mm;

template<class ForwardIterator>
double GetDistance(const ForwardIterator start, int i, int j);

template<class ForwardIterator>
double GetDistance(double xpos, double ypos, const ForwardIterator start, int i);

template <class T, class ForwardIterator>
double GetInterpolatedZ(double xpos, double ypos, ForwardIterator first, ForwardIterator last, int m_nSize, TMatrix<T> m_matA, std::vector<int> m_Permutation);

void area(std::vector<double> vec, double& min, double& max);

#pragma endregion

#pragma region 克里金插值类

template<class T, class ForwardIterator1, class ForwardIterator2>
class TKriging : public TInterpolater<ForwardIterator2>
{
public:
	TKriging(std::string TerrainDataFile)
	{
		ReaderTerrainData *temp = nullptr;
		int levels;
		ReaderTerrainData *rtd = new ReaderTerrainData();
		RWBuilder<TerrainData> *TerrainDataReader =
			new RWBuilder<TerrainData>(rtd, TerrainDataFile);
		TerrainDataReader->Execute();
		temp = dynamic_cast<ReaderTerrainData *>(TerrainDataReader->GetRWBase());
		input = temp->GetData();
		temp->GetLevels(levels);
		delete TerrainDataReader;

		H.push_back(0.0);
		for (size_t index = 0; index < input.size() - 1; index++)
		{
			H.push_back(input[index].at(0).z - input[index + 1].at(0).z);
		}
		
		for (size_t mIndex = 0; mIndex < input[0].size(); mIndex++)
		{
			mm += input[levels - 3].at(mIndex).z - input[levels - 2].at(mIndex).z;
		}
		mm = mm / input[0].size();

		ReaderInfo *ri = new ReaderInfo();
		RWBuilder<Info> *InfoReader =
					new RWBuilder<Info>(ri, "./info.te");
		InfoReader->Execute();
		ReaderInfo *infotemp = dynamic_cast<ReaderInfo *>(InfoReader->GetRWBase());
		if (infotemp->HasData())
		{
			return;
		}
		delete InfoReader;

		ForwardIterator1 first1, last1;
		first1 = input.begin();
		last1 = input.end();
		
		std::vector<double> vecx;
		std::vector<double> vecy;
	
		ForwardIterator1 start1 = first1;
		int index_x = 0;
		int countx = 0;
		int county = 0;
		WriteData<double> * wd = new WriteData<double>();
		while (start1 != last1) 
		{
			 int index_y = 0;
			 m_nSize = 0;
			 ForwardIterator2 first2, last2;
			 first2 = start1->begin();
			 last2 = start1->end();
			 ForwardIterator2 start2 = first2;
			 while (start2 != last2)
			 {
				 vecx.push_back(input[index_x][index_y].x);
				 vecy.push_back(input[index_x][index_y].y);
				 ++m_nSize;
				 ++start2;
				 index_y++;
			 }
			 m_matA.SetDimension(m_nSize, m_nSize);
			
			 for (int j = 0; j < m_nSize; j++)
			 {
				for (int i = 0; i < m_nSize; i++) 
				{
					if (i == m_nSize - 1 || j == m_nSize - 1) 
					{
						m_matA(i, j) = 1;
						if (i == m_nSize - 1 && j == m_nSize - 1)
							m_matA(i, j) = 0;
						continue;
					}
					m_matA(i, j) = GetDistance(first2, i, j);
				}
			}
			 
			int nD;
			LUDecompose(m_matA, m_Permutation, nD);
			double x0, y0, x1, y1;
			area(vecx, x0, x1);
			area(vecy, y0, y1);
			int nDiameter = 200;
			int nRadius = nDiameter / 2;
			double xpos = x0, ypos = y0;
			int height = 40;
			int width = 40;
			county = 0;
			for (int i = 0; ypos < (y1 + height); i++)
			{
				countx = 0;
				for (int j = 0; xpos < (x1 + width); j++)
				{
					countx++;
					double z = GetInterpolatedZ(xpos, ypos, (*start1).begin(), (*start1).end(), m_nSize, m_matA, m_Permutation);
					wd->Execute(xpos, ypos, z);
					xpos += width;
				}
				county++;
				xpos = x0;
				ypos += height;
			}
			index_x++;
			++start1;
		}
		WriteInfo<int> * wi = new WriteInfo<int>();
		wi->Execute(countx, county, index_x - 1);
		delete wd;
		delete wi;
	}

private:
	TMatrix<T>							m_matA;
	std::vector<int>					m_Permutation;
	int									m_nSize;
	double								m_dSemivariance;
	std::vector<std::vector<Point3D> >	input;
};

typedef TKriging<double, std::vector<std::vector<Point3D> >::iterator, std::vector<Point3D>::iterator> Kriging;

#pragma endregion

#pragma region 工具实现
template<class ForwardIterator>
double GetDistance(const ForwardIterator start, int i, int j)
{
	return sqrt(pow(((*(start + i)).x - (*(start + j)).x), 2) + pow(((*(start + i)).y - (*(start + j)).y), 2));
}

template<class ForwardIterator>
double GetDistance(double xpos, double ypos, const ForwardIterator start, int i)
{
	return sqrt(pow(((*(start + i)).x - xpos), 2) + pow(((*(start + i)).y - ypos), 2));
}

template <class T, class ForwardIterator>
double GetInterpolatedZ(double xpos, double ypos, ForwardIterator first, ForwardIterator last, int m_nSize, TMatrix<T> m_matA, std::vector<int> m_Permutation) throw(InterpolaterException)
{
	std::vector<double> vecB(m_nSize);

	for (int i = 0; i<m_nSize - 1; i++) {
		double dist = GetDistance(xpos, ypos, first, i);
		vecB[i] = dist;
	}
	vecB[m_nSize - 1] = 1;

	LUBackSub(m_matA, m_Permutation, vecB);

	double z = 0;
	for (int i = 0; i<m_nSize - 1; i++) {
		double inputz = (*(first + i)).z;
		z += vecB[i] * inputz;
	}

	return z;
}

void area(std::vector<double> vec, double& min, double& max)
{
	auto iter = max_element(vec.begin(), vec.end());
	max = *iter;

	iter = min_element(vec.begin(), vec.end());
	min = *iter;
}

#pragma endregion
