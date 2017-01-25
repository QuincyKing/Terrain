#ifndef ReaderInfo_h__
#define ReaderInfo_h__

#include "ReaderBase.h"
#include "../BaseData.h"

template<typename T = Info>
class ReaderInfo : public ReaderBase<T>
{
public:
	ReaderInfo(std::string filename = "info.te") :ReaderBase(filename) { Read(); }
	bool HasData();
	void Read();
};

template<typename T>
void ReaderInfo<T>::Read()
{
	std::string line;
	std::ifstream in(m_filename.c_str());
	if (!in) return;
	int countx, county, countz;
	while (!in.eof())
	{
		getline(in, line);
		std::istringstream iss(line);
		iss >> countx >> county >> countz;
		m_data.Put(countx, county, countz);
	}
	return;
}

template<typename T>
bool ReaderInfo<T>::HasData()
{
	std::ifstream in(m_filename.c_str());
	string line;
	getline(in, line);
	if (!in || in.eof()) return false;
	return true;
}

#endif