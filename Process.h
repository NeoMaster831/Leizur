#pragma once
#pragma warning( disable : 4267 4244 )
#include "Memory.h"
#include <map>
#include <string>

// Class Process: do some shit with process memory
// Throw-level handling
/*
 * Errors (0x4b):
 * - 0: can't get module base from string
 * - 1: can't get process handle; process is running with admin
 * - 2: can't get pid; the process name is invalid
 * - 3: can't get modules; idk why
 * - 4: can't read process memory
 * - 5: can't write process memory
*/

class Process
{
	DWORD pid = -1;
	std::map<std::wstring, uintptr_t> modules;

public:
	
	Process(LPCWSTR processName);

	template <class T>
	T ReadMemory(uintptr_t _where) {
		auto res = RPM<T>(this->pid, _where);
		if (!res.second) throw 0x4b000004;
		return res.first;
	}

	template <class T>
	void WriteMemory(uintptr_t _where, T& what) {
		auto res = WPM<T>(this->pid, _where, what);
		if (!res) throw 0x4b000005;
	}

	template <class T>
	T GetPointerChainVal(LPCWSTR moduleName, std::vector<uint64_t>& offsets) {
		auto start = GetModuleBase(moduleName);
		for (int i = 0; i < offsets.size() - 1; i++) {
			start += offsets[i]; start = ReadMemory<uintptr_t>(start);
		}
		start += offsets[offsets.size() - 1];
		return ReadMemory<T>(start);
	}

	template <class T>
	T ReadMemory(void* _where) {
		return ReadMemory<T>((uintptr_t)_where);
	};

	template <class T>
	void WriteMemory(void* _where, T& what) {
		WriteMemory<T>((uintptr_t)_where, what);
	};

	uintptr_t GetModuleBase(std::wstring moduleName) {
		auto res = modules.find(moduleName);
		if (res == modules.end()) throw 0x4b000000;
		return res->second;
	}

};

