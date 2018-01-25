#ifndef WriteInfo_h__
#define WriteInfo_h__

#include "WriteBase.h"

template<typename T = int>
class WriteInfo : public WriteBase<T>
{
public:
	WriteInfo() :WriteBase("info.te") {}
	void Write(T, T, T);
};

template<typename T>
void WriteInfo<T>::Write(T count_x, T count_y, T count_z)
{
	if (!m_out) return;
	std::string line;
	std::ostringstream oss(line);
	oss << count_x << " " << count_y << " " << count_z << "\n";
	std::string record = oss.str();
	m_out.write(record.c_str(), record.length());
}
#endif