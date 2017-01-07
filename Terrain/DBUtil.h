#ifndef DBUTIL_H
#define DBUTIL_H

#include "BaseData.h"
#include "TerrainModel.h"
#include <vector>
#include <cstdio>
#import "C:\Program Files\Common Files\System\ado\msado15.dll" no_namespace rename("EOF", "adoEOF")
using namespace std;

class DBUtil
{
  private:
	_ConnectionPtr  m_pConnection;
	_RecordsetPtr	m_pRecordset;
	HRESULT  hr;
	void _ReadData(Info &Data);
	void _ReadData(Vertices &Data);
	void _ReadData(vector<vector<Point3D> > &Data);
 public:
	 DBUtil();
	 ~DBUtil();
	 template<typename T>
	 void ReadData(string DBname, T &Data);
	 void SQLQuery(char sql[100]);
	 bool IsData(string DBname);
};

DBUtil::DBUtil()
{
	CoInitialize(NULL);
	if (::CoInitialize(NULL))
		::CoUninitialize();

	hr = m_pConnection.CreateInstance(__uuidof(Connection));
	if (FAILED(hr))
	{
		MessageBox(NULL, LPCWSTR("log"), LPCWSTR("failed!"), NULL);
	}
	else
	{
		try
		{
			_bstr_t strConnect = "Driver={sql server};server=127.0.0.1,1433;uid=sa;pwd=13396731991;database=Terrain;";
			m_pConnection->Open(strConnect, "", "", adModeUnknown);
		}
		catch (_com_error e)
		{
			MessageBox(NULL, LPCWSTR("log"), LPCWSTR("failed!!"), NULL);
		}
	}
}

DBUtil::~DBUtil()
{
	if (m_pRecordset != NULL)
	{
		m_pRecordset->Close();
	}
	m_pConnection->Close();
	::CoUninitialize();
}

template<typename T>
void DBUtil::ReadData(string DBname, T &Data)
{
	hr = m_pRecordset.CreateInstance(__uuidof(Recordset));
	if (FAILED(hr))
	{
		MessageBox(NULL, LPCWSTR("log"), LPCWSTR("failed!!!"), NULL);
	}
	try
	{
		char sql[50];
		sprintf_s(sql, sizeof(sql), "SELECT * FROM %s", DBname.c_str());
		m_pRecordset->Open(sql, m_pConnection.GetInterfacePtr(), adOpenDynamic, adLockOptimistic, adCmdText);
	}
	catch (_com_error e)
	{
		MessageBox(NULL, LPCWSTR("log"), LPCWSTR("failed!!!!"), NULL);
	}

	_ReadData(Data);
}

void DBUtil::_ReadData(Info &Data)
{
	int countx, county, countz;
	m_pRecordset->MoveFirst();
	countx = m_pRecordset->Fields->GetItem(_variant_t("count_x"))->Value;
	county = m_pRecordset->Fields->GetItem(_variant_t("count_y"))->Value;
	countz = m_pRecordset->Fields->GetItem(_variant_t("count_z"))->Value;
	Data.Put(countx, county, countz);
}

void DBUtil::_ReadData(vector<vector<Point3D> > &Data)
{
	float x, y, z;
	for (int i = 1; i <= 7; i++)
	{
		m_pRecordset->MoveFirst();
		char str[10];
		sprintf_s(str, sizeof(str), "z%d", i);
		vector<Point3D> level;
		while (!m_pRecordset->adoEOF)
		{
			x = m_pRecordset->Fields->GetItem(_variant_t("x"))->Value;
			y = m_pRecordset->Fields->GetItem(_variant_t("y"))->Value;
			z = m_pRecordset->Fields->GetItem(_variant_t(str))->Value;
			level.push_back(Point3D(x, y, z));
			m_pRecordset->MoveNext();
		}
		Data.push_back(level);
	}
}

void DBUtil::_ReadData(Vertices &Data)
{
	float x, y, z;
	m_pRecordset->MoveFirst();
	while (!m_pRecordset->adoEOF)
	{
		x = m_pRecordset->Fields->GetItem(_variant_t("x"))->Value;
		y = m_pRecordset->Fields->GetItem(_variant_t("y"))->Value;
		z = m_pRecordset->Fields->GetItem(_variant_t("z"))->Value;
		Data.push(Point3D(x, y, z));
		m_pRecordset->MoveNext();
	}
}

void DBUtil::SQLQuery(char sql[100])
{
	hr = m_pRecordset.CreateInstance(__uuidof(Recordset));
	if (FAILED(hr))
	{
		MessageBox(NULL, LPCWSTR("log"), LPCWSTR("failed!!!"), NULL);
	}
	try
	{
		m_pRecordset->Open(sql, m_pConnection.GetInterfacePtr(), adOpenDynamic, adLockOptimistic, adCmdText);
	}
	catch (_com_error e)
	{
		MessageBox(NULL, LPCWSTR("log"), LPCWSTR("failed!!!!"), NULL);
	}
}

bool DBUtil::IsData(string DBname)
{
	int count = 0;
	hr = m_pRecordset.CreateInstance(__uuidof(Recordset));
	if (FAILED(hr))
	{
		MessageBox(NULL, LPCWSTR("log"), LPCWSTR("failed!!!"), NULL);
	}
	try
	{
		char sql[50];
		sprintf_s(sql, sizeof(sql), "SELECT * FROM %s", DBname.c_str());
		m_pRecordset->Open(sql, m_pConnection.GetInterfacePtr(), adOpenDynamic, adLockOptimistic, adCmdText);
	}
	catch (_com_error e)
	{
		MessageBox(NULL, LPCWSTR("log"), LPCWSTR("failed!!!!"), NULL);
	}

	while (!m_pRecordset->adoEOF)
	{
		m_pRecordset->MoveNext();
		count++;
	}
	return count == 0 ? false : true;
}

#endif