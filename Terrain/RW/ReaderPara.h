#ifndef ReaderPara_h__
#define ReaderPara_h__

#include "ReaderBase.h"
#include "../BaseData.h"
#include <vector>

template<typename T=Para>
class ReaderPara:public ReaderBase<T>
{
public:
	ReaderPara(std::string filename) :ReaderBase(filename) { Read(); }
	void Read();
};

template<typename T>
void ReaderPara<T>::Read()
{
	double para;
	std::vector<double> vecE, vecV, vecK;
	std::string line;
	double e, v, k;
	std::ifstream in(m_filename.c_str());
	if (!in) return;
	getline(in, line);
	std::istringstream issE(line);
	while (issE >> e)
	{
		vecE.push_back(e);
	}
	m_data.PutE(vecE);
	if (in.eof())
	{
	//Log
	}

	getline(in, line);
	std::istringstream issV(line);
	while (issV >> v)
	{
		vecV.push_back(v);
	}
	m_data.PutV(vecV);
	if (in.eof())
	{
		//Log
	}

	getline(in, line);
	std::istringstream issK(line);
	while (issK >> k)
	{
		vecK.push_back(k);
	}
	m_data.PutK(vecK);
	return;
}
#endif