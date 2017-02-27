#include "TerrainUtility.h"

std::vector<wstring> splitManyW(const wstring &original, const wstring &delimiters)
{
	std::wstringstream stream(original);
	std::wstring line;
	std::vector <wstring> wordVector;

	while (std::getline(stream, line))
	{
		std::size_t prev = 0, pos;
		while ((pos = line.find_first_of(delimiters, prev)) != std::wstring::npos)
		{
			if (pos > prev)
				wordVector.emplace_back(line.substr(prev, pos - prev));

			prev = pos + 1;
		}

		if (prev < line.length())
			wordVector.emplace_back(line.substr(prev, std::wstring::npos));
	}

	return wordVector;
}

void PackageParam(LPWSTR information, LPWSTR Pi, LPWSTR Qi, LPWSTR q, LPWSTR Param, LPWSTR TerrainData)
{
	LPWSTR tmp = { L"?" };
	wcscat(Pi, tmp);
	wcscat(Qi, tmp);
	wcscat(q, tmp);
	wcscat(Param, tmp);
	wcscat(information, Pi);
	wcscat(information, Qi);
	wcscat(information, q);
	wcscat(information, Param);
	wcscat(information, TerrainData);
}

void UnpackageParam(LPWSTR information, DOUBLE &Pi, DOUBLE &Qi, DOUBLE &q, wstring &Param,wstring &TerrainData)
{
	wstring separators = L"?";
	std::vector<wstring> results = splitManyW(wstring(information), separators);
	Pi = (double)_wtof(results[0].c_str());
	Qi = (double)_wtof(results[1].c_str());
	q = (double)_wtof(results[2].c_str());	
	Param = results[3];
	TerrainData = results[4];
}