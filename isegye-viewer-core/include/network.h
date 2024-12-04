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

namespace py = pybind11;

class Network {
public:
	Network();
};

#endif // INCLUDE_NETWORK_H_
