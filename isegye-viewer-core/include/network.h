// Copyright 2024 kus-system-security-team-1
#ifndef INCLUDE_NETWORK_H_
#define INCLUDE_NETWORK_H_

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <Windows.h>
#include <tchar.h>
#include <processthreadsapi.h>
#include <psapi.h>
#include <winbase.h>
#include <fileapi.h>
#define NTDDI_VERSION NTDDI_WIN7
#define _WIN32_WINNT 0x0601
#define WINVER 0x0601
#include <windows.h>
#include <fwpmu.h>
#include <stdlib.h>
#pragma comment(lib,"fwpuclnt.lib")

namespace py = pybind11;

class Network {
public:
	Network();
	int blockProcessTraffic(DWORD pid);
	int unblockProcessTraffic(DWORD pid);
};

#endif // INCLUDE_NETWORK_H_
