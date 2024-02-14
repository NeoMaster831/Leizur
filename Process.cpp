#include "Process.h"
#include <TlHelp32.h>

Process::Process(LPCWSTR processName) {

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pe32 = { 0 };
	pe32.dwSize = sizeof(PROCESSENTRY32);
	if (hSnapshot == INVALID_HANDLE_VALUE || !Process32First(hSnapshot, &pe32)) {
		throw 0x4b000001;
	}

	do {
		if (!lstrcmpi(pe32.szExeFile, processName)) {
			this->pid = pe32.th32ProcessID;
			break;
		}
	} while (Process32Next(hSnapshot, &pe32));

	if (this->pid == -1) {
		throw 0x4b000002;
	}

	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, this->pid);
	MODULEENTRY32 me32 = { 0 };
	me32.dwSize = sizeof(MODULEENTRY32);
	if (hSnapshot == INVALID_HANDLE_VALUE || !Module32First(hSnapshot, &me32)) {
		throw 0x4b000003;
	}

	do {
		std::wstring cvrt(me32.szModule);
		this->modules.insert({ cvrt, (uintptr_t)me32.modBaseAddr });
	} while (Module32Next(hSnapshot, &me32));

}