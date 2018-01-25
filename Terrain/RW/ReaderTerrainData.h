#ifndef ReaderTerrainData_h__
#define ReaderTerrainData_h__

#include <stdint.h>
#include "ReaderBase.h"
#include "../BaseData.h"

template<typename T = std::vector<std::vector<Point3D> > >
class ReaderTerrainData:public ReaderBase<T>
{
public:
	ReaderTerrainData(std::string filename) :ReaderBase(filename) { Read(); }
	void Read();
	void GetLevels(int &levels);

private:
	std::vector<Point3D> tmp;
};

template<typename T>
void ReaderTerrainData<T>::GetLevels(int &levels)
{
	levels = m_data.size();
}

template<typename T>
void ReaderTerrainData<T>::Read()
{
	std::string line;
	std::vector<double> vecX;
	std::vector<double> vecY;
	std::ifstream in(m_filename.c_str());
	if (!in) return;
	double x, y, z;
	if (!in.eof())
	{
		getline(in, line);
		std::istringstream issX(line);
		while (issX >> x)
		{
			vecX.push_back(x);
		}
	}
	else
	{
	//Log
	}
	if (!in.eof())
	{
		getline(in, line);
		std::istringstream issY(line);
		while (issY >> y)
		{
			vecY.push_back(y);
		}
	}
	else
	{
	//Log
	}


	while (!in.eof())
	{
		tmp.clear();
		uint16_t count = 0;
		getline(in, line);
		std::istringstream issZ(line);
		while (issZ >> z)
		{
			tmp.push_back(Point3D(vecX[count], vecY[count], z));
			count++;
		}
		m_data.push_back(tmp);
	}
	return;
}

#endif