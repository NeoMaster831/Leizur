#pragma once
#pragma warning( disable : 4267 4244 )
#include <Windows.h>
#include <vector>

// Memory.h:  do some shit with memory

#define MAGIC 0x57616E654C6F6C69

typedef enum _USERMODE_COMMAND_MODE {
	Read,
	Write
} USERMODE_COMMAND_MODE;

typedef struct _USERMODE_COMMAND {
	unsigned long long magic = MAGIC;
	USERMODE_COMMAND_MODE mode;
	void* arg;
	void* retrn;
} USERMODE_COMMAND, * PUSERMODE_COMMAND;

typedef struct _READ_CMD_ARGS {
	DWORD pid;
	uintptr_t wh;
	size_t sz;
} READ_CMD_ARGS, * PREAD_CMD_ARGS;

typedef struct _WRITE_CMD_ARGS {
	DWORD pid;
	uintptr_t wh;
	void* what;
	size_t sz;
} WRITE_CMD_ARGS, * PWRITE_CMD_ARGS;

inline NTSTATUS(*queryFunc)(PUSERMODE_COMMAND, uintptr_t) = nullptr;

static bool MemInit() {
	auto user32dll = LoadLibrary(L"user32.dll");
	auto win32udll = LoadLibrary(L"win32u.dll");
	if (!user32dll || !win32udll) return false;

	auto addr = GetProcAddress(win32udll, "NtQueryCompositionSurfaceStatistics");
	if (!addr) return false;

	queryFunc = reinterpret_cast<NTSTATUS(*)(PUSERMODE_COMMAND, uintptr_t)>(addr);
	return true;
}

template <class T>
static std::pair<T, bool> RPM(DWORD processId, uintptr_t _where) {
	USERMODE_COMMAND rpmQuery;
	rpmQuery.mode = Read;

	READ_CMD_ARGS args = { processId, _where, sizeof(T) };
	rpmQuery.arg = &args;

	T ret;
	rpmQuery.retrn = &ret;
	
	auto result = queryFunc(&rpmQuery, 0);
	
	if (result != 0) return { ret, false };
	else return { ret, true };
}

static bool RPM(DWORD processId, uintptr_t _where, void* storeat, size_t size) {
	USERMODE_COMMAND rpmQuery;
	rpmQuery.mode = Read;

	READ_CMD_ARGS args = { processId, _where, size };
	rpmQuery.arg = &args;
	rpmQuery.retrn = storeat;

	return queryFunc(&rpmQuery, 0) == 0;
}

template <class T>
static bool WPM(DWORD processId, uintptr_t _where, T& what) {
	USERMODE_COMMAND wpmQuery;
	wpmQuery.mode = Write;

	WRITE_CMD_ARGS args = { processId, _where, what, sizeof(T) };
	wpmQuery.arg = &args;

	return queryFunc(&wpmQuery, 1) == 0;
}

static bool WPM(DWORD processId, uintptr_t _where, void* what, size_t size) {
	USERMODE_COMMAND wpmQuery;
	wpmQuery.mode = Write;

	WRITE_CMD_ARGS args = { processId, _where, what, size };
	wpmQuery.arg = &args;

	return queryFunc(&wpmQuery, 1) == 0;
}