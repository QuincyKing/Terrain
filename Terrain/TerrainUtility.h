#pragma once

#include <Windows.h>
#include <vector>
#include <sstream>
#include <string>
#include <memory>

using std::wstring;
using std::vector;

void PackageParam(LPWSTR information, LPWSTR Pi, LPWSTR Qi, LPWSTR q, LPWSTR Param, LPWSTR TerrainData);

void UnpackageParam(LPWSTR, DOUBLE &, DOUBLE &, DOUBLE &, wstring &, wstring &);

std::vector<wstring> splitManyW(const wstring &original, const wstring &delimiters);