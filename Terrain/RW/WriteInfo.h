#pragma once

#include "WriteBase.h"

template<typename T>
class WriteInfo : public WriteBase<T>
{
public:
	WriteInfo() :WriteBase("./info.te") {}
	void Execute(T, T, T);
};

template<typename T>
void WriteInfo<T>::Execute(T count_x, T count_y, T count_z)
{
	if (!mOut) return;
	std::string line;
	std::ostringstream oss(line);
	oss << count_x << " " << count_y << " " << count_z << "\n";
	std::string record = oss.str();
	mOut.write(record.c_str(), record.length());
}
