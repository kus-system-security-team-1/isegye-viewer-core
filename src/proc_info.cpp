// Copyright 2024 kus-system-security-team-1
#include "..\include\proc_info.h"

BasicProcInfo::BasicProcInfo() {
}

BasicProcInfo::~BasicProcInfo() {
}

std::vector<DWORD> BasicProcInfo::getAllProcesses() {
    std::vector<DWORD> processes(MAX_PROC_CNT);
    DWORD cb_needed;

    size_t process_count = processes.size();
    size_t buffer_size = process_count * sizeof(DWORD);

    if (buffer_size > MAXDWORD) {
        OutputDebugStringW(L"Buffer size exceeds maximum DWORD value.");
        return std::vector<DWORD>();
    }

    if (EnumProcesses(processes.data(), static_cast<DWORD>(buffer_size), &cb_needed)) {
        processes.resize(cb_needed / sizeof(DWORD));
        return processes;
    }
    else {
        OutputDebugStringW(L"Failed to enumerate processes.");
        return std::vector<DWORD>();
    }
}

std::wstring BasicProcInfo::getProcessName(DWORD pid) {
    HANDLE proc_handle = OpenProcess(
        PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_VM_READ,
        FALSE,
        pid
    );

    if (proc_handle) {
        WCHAR buffer[MAX_PATH] = {};
        DWORD buffer_size = MAX_PATH;

        if (QueryFullProcessImageNameW(proc_handle, 0, buffer, &buffer_size)) {
            CloseHandle(proc_handle);
            return std::wstring(buffer);
        }
        else {
            OutputDebugStringW(L"Failed to query process image name.");
        }
        CloseHandle(proc_handle);
    }
    else {
        OutputDebugStringW(L"Failed to open process for getting name.");
    }
    return L"";
}

std::wstring BasicProcInfo::getProcessOwner(DWORD pid) {
    HANDLE proc_handle = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
    if (proc_handle == NULL) {
        OutputDebugStringW(L"Failed to open process for getting owner.");
        return L"";
    }

    HANDLE token_handle;
    if (!OpenProcessToken(proc_handle, TOKEN_QUERY, &token_handle)) {
        OutputDebugStringW(L"Failed to open process token.");
        CloseHandle(proc_handle);
        return L"";
    }

    DWORD token_info_length = 0;
    GetTokenInformation(token_handle, TokenOwner, NULL, 0, &token_info_length);
    if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
        OutputDebugStringW(L"Failed to get token information length.");
        CloseHandle(token_handle);
        CloseHandle(proc_handle);
        return L"";
    }

    std::vector<BYTE> token_info_buffer(token_info_length);
    TOKEN_OWNER* token_owner = reinterpret_cast<TOKEN_OWNER*>(token_info_buffer.data());

    if (!GetTokenInformation(token_handle, TokenOwner, token_owner, token_info_length, &token_info_length)) {
        OutputDebugStringW(L"Failed to get token information.");
        CloseHandle(token_handle);
        CloseHandle(proc_handle);
        return L"";
    }

    WCHAR account_name[UNLEN + 1];
    WCHAR domain_name[DNLEN + 1];
    DWORD account_name_size = UNLEN + 1;
    DWORD domain_name_size = DNLEN + 1;
    SID_NAME_USE sid_type;

    if (!LookupAccountSidW(NULL, token_owner->Owner, account_name, &account_name_size, domain_name, &domain_name_size, &sid_type)) {
        OutputDebugStringW(L"Failed to lookup account SID.");
        CloseHandle(token_handle);
        CloseHandle(proc_handle);
        return L"";
    }

    std::wstring result = std::wstring(domain_name) + L"\\" + std::wstring(account_name);

    CloseHandle(token_handle);
    CloseHandle(proc_handle);

    return result;
}

SIZE_T BasicProcInfo::getVirtualMemUsage(DWORD pid) {
    HANDLE proc_handle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (proc_handle == NULL) {
        OutputDebugStringW(L"Failed to open process for getting memory usage.");
        return 0;
    }

    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(proc_handle, &pmc, sizeof(pmc))) {
        CloseHandle(proc_handle);
        return pmc.WorkingSetSize;
    }
    else {
        OutputDebugStringW(L"Failed to get process memory info.");
    }

    CloseHandle(proc_handle);
    return 0;
}

double BasicProcInfo::getCurrentCpuUsage(DWORD pid) {
    // Open the process to query its CPU usage
    HANDLE proc_handle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (proc_handle == NULL) {
        OutputDebugStringW(L"Failed to open process for getting CPU usage.");
        return 0.0;
    }

    FILETIME ftSysIdle1, ftSysKernel1, ftSysUser1;
    FILETIME ftProcCreation1, ftProcExit1, ftProcKernel1, ftProcUser1;

    // Get system times first measurement
    if (!GetSystemTimes(&ftSysIdle1, &ftSysKernel1, &ftSysUser1)) {
        OutputDebugStringW(L"Failed to get system times (first measurement).");
        CloseHandle(proc_handle);
        return 0.0;
    }

    // Get process times first measurement
    if (!GetProcessTimes(proc_handle, &ftProcCreation1, &ftProcExit1, &ftProcKernel1, &ftProcUser1)) {
        OutputDebugStringW(L"Failed to get process times (first measurement).");
        CloseHandle(proc_handle);
        return 0.0;
    }

    // Wait for a short interval
    Sleep(1000); // 1000 milliseconds (1 second)

    FILETIME ftSysIdle2, ftSysKernel2, ftSysUser2;
    FILETIME ftProcCreation2, ftProcExit2, ftProcKernel2, ftProcUser2;

    // Get system times second measurement
    if (!GetSystemTimes(&ftSysIdle2, &ftSysKernel2, &ftSysUser2)) {
        OutputDebugStringW(L"Failed to get system times (second measurement).");
        CloseHandle(proc_handle);
        return 0.0;
    }

    // Get process times second measurement
    if (!GetProcessTimes(proc_handle, &ftProcCreation2, &ftProcExit2, &ftProcKernel2, &ftProcUser2)) {
        OutputDebugStringW(L"Failed to get process times (second measurement).");
        CloseHandle(proc_handle);
        return 0.0;
    }

    // Convert FILETIME to ULONGLONG
    auto ConvertFileTimeToULL = [](const FILETIME& ft) -> ULONGLONG {
        return ((ULONGLONG)ft.dwHighDateTime << 32) | ft.dwLowDateTime;
    };

    ULONGLONG sys_idle1 = ConvertFileTimeToULL(ftSysIdle1);
    ULONGLONG sys_kernel1 = ConvertFileTimeToULL(ftSysKernel1);
    ULONGLONG sys_user1 = ConvertFileTimeToULL(ftSysUser1);

    ULONGLONG proc_kernel1 = ConvertFileTimeToULL(ftProcKernel1);
    ULONGLONG proc_user1 = ConvertFileTimeToULL(ftProcUser1);

    ULONGLONG sys_idle2 = ConvertFileTimeToULL(ftSysIdle2);
    ULONGLONG sys_kernel2 = ConvertFileTimeToULL(ftSysKernel2);
    ULONGLONG sys_user2 = ConvertFileTimeToULL(ftSysUser2);

    ULONGLONG proc_kernel2 = ConvertFileTimeToULL(ftProcKernel2);
    ULONGLONG proc_user2 = ConvertFileTimeToULL(ftProcUser2);

    // Calculate the differences
    ULONGLONG sys_idle_delta = sys_idle2 - sys_idle1;
    ULONGLONG sys_kernel_delta = sys_kernel2 - sys_kernel1;
    ULONGLONG sys_user_delta = sys_user2 - sys_user1;

    ULONGLONG proc_kernel_delta = proc_kernel2 - proc_kernel1;
    ULONGLONG proc_user_delta = proc_user2 - proc_user1;

    // Calculate total system time delta excluding idle time
    ULONGLONG sys_total = (sys_kernel_delta + sys_user_delta) - sys_idle_delta;

    // Calculate total process time delta
    ULONGLONG proc_total = proc_kernel_delta + proc_user_delta;

    // Get number of processors
    SYSTEM_INFO sys_info;
    GetSystemInfo(&sys_info);
    int num_processors = sys_info.dwNumberOfProcessors;

    double cpu_usage = 0.0;

    if (sys_total > 0 && num_processors > 0) {
        // Normalize CPU usage by number of processors
        cpu_usage = (static_cast<double>(proc_total) * 100.0) / (static_cast<double>(sys_total) * num_processors);
    }

    // Optional: Log the CPU usage value for debugging
    std::wstring debug_message = L"CPU Usage Calculated: " + std::to_wstring(cpu_usage) + L"%";
    OutputDebugStringW(debug_message.c_str());

    CloseHandle(proc_handle);
    return cpu_usage;
}

DiskIOCounters BasicProcInfo::getDiskIo(DWORD pid) {
    HANDLE proc_handle = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
    if (proc_handle == NULL) {
        OutputDebugStringW(L"Failed to open process for getting disk I/O.");
        return DiskIOCounters{};
    }

    IO_COUNTERS io_counters;
    if (GetProcessIoCounters(proc_handle, &io_counters)) {
        CloseHandle(proc_handle);
        DiskIOCounters counters;
        counters.read_operation_count = io_counters.ReadOperationCount;
        counters.write_operation_count = io_counters.WriteOperationCount;
        counters.other_operation_count = io_counters.OtherOperationCount;
        counters.read_transfer_count = io_counters.ReadTransferCount;
        counters.write_transfer_count = io_counters.WriteTransferCount;
        counters.other_transfer_count = io_counters.OtherTransferCount;
        return counters;
    }
    else {
        OutputDebugStringW(L"Failed to get process I/O counters.");
    }

    CloseHandle(proc_handle);
    return DiskIOCounters{};
}

std::vector<std::wstring> BasicProcInfo::getProcessModules(DWORD pid) {
    std::vector<std::wstring> module_list;

    HANDLE h_process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (h_process == NULL) {
        OutputDebugStringW(L"Failed to open process for getting modules.");
        return module_list;
    }

    HMODULE h_mods[1024];
    DWORD cb_needed;

    if (EnumProcessModulesEx(h_process, h_mods, sizeof(h_mods), &cb_needed, LIST_MODULES_ALL)) {
        size_t modules_count = cb_needed / sizeof(HMODULE);
        WCHAR sz_mod_name[MAX_PATH];

        for (size_t i = 0; i < modules_count; i++) {
            if (GetModuleFileNameExW(h_process, h_mods[i], sz_mod_name, sizeof(sz_mod_name) / sizeof(WCHAR))) {
                module_list.push_back(std::wstring(sz_mod_name));
            }
        }
    }
    else {
        OutputDebugStringW(L"Failed to enumerate process modules.");
    }

    CloseHandle(h_process);
    return module_list;
}

bool BasicProcInfo::isModuleLoaded(DWORD pid, const std::wstring& module_name) {
    HANDLE h_process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (h_process == NULL) {
        OutputDebugStringW(L"Failed to open process for checking module.");
        return false;
    }

    HMODULE h_mods[1024];
    DWORD cb_needed;
    bool module_found = false;

    if (EnumProcessModulesEx(h_process, h_mods, sizeof(h_mods), &cb_needed, LIST_MODULES_ALL)) {
        size_t modules_count = cb_needed / sizeof(HMODULE);
        WCHAR sz_mod_name[MAX_PATH];

        for (size_t i = 0; i < modules_count; i++) {
            if (GetModuleFileNameExW(h_process, h_mods[i], sz_mod_name, sizeof(sz_mod_name) / sizeof(WCHAR))) {
                std::wstring mod_name(sz_mod_name);
                // Compare module names case-insensitively
                if (_wcsicmp(mod_name.c_str(), module_name.c_str()) == 0) {
                    module_found = true;
                    break;
                }
            }
        }
    }
    else {
        OutputDebugStringW(L"Failed to enumerate process modules.");
    }

    CloseHandle(h_process);
    return module_found;
}

BOOL BasicProcInfo::restartProcessByPid(DWORD pid) {
    HANDLE proc_handle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_TERMINATE | PROCESS_VM_READ, FALSE, pid);
    if (proc_handle == NULL) {
        OutputDebugStringW(L"Failed to open process for restarting.");
        return FALSE;
    }

    WCHAR proc_path[MAX_PATH] = { 0 };
    if (GetModuleFileNameExW(proc_handle, NULL, proc_path, MAX_PATH) == 0) {
        OutputDebugStringW(L"Failed to get process path.");
        CloseHandle(proc_handle);
        return FALSE;
    }

    if (!TerminateProcess(proc_handle, 0)) {
        OutputDebugStringW(L"Failed to terminate process.");
        CloseHandle(proc_handle);
        return FALSE;
    }
    CloseHandle(proc_handle);

    STARTUPINFOW startup_info = { 0 };
    startup_info.cb = sizeof(startup_info);
    PROCESS_INFORMATION process_info = { 0 };

    if (!CreateProcessW(
        proc_path,
        NULL,
        NULL,
        NULL,
        FALSE,
        0,
        NULL,
        NULL,
        &startup_info,
        &process_info)) {
        OutputDebugStringW(L"Failed to create process.");
        return FALSE;
    }

    CloseHandle(process_info.hProcess);
    CloseHandle(process_info.hThread);
    return TRUE;
}

PYBIND11_MODULE(isegye_viewer_core_proc_info, m) {
    py::class_<BasicProcInfo>(m, "BasicProcInfo")
        .def(py::init())
        .def("getAllProcesses", &BasicProcInfo::getAllProcesses)
        .def("restartProcessByPid", &BasicProcInfo::restartProcessByPid)
        .def("getProcessName", &BasicProcInfo::getProcessName)
        .def("getProcessOwner", &BasicProcInfo::getProcessOwner)
        .def("getVirtualMemUsage", &BasicProcInfo::getVirtualMemUsage)
        .def("getCurrentCpuUsage", &BasicProcInfo::getCurrentCpuUsage)
        .def("getDiskIo", &BasicProcInfo::getDiskIo)
        .def("getProcessModules", &BasicProcInfo::getProcessModules)
        .def("isModuleLoaded", &BasicProcInfo::isModuleLoaded);

    py::class_<DiskIOCounters>(m, "DiskIOCounters")
        .def_readonly("read_operation_count", &DiskIOCounters::read_operation_count)
        .def_readonly("write_operation_count", &DiskIOCounters::write_operation_count)
        .def_readonly("other_operation_count", &DiskIOCounters::other_operation_count)
        .def_readonly("read_transfer_count", &DiskIOCounters::read_transfer_count)
        .def_readonly("write_transfer_count", &DiskIOCounters::write_transfer_count)
        .def_readonly("other_transfer_count", &DiskIOCounters::other_transfer_count);

#ifdef VERSION_INFO
    m.attr("__version__") = VERSION_INFO;
#else
    m.attr("__version__") = "dev";
#endif
}

