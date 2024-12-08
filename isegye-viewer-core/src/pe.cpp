// Copyright 2024 kus-system-security-team-1
#include "..\include\pe.h"

PE::PE() {
}

double PE::calculateEntropy(WCHAR *pe_name) {
	HANDLE hFile = CreateFileW(pe_name, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		return -1;
	}

	DWORD fileSize = GetFileSize(hFile, NULL);
	if (fileSize == INVALID_FILE_SIZE) {
		CloseHandle(hFile);
		return -1;
	}

	unsigned char* fileBuffer = new unsigned char[fileSize];
	if (fileBuffer == NULL) {
		CloseHandle(hFile);
		return -1;
	}
	DWORD bytesRead;
	if (!ReadFile(hFile, fileBuffer, fileSize, &bytesRead, NULL)) {
		delete[] fileBuffer;
		CloseHandle(hFile);
		return -1;
	}
	CloseHandle(hFile);

	/* Start calculate entropy of PE */
	unsigned int freq[256] = { 0 };
	double entropy = 0.0;
	
	for (DWORD i = 0; i < fileSize; i++) {
		freq[fileBuffer[i]]++;
	}

	for (int i = 0; i < 256; i++) {
		if (freq[i] == 0) {
			continue;
		}
		double p = (double)freq[i] / fileSize;
		entropy -= p * log2(p);
	}
	return entropy;
}

/* 1.1.1 */
DetectEntropyType PE::detectEntropyLevel(double entropy) {
	if (entropy >= 7.8) {
		return HIGH;
	}
	else if (7.5 <= entropy && entropy < 7.8) {
		return MIDDLE;
	}
	else {
		return LOW;
	}
}

void bind_pe(py::module_& m) {
	py::enum_<DetectEntropyType>(m, "DetectEntropyType")
		.value("HIGH", DetectEntropyType::HIGH)
		.value("MIDDLE", DetectEntropyType::MIDDLE)
		.value("LOW", DetectEntropyType::LOW)
		.export_values();

	py::class_<PE>(m, "PE")
		.def(py::init())
		.def("calculateEntropy", &PE::calculateEntropy)
		.def("detectEntropyLevel", &PE::detectEntropyLevel);

#ifdef VERSION_INFO
	m.attr("__version__") = VERSION_INFO;
#else
	m.attr("__version__") = "dev";
#endif
}
