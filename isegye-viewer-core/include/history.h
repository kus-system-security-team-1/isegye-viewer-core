// Copyright 2024 kus-system-security-team-1
#ifndef INCLUDE_HISTORY_H_
#define INCLUDE_HISTORY_H_

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <Windows.h>
#include <tchar.h>
#include <processthreadsapi.h>
#include <psapi.h>
#include <winbase.h>
#include <fileapi.h>

namespace py = pybind11;

class History {
public:
	History();
	wchar_t* getAccountNameOfProcess(DWORD pid);
	void WriteFileTimeToLog(FILETIME ft, FILE* logFile);
	bool addProcessLogToFile(DWORD pid);
	bool LogProcessTimesToFile(DWORD pid, WCHAR* logFilePath);
};
#endif // INCLUDE_HISTORY_H_
