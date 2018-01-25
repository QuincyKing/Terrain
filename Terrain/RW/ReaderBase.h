#ifndef ReaderBase_h__
#define ReaderBase_h__

#include <string>
#include <vector>
#include "../TerrainModel.h"
#include "../BaseData.h"
#include <fstream>
#include <sstream>

template<typename T>
class ReaderBase
{
public:
	ReaderBase() {};
	ReaderBase(std::string filename):m_filename(filename) {}
	virtual T GetData() { return  m_data; }
	virtual void Read() = 0;

protected:
	T  m_data;
	std::string m_filename;
};
#endif