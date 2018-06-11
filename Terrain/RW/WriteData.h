#ifndef WriteData_h__
#define WriteData_h__

#include "WriteBase.h"

template<typename T>
class WriteData :public WriteBase<T>
{
public:
	WriteData(void) : WriteBase("./data.te") {}
	void Execute(T, T, T);
};

template<typename T>
void WriteData<T>::Execute(T x, T y, T z)
{
	if (!mOut) return;
	std::string line;
	std::ostringstream oss(line);
	oss << x << " " << y << " " << z << "\n";
	std::string record = oss.str();
	mOut.write(record.c_str(), record.length());
}

#endif