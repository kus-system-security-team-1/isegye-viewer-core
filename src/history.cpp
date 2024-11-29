// Copyright 2024 kus-system-security-team-1
#include "..\include\history.h"

History::History() {

}

void WriteFileTimeToLog(FILETIME ft, FILE *logFile) {
    SYSTEMTIME st;
    FileTimeToSystemTime(&ft, &st);
    fprintf(logFile, "%04d-%02d-%02d %02d:%02d:%02d\n",
            st.wYear, st.wMonth, st.wDay,
            st.wHour, st.wMinute, st.wSecond);
}

wchar_t *History::getAccountNameOfProcess(DWORD pid) {
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
	if (hProcess == NULL) {
		return NULL;
	}
	HANDLE hToken;
	if (!OpenProcessToken(hProcess, TOKEN_QUERY, &hToken)) {
		CloseHandle(hProcess);
		return NULL;
	}
	DWORD dwLength = 0;
	GetTokenInformation(hToken, TokenUser, NULL, 0, &dwLength);
	PTOKEN_USER pTokenUser = (PTOKEN_USER)GlobalAlloc(GPTR, dwLength);
	if (pTokenUser == NULL) {
		CloseHandle(hToken);
		CloseHandle(hProcess);
		return NULL;
	}
	if (!GetTokenInformation(hToken, TokenUser, pTokenUser, dwLength, &dwLength)) {
		GlobalFree(pTokenUser);
		CloseHandle(hToken);
		CloseHandle(hProcess);
		return NULL;
	}
	CloseHandle(hToken);
	CloseHandle(hProcess);
	SID_NAME_USE SidType;
	wchar_t lpName[256];
	wchar_t lpDomain[256];
	DWORD dwName = 256;
	DWORD dwDomain = 256;
	if (!LookupAccountSidW(NULL, pTokenUser->User.Sid, lpName, &dwName, lpDomain, &dwDomain, &SidType)) {
		GlobalFree(pTokenUser);
		return NULL;
	}
	GlobalFree(pTokenUser);
	return lpName;
}

/* 2.1.2 */
/* TODO: modify write history to logfile logic. it's not worked. */
bool History::addProcessLogToFile(DWORD pid) {
	wchar_t* accountName = getAccountNameOfProcess(pid);
	if (accountName == NULL) {
		return FALSE;
	}
	HANDLE hFile = CreateFileW(L"history.txt", FILE_APPEND_DATA, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		return FALSE;
	}
	DWORD dwWritten;
	WriteFile(hFile, accountName, wcslen(accountName) * sizeof(wchar_t), &dwWritten, NULL);
	WriteFile(hFile, L"\n", sizeof(wchar_t), &dwWritten, NULL);
	CloseHandle(hFile);
	return TRUE;
}

void History::WriteFileTimeToLog(FILETIME ft, FILE* logFile) {
	SYSTEMTIME st;
	FileTimeToSystemTime(&ft, &st);
	fwprintf(logFile, L"%04d-%02d-%02d %02d:%02d:%02d\n",
		st.wYear, st.wMonth, st.wDay,
		st.wHour, st.wMinute, st.wSecond);
}

/* 2.1.1 */
bool History::LogProcessTimesToFile(DWORD pid, WCHAR* logFilePath) {
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
	if (hProcess == NULL) {
		printf("Failed to open process. Error: %lu\n", GetLastError());
		return FALSE;
	}

	FILETIME creationTime, exitTime, kernelTime, userTime;
	if (GetProcessTimes(hProcess, &creationTime, &exitTime, &kernelTime, &userTime)) {
		FILE* logFile = _wfopen(logFilePath, L"a"); // Open in append mode
		if (logFile == NULL) {
			printf("Failed to open log file: %ls\n", logFilePath);
			CloseHandle(hProcess);
			return FALSE;
		}

		fprintf(logFile, "Process ID: %lu\n", pid);

		fprintf(logFile, "Start Time: ");
		WriteFileTimeToLog(creationTime, logFile);

		if (exitTime.dwLowDateTime == 0 && exitTime.dwHighDateTime == 0) {
			fprintf(logFile, "End Time: The process is still running.\n");
		}
		else {
			fprintf(logFile, "End Time: ");
			WriteFileTimeToLog(exitTime, logFile);
		}

		fprintf(logFile, "-----------------------\n");
		fclose(logFile);
		printf("Process times logged to %ls\n", logFilePath);
	}
	else {
		printf("Failed to retrieve process times. Error: %lu\n", GetLastError());
		return FALSE;
	}

	CloseHandle(hProcess);
}

PYBIND11_MODULE(isegye_viewer_core_history, m) {
	py::class_<History>(m, "History")
		.def(py::init())
		.def("getAccountNameOfProcess", &History::getAccountNameOfProcess)
		.def("addProcessLogToFile", &History::addProcessLogToFile)
		.def("LogProcessTimesToFile", &History::LogProcessTimesToFile)
		.def("WriteFileTimeToLog", &History::WriteFileTimeToLog);

#ifdef VERSION_INFO
	m.attr("__version__") = VERSION_INFO;
#else
	m.attr("__version__") = "dev";
#endif
}