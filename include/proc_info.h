#ifndef ISEGYE_VIEWER_CORE_PROC_INFO_H_
#define ISEGYE_VIEWER_CORE_PROC_INFO_H_

#include <Windows.h>
#include <WinBase.h>
#include <sddl.h>
#include <processthreadsapi.h>
#include <iphlpapi.h>
#include <Pdh.h>
#include <winnt.h>
#include <tchar.h>
#include <PSApi.h>
#include <Securitybaseapi.h>

#include <string>
#include <iostream>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#define MAX_USER_LEN 256
#define MAX_PROC_CNT 1024
namespace py = pybind11;

class BasicProcInfo {
private:
	DWORD *processes_;
	BOOL get_all_processes_impl();
public:
	BasicProcInfo();
	~BasicProcInfo();
	// ���� ����ǰ� �ִ� ��� ���μ����� pid�� �迭�� ������. �ִ� ũ�� 1024
	std::vector<DWORD> get_all_processes();
	std::wstring get_process_name(DWORD pid);
	WCHAR *get_process_owner(DWORD pid);
	SIZE_T get_virtual_mem_usage(DWORD pid);
	double get_current_cpu_usage(DWORD pid);
	_IO_COUNTERS get_disk_io(DWORD pid);
	// pid�� ���μ��� ��� �˻� ��, ���� �� �� ����
	BOOL restart_process_by_pid(DWORD pid);
};

#endif