#ifndef WriteData_h__
#define WriteData_h__

#include "WriteBase.h"

template<typename T>
class WriteData :public WriteBase<T>
{
public:
	WriteData(void) : WriteBase("data.te") {}
	void Write(T, T, T);
};

template<typename T>
void WriteData<T>::Write(T x, T y, T z)
{
	if (!m_out) return;
	std::string line;
	std::ostringstream oss(line);
	oss << x << " " << y << " " << z << "\n";
	std::string record = oss.str();
	m_out.write(record.c_str(), record.length());
}

#endif