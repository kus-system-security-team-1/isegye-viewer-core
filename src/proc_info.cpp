// Copyright 2024 kus-system-security-team-1
#include "..\include\proc_info.h"

BasicProcInfo::BasicProcInfo() {
}

BasicProcInfo::~BasicProcInfo() {
}

std::vector<DWORD> BasicProcInfo::get_all_processes() {
    if (get_all_processes_impl()) {
        std::vector<DWORD> vec(this->processes_, this->processes_ + MAX_PROC_CNT);
        delete this->processes_;
        return vec;
    }
    else {
        std::vector<DWORD> vec(0);
        delete this->processes_;
        return vec;
    }
}

std::wstring BasicProcInfo::get_process_name(DWORD pid) {
    HANDLE proc_handle = OpenProcess(
        PROCESS_QUERY_LIMITED_INFORMATION,
        FALSE,
        pid
    );

    if (proc_handle) {
        WCHAR buffer[MAX_PATH] = {};
        DWORD buffer_size = MAX_PATH;
        if (QueryFullProcessImageNameW(proc_handle, 0, buffer, &buffer_size)) {
            return std::wstring(buffer);
        }
        CloseHandle(proc_handle);
    }
    return nullptr;
}

WCHAR* BasicProcInfo::get_process_owner(DWORD pid) {
}

SIZE_T BasicProcInfo::get_virtual_mem_usage(DWORD pid) {
}

double BasicProcInfo::get_current_cpu_usage(DWORD pid) {
}

_IO_COUNTERS BasicProcInfo::get_disk_io(DWORD pid) {
}

BOOL BasicProcInfo::restart_process_by_pid(DWORD pid) {
    HANDLE proc_handle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_TERMINATE | PROCESS_VM_READ, FALSE, pid);
    if (proc_handle == NULL) {
        return FALSE;
    }

    WCHAR proc_path[MAX_PATH] = { 0 };
    if (GetModuleFileNameExW(proc_handle, NULL, proc_path, MAX_PATH) == 0) {
        CloseHandle(proc_handle);
        return FALSE;
    }

    if (!TerminateProcess(proc_handle, 0)) {
        CloseHandle(proc_handle);
        return FALSE;
    }
    CloseHandle(proc_handle);

    STARTUPINFOW startupInfo = { 0 };
    startupInfo.cb = sizeof(startupInfo);
    PROCESS_INFORMATION processInfo = { 0 };

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
        return FALSE;
    }

    CloseHandle(processInfo.hProcess);
    CloseHandle(processInfo.hThread);
    return TRUE;
}

BOOL BasicProcInfo::get_all_processes_impl() {
    DWORD cb_needed;
    this->processes_ = new DWORD[MAX_PROC_CNT];
    memset(this->processes_, 0, sizeof(DWORD) * MAX_PROC_CNT);
    DWORD buffer_size = MAX_PROC_CNT * sizeof(DWORD);

    if (!EnumProcesses(this->processes_, buffer_size, &cb_needed)) {
        return 0;
    }

    return true;
}

PYBIND11_MODULE(isegye_viewer_core_proc_info, m) {
    py::class_<BasicProcInfo>(m, "BasicProcInfo")
        .def(py::init())
        .def("get_all_processes", &BasicProcInfo::get_all_processes)
        .def("restart_process_by_pid", &BasicProcInfo::restart_process_by_pid)
        .def("get_process_name", &BasicProcInfo::get_process_name);
#ifdef VERSION_INFO
    m.attr("__version__") = VERSION_INFO;
#else
    m.attr("__version__") = "dev";
#endif
}
