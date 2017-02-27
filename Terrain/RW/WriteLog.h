#pragma once

#include "WriteBase.h"
#include <string>
#include <Windows.h>

template<typename T>
class WriteLog : public WriteBase<T>
{
public:
	WriteLog() : WriteBase() {}
	void Execute(T filename, T log);
};

template<typename T>
void WriteLog<T>::Execute(T filename, T log)
{
	SYSTEMTIME sys_time;
	GetLocalTime(&sys_time);

	char szModuleFilePath[MAX_PATH];
	int n = GetModuleFileNameA(0, szModuleFilePath, MAX_PATH);
	szModuleFilePath[strrchr(szModuleFilePath, '\\') - szModuleFilePath + 1] = 0;
	strcat(szModuleFilePath, "\\log.txt");
	mOut.open(szModuleFilePath, std::ofstream::app);
	if (!mOut) return;
	std::string line;
	std::ostringstream oss(line);
	oss << sys_time.wYear << "/" << sys_time.wMonth << "/" << sys_time.wDay << " "
		<< sys_time.wHour << ":" << sys_time.wMinute << ":" << sys_time.wSecond
		<< filename << ":" << log << "\n";
	std::string record = oss.str();
	mOut.write(record.c_str(), record.length());
}

using WriteLogging = WriteLog<string>;