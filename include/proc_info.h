//Copyright 2024 kus-system-security-team-1
#ifndef INCLUDE_PROC_INFO_H_
#define INCLUDE_PROC_INFO_H_

#include <aclapi.h>
#include <algorithm>
#include <cstdint>
#include <debugapi.h>
#include <iphlpapi.h>
#include <iostream>
#include <Lmcons.h>
#include <Pdh.h>
#include <processthreadsapi.h>
#include <psapi.h>
#pragma comment(lib, "psapi.lib")
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <sddl.h>
#include <securitybaseapi.h>
#include <string>
#include <tchar.h>
#include <vector>
#include <WinBase.h>
#include <Windows.h>
#include <winnt.h>

#define MAX_USER_LEN 256
#define MAX_PROC_CNT 1024

namespace py = pybind11;

struct DiskIOCounters {
	uint64_t ReadOperationCount;
	uint64_t WriteOperationCount;
	uint64_t OtherOperationCount;
	uint64_t ReadTransferCount;
	uint64_t WriteTransferCount;
	uint64_t OtherTransferCount;
};

class BasicProcInfo {
public:
	BasicProcInfo();
	~BasicProcInfo();

	std::vector<DWORD> get_all_processes();
	std::wstring get_process_name(DWORD pid);
	std::wstring get_process_owner(DWORD pid);
	SIZE_T get_virtual_mem_usage(DWORD pid);
	double get_current_cpu_usage(DWORD pid);
	DiskIOCounters get_disk_io(DWORD pid);
	BOOL restart_process_by_pid(DWORD pid);
	std::vector<std::wstring> get_process_modules(DWORD pid);
	bool is_module_loaded(DWORD pid, const std::wstring& module_name);
};

#endif // INCLUDE_PROC_INFO_H_