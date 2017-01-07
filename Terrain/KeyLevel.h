#ifndef KeyLevel_h__
#define KeyLevel_h__

#include <vector>
#include <algorithm>
#include <cstdint>

template<typename T>
class KeyLevel
{
public:
	KeyLevel() { }
	KeyLevel(const std::vector<T> &e, const std::vector<T> &h, const std::vector<T> &v);

	std::vector<T>& GetE();
	std::vector<T>& GetH();
	std::vector<T>& GetV();

	void SetE(const std::vector<T>& e);
	void SetH(const std::vector<T>& h);
	void SetV(const std::vector<T>& v);

	void SetEIndex(const uint16_t index, T data);
	void SetHIndex(const uint16_t index, T data);
	void SetVIndex(const uint16_t index, T data);

	uint16_t GetKeyLevel();
	
private:
	std::vector<T> mE;
	std::vector<T> mH;
	std::vector<T> mV;
};

template<typename T>
KeyLevel<T>::KeyLevel(const std::vector<T> &e, const std::vector<T> &h, const std::vector<T> &v) 
{
	mE = e;
	mH = h;
	mV = v;
}

template<typename T>
std::vector<T>& KeyLevel<T>::GetE()
{
	return mE;
}

template<typename T>
std::vector<T>& KeyLevel<T>::GetH()
{
	return mH;
}

template<typename T>
std::vector<T>& KeyLevel<T>::GetV()
{
	return mV;
}

template<typename T>
void KeyLevel<T>::SetE(const std::vector<T>& e)
{
	mE = e;
}

template<typename T>
void KeyLevel<T>::SetH(const std::vector<T>& h)
{
	mH = h;
}

template<typename T>
void KeyLevel<T>::SetV(const std::vector<T>& v)
{
	mV = v;
}

template<typename T>
void KeyLevel<T>::SetEIndex(const uint16_t index, T data)
{
	mE[index] = data;
}

template<typename T>
void KeyLevel<T>::SetHIndex(const uint16_t index, T data)
{
	mH[index] = data;
}

template<typename T>
void KeyLevel<T>::SetVIndex(const uint16_t index, T data)
{
	mV[index] = data;
}

template<typename T>
uint16_t KeyLevel<T>::GetKeyLevel()
{
	T sumHV = 0;
	T sumEH = 0;
	T result;
	std::vector<T> reversemE;
	Reverse(mE, reversemE);
	std::vector<T> reversemH;
	Reverse(mH, reversemH);
	std::vector<T> reversemV;
	Reverse(mV, reversemV);
	uint16_t  saggingZone = 0;
	for (size_t m = 0; m < reversemE.size() - 1; m++)
	{
		result = 0;
		sumHV += reversemH[m] * reversemV[m];
		sumEH += reversemE[m] * std::pow(reversemH[m], 3);

		result = reversemE[m + 1] * reversemH[m + 1] * reversemH[m + 1] * sumHV - reversemV[m + 1] * sumEH;
		if (result > 0)
		{
			saggingZone = m;
			continue;
		}
	}
	return saggingZone;
}

template<typename T>
void Reverse(std::vector<T> &vec, std::vector<T>& arrayRever)
{
	std::vector<T>::reverse_iterator riter;
	for (riter = vec.rbegin(); riter != vec.rend(); riter++)
	{
		arrayRever.push_back(*riter);
	}
}

#endif