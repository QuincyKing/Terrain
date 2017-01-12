#ifndef INPUTREADER_H
#define INPUTREADER_H

#include <string>
#include <vector>
#include "TerrainModel.h"
#include "BaseData.h"
#include "DBUtil.h"
#include <fstream>
#include <sstream>

template<typename T>
class InputReader
{
public:
	InputReader() throw() {}
	InputReader(std::string filename) throw() { Read(filename); }
	const std::vector<std::vector<T> >& Get3DTerrinInfo() const throw() { return m_terrinInfo; }
	void Read(std::string filename) throw();
	std::pair<int, int> InputDataBase();
private:
	std::vector<T>			m_data;
	std::vector<std::vector<T> >	m_terrainInfo;
	void					_Read(std::string &line);
};

template<typename T>
void InputReader<T>::_Read(std::string &line)
{
	T data;
	m_data.clear();
	std::istringstream iss(line);
	while (iss >> data)
	{
		m_data.push_back(data);
	}
}

template<typename T>
void InputReader<T>::Read(std::string filename)
{
	std::string line;
	std::ifstream in(filename.c_str());
	if (!in) return;
	while (!in.eof())
	{
		getline(in, line);
		_Read(line);
		m_terrainInfo.push_back(m_data);
	}
}

template<typename T>
std::pair<int, int> InputReader<T>::InputDataBase()
{
	DBUtil *DBU = new DBUtil();
	DBU->SQLQuery("delete from terrainData");
	DBU->SQLQuery("delete from para");
	char sqlterrain[100];
	char sqlpara[100];
	int cursor = 2;
	for (; cursor != m_terrainInfo.size()-3; cursor++)
	{
		for (int index = 0; index < m_terrainInfo[cursor].size(); index++)
		{
			sprintf_s(sqlterrain, sizeof(sqlterrain), "INSERT INTO terrainData VALUES(%f, %f, %f)", m_terrainInfo[0][index],  m_terrainInfo[1][index], m_terrainInfo[cursor][index]);
			DBU->SQLQuery(sqlterrain);
		}
	}

	auto Ecursor = cursor;
	auto Vcursor = cursor + 1;
	auto Kcursor = cursor + 2;

	for (int index = 0; index < m_terrainInfo[Ecursor].size(); index++)
	{
		sprintf_s(sqlpara, sizeof(sqlpara), "INSERT INTO para VALUES(%f, %f, %f)", m_terrainInfo[Ecursor][index], m_terrainInfo[Vcursor][index], m_terrainInfo[Kcursor][index]);
		DBU->SQLQuery(sqlpara);
	}
	return pair<int, int>(m_terrainInfo.at(Ecursor).size(), m_terrainInfo[0].size());
}
#endif