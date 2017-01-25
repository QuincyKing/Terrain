#ifndef ReaderData_h__
#define ReaderData_h__

#include "ReaderBase.h"
#include "../TerrainModel.h"
#include "../BaseData.h"

template<typename T=Vertices>
class ReaderData :public ReaderBase<T>
{
public:
	ReaderData(std::string filename) :ReaderBase(filename) { Read(); };
	void Read();
};

template<typename T>
void ReaderData<T>::Read()
{
	std::string line;
	std::ifstream in(m_filename.c_str());
	if (!in) return;
	double x, y, z;
	getline(in, line);
	while (line != "")
	{
		std::istringstream iss(line);
		iss >> x >> y >> z;
		m_data.push(Point3D(x, y ,z));
		getline(in, line);
	}
	return;
}

#endif