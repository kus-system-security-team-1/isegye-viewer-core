// Copyright 2024 kus-system-security-team-1
#include "..\include\proc_info.h"

BasicProcInfo::BasicProcInfo() {
}

BasicProcInfo::~BasicProcInfo() {
}

std::vector<DWORD> BasicProcInfo::get_all_processes() {
    std::vector<DWORD> processes(MAX_PROC_CNT);
    DWORD cb_needed;

    size_t process_count = processes.size();
    size_t buffer_size = process_count * sizeof(DWORD);

    if (buffer_size > MAXDWORD) {
        // Handle the error: buffer size exceeds DWORD max value
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

std::wstring BasicProcInfo::get_process_name(DWORD pid) {
    // Open the process with required access rights
    HANDLE proc_handle = OpenProcess(
        PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_VM_READ,
        FALSE,
        pid
    );

    if (proc_handle) {
        WCHAR buffer[MAX_PATH] = {};
        DWORD buffer_size = MAX_PATH;

        // Retrieve the full path of the process executable
        if (QueryFullProcessImageNameW(proc_handle, 0, buffer, &buffer_size)) {
            CloseHandle(proc_handle);
            return std::wstring(buffer);
        }
        else {
            // Log an error message if retrieval fails
            OutputDebugStringW(L"Failed to query process image name.");
        }
        CloseHandle(proc_handle);
    }
    else {
        // Log an error message if opening the process fails
        OutputDebugStringW(L"Failed to open process for getting name.");
    }
    return L"";
}

std::wstring BasicProcInfo::get_process_owner(DWORD pid) {
    // Open the process to query its information
    HANDLE proc_handle = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
    if (proc_handle == NULL) {
        OutputDebugStringW(L"Failed to open process for getting owner.");
        return L"";
    }

    HANDLE token_handle;
    // Open the access token associated with the process
    if (!OpenProcessToken(proc_handle, TOKEN_QUERY, &token_handle)) {
        OutputDebugStringW(L"Failed to open process token.");
        CloseHandle(proc_handle);
        return L"";
    }

    // Determine the size of the buffer needed for token information
    DWORD token_info_length = 0;
    GetTokenInformation(token_handle, TokenOwner, NULL, 0, &token_info_length);
    if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
        OutputDebugStringW(L"Failed to get token information length.");
        CloseHandle(token_handle);
        CloseHandle(proc_handle);
        return L"";
    }

    // Allocate buffer and retrieve token information
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

    // Retrieve the account and domain name associated with the SID
    if (!LookupAccountSidW(NULL, token_owner->Owner, account_name, &account_name_size, domain_name, &domain_name_size, &sid_type)) {
        OutputDebugStringW(L"Failed to lookup account SID.");
        CloseHandle(token_handle);
        CloseHandle(proc_handle);
        return L"";
    }

    // Combine domain and account name to create the full owner name
    std::wstring result = std::wstring(domain_name) + L"\\" + std::wstring(account_name);

    // Close handles to release resources
    CloseHandle(token_handle);
    CloseHandle(proc_handle);

    return result;
}

SIZE_T BasicProcInfo::get_virtual_mem_usage(DWORD pid) {
    // Open the process to query its memory usage
    HANDLE proc_handle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (proc_handle == NULL) {
        OutputDebugStringW(L"Failed to open process for getting memory usage.");
        return 0;
    }

    PROCESS_MEMORY_COUNTERS pmc;
    // Retrieve memory usage information
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

double BasicProcInfo::get_current_cpu_usage(DWORD pid) {
    // Open the process to query its CPU usage
    HANDLE proc_handle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (proc_handle == NULL) {
        OutputDebugStringW(L"Failed to open process for getting CPU usage.");
        return 0.0;
    }

    FILETIME ftSysIdle, ftSysKernel, ftSysUser;
    FILETIME ftProcCreation, ftProcExit, ftProcKernel, ftProcUser;

    // Get system times
    if (!GetSystemTimes(&ftSysIdle, &ftSysKernel, &ftSysUser)) {
        OutputDebugStringW(L"Failed to get system times.");
        CloseHandle(proc_handle);
        return 0.0;
    }

    // Get process times
    if (!GetProcessTimes(proc_handle, &ftProcCreation, &ftProcExit, &ftProcKernel, &ftProcUser)) {
        OutputDebugStringW(L"Failed to get process times.");
        CloseHandle(proc_handle);
        return 0.0;
    }

    static ULONGLONG lastSysKernel = 0, lastSysUser = 0;
    static ULONGLONG lastProcKernel = 0, lastProcUser = 0;

    // Convert FILETIME to ULONGLONG for calculations
    ULONGLONG sysKernel = ((ULONGLONG)ftSysKernel.dwHighDateTime << 32) | ftSysKernel.dwLowDateTime;
    ULONGLONG sysUser = ((ULONGLONG)ftSysUser.dwHighDateTime << 32) | ftSysUser.dwLowDateTime;

    ULONGLONG procKernel = ((ULONGLONG)ftProcKernel.dwHighDateTime << 32) | ftProcKernel.dwLowDateTime;
    ULONGLONG procUser = ((ULONGLONG)ftProcUser.dwHighDateTime << 32) | ftProcUser.dwLowDateTime;

    // Calculate the total system and process times since the last call
    ULONGLONG sysTotal = (sysKernel - lastSysKernel) + (sysUser - lastSysUser);
    ULONGLONG procTotal = (procKernel - lastProcKernel) + (procUser - lastProcUser);

    double cpuUsage = 0.0;
    if (sysTotal > 0) {
        // Calculate CPU usage percentage
        cpuUsage = (procTotal * 100.0) / sysTotal;
    }

    // Update last times for next calculation
    lastSysKernel = sysKernel;
    lastSysUser = sysUser;
    lastProcKernel = procKernel;
    lastProcUser = procUser;

    CloseHandle(proc_handle);
    return cpuUsage;
}

DiskIOCounters BasicProcInfo::get_disk_io(DWORD pid) {
    // Open the process to query its I/O counters
    HANDLE proc_handle = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
    if (proc_handle == NULL) {
        OutputDebugStringW(L"Failed to open process for getting disk I/O.");
        return DiskIOCounters{};  // Return empty struct
    }

    IO_COUNTERS io_counters;
    // Retrieve I/O counters
    if (GetProcessIoCounters(proc_handle, &io_counters)) {
        CloseHandle(proc_handle);
        DiskIOCounters counters;
        counters.ReadOperationCount = io_counters.ReadOperationCount;
        counters.WriteOperationCount = io_counters.WriteOperationCount;
        counters.OtherOperationCount = io_counters.OtherOperationCount;
        counters.ReadTransferCount = io_counters.ReadTransferCount;
        counters.WriteTransferCount = io_counters.WriteTransferCount;
        counters.OtherTransferCount = io_counters.OtherTransferCount;
        return counters;
    }
    else {
        OutputDebugStringW(L"Failed to get process I/O counters.");
    }

    CloseHandle(proc_handle);
    return DiskIOCounters{};  // Return empty struct
}

BOOL BasicProcInfo::restart_process_by_pid(DWORD pid) {
    // Open the process with required access rights to terminate and restart
    HANDLE proc_handle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_TERMINATE | PROCESS_VM_READ, FALSE, pid);
    if (proc_handle == NULL) {
        OutputDebugStringW(L"Failed to open process for restarting.");
        return FALSE;
    }

    WCHAR proc_path[MAX_PATH] = { 0 };
    // Get the executable path of the process
    if (GetModuleFileNameExW(proc_handle, NULL, proc_path, MAX_PATH) == 0) {
        OutputDebugStringW(L"Failed to get process path.");
        CloseHandle(proc_handle);
        return FALSE;
    }

    // Terminate the process
    if (!TerminateProcess(proc_handle, 0)) {
        OutputDebugStringW(L"Failed to terminate process.");
        CloseHandle(proc_handle);
        return FALSE;
    }
    CloseHandle(proc_handle);

    // Initialize structures for creating a new process
    STARTUPINFOW startupInfo = { 0 };
    startupInfo.cb = sizeof(startupInfo);
    PROCESS_INFORMATION processInfo = { 0 };

    // Create a new process using the retrieved executable path
    if (!CreateProcessW(
        proc_path,
        NULL,
        NULL,
        NULL,
        FALSE,
        0,
        NULL,
        NULL,
        &startupInfo,
        &processInfo)) {
        OutputDebugStringW(L"Failed to create process.");
        return FALSE;
    }

    // Close handles to the newly created process and thread
    CloseHandle(processInfo.hProcess);
    CloseHandle(processInfo.hThread);
    return TRUE;
}

std::vector<std::wstring> BasicProcInfo::get_process_modules(DWORD pid) {
    std::vector<std::wstring> module_list;

    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (hProcess == NULL) {
        OutputDebugStringW(L"Failed to open process for getting modules.");
        return module_list;
    }

    HMODULE hMods[1024];
    DWORD cbNeeded;

    if (EnumProcessModulesEx(hProcess, hMods, sizeof(hMods), &cbNeeded, LIST_MODULES_ALL)) {
        size_t modules_count = cbNeeded / sizeof(HMODULE);
        WCHAR szModName[MAX_PATH];

        for (size_t i = 0; i < modules_count; i++) {
            if (GetModuleFileNameExW(hProcess, hMods[i], szModName, sizeof(szModName) / sizeof(WCHAR))) {
                module_list.push_back(std::wstring(szModName));
            }
        }
    }
    else {
        OutputDebugStringW(L"Failed to enumerate process modules.");
    }

    CloseHandle(hProcess);
    return module_list;
}

bool BasicProcInfo::is_module_loaded(DWORD pid, const std::wstring& module_name) {
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (hProcess == NULL) {
        OutputDebugStringW(L"Failed to open process for checking module.");
        return false;
    }

    HMODULE hMods[1024];
    DWORD cbNeeded;
    bool module_found = false;

    if (EnumProcessModulesEx(hProcess, hMods, sizeof(hMods), &cbNeeded, LIST_MODULES_ALL)) {
        size_t modules_count = cbNeeded / sizeof(HMODULE);
        WCHAR szModName[MAX_PATH];

        for (size_t i = 0; i < modules_count; i++) {
            if (GetModuleFileNameExW(hProcess, hMods[i], szModName, sizeof(szModName) / sizeof(WCHAR))) {
                std::wstring modName(szModName);
                if (_wcsicmp(modName.c_str(), module_name.c_str()) == 0) {
                    module_found = true;
                    break;
                }
            }
        }
    }
    else {
        OutputDebugStringW(L"Failed to enumerate process modules.");
    }

    CloseHandle(hProcess);
    return module_found;
}

PYBIND11_MODULE(isegye_viewer_core_proc_info, m) {
    py::class_<BasicProcInfo>(m, "BasicProcInfo")
        .def(py::init())
        .def("get_all_processes", &BasicProcInfo::get_all_processes)
        .def("restart_process_by_pid", &BasicProcInfo::restart_process_by_pid)
        .def("get_process_name", &BasicProcInfo::get_process_name)
        .def("get_process_owner", &BasicProcInfo::get_process_owner)
        .def("get_virtual_mem_usage", &BasicProcInfo::get_virtual_mem_usage)
        .def("get_current_cpu_usage", &BasicProcInfo::get_current_cpu_usage)
        .def("get_disk_io", &BasicProcInfo::get_disk_io)
        .def("get_process_modules", &BasicProcInfo::get_process_modules)
        .def("is_module_loaded", &BasicProcInfo::is_module_loaded);


    py::class_<DiskIOCounters>(m, "DiskIOCounters")
        .def_readonly("ReadOperationCount", &DiskIOCounters::ReadOperationCount)
        .def_readonly("WriteOperationCount", &DiskIOCounters::WriteOperationCount)
        .def_readonly("OtherOperationCount", &DiskIOCounters::OtherOperationCount)
        .def_readonly("ReadTransferCount", &DiskIOCounters::ReadTransferCount)
        .def_readonly("WriteTransferCount", &DiskIOCounters::WriteTransferCount)
        .def_readonly("OtherTransferCount", &DiskIOCounters::OtherTransferCount);

#ifdef VERSION_INFO
    m.attr("__version__") = VERSION_INFO;
#else
    m.attr("__version__") = "dev";
#endif
}
