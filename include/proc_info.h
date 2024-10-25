//Copyright 2024 kus-system-security-team-1
#ifndef INCLUDE_PROC_INFO_H_
#define INCLUDE_PROC_INFO_H_

#include <Windows.h>
#include <WinBase.h>
#include <aclapi.h>
#include <vector>
#include <sddl.h>
#include <Lmcons.h>
#include <debugapi.h>
#include <string>
#include <cstdint>
#include <processthreadsapi.h>
#include <iphlpapi.h>
#include <Pdh.h>
#include <winnt.h>
#include <tchar.h>
#include <psapi.h>
#pragma comment(lib, "psapi.lib")
#include <securitybaseapi.h>
#include <string>
#include <iostream>
#include <vector>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

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
private:

public:
	BasicProcInfo();
	~BasicProcInfo();
	// 현재 실행되고 있는 모든 프로세스의 pid를 배열로 가져옴. 최대 크기 1024
	std::vector<DWORD> get_all_processes();
	std::wstring get_process_name(DWORD pid);
	std::wstring get_process_owner(DWORD pid);
	SIZE_T get_virtual_mem_usage(DWORD pid);
	double get_current_cpu_usage(DWORD pid);
	DiskIOCounters get_disk_io(DWORD pid);
	// pid로 프로세스 경로 검색 후, 종료 후 재 시작
	BOOL restart_process_by_pid(DWORD pid);
};

#endif // INCLUDE_PROC_INFO_H_