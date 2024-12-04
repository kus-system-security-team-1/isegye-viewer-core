// Copyright 2024 kus-system-security-team-1
#ifndef INCLUDE_PE_H_
#define INCLUDE_PE_H_

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <Windows.h>
#include <tchar.h>
#include <processthreadsapi.h>
#include <psapi.h>
#include <winbase.h>
#include <fileapi.h>

namespace py = pybind11;

enum DetectEntropyType {
	HIGH,
	MIDDLE,
	LOW
};

class PE {
public:
	PE();
	double calculateEntropy(WCHAR *pe_name);
	DetectEntropyType detectEntropyLevel(double entropy);
	void WriteFileTimeToLog(FILETIME ft, FILE* logFile);
};
#endif // INCLUDE_PE_H_
