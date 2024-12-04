// Copyright 2024 kus-system-security-team-1
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
#include <userenv.h>
#include <vector>
#include <WinBase.h>
#include <Windows.h>
#include <winnt.h>

#define MAX_USER_LEN 256
#define MAX_PROC_CNT 1024

namespace py = pybind11;

struct DiskIOCounters {
    ULONGLONG read_operation_count;
    ULONGLONG write_operation_count;
    ULONGLONG other_operation_count;
    ULONGLONG read_transfer_count;
    ULONGLONG write_transfer_count;
    ULONGLONG other_transfer_count;
};

class BasicProcInfo {
public:
    BasicProcInfo();
    ~BasicProcInfo();

    std::vector<DWORD> getAllProcesses();
    BOOL restartProcessByPid(DWORD pid);
    BOOL terminateProcessByPid(DWORD pid);
    std::wstring getProcessName(DWORD pid);
    std::wstring getProcessOwner(DWORD pid);
    SIZE_T getVirtualMemUsage(DWORD pid);
    double getCurrentCpuUsage(DWORD pid);
    DiskIOCounters getDiskIo(DWORD pid);
    std::vector<std::wstring> getProcessModules(DWORD pid);
    bool isModuleLoaded(DWORD pid, const std::wstring& module_name);
};
#endif // INCLUDE_PROC_INFO_H_
