#pragma once
#pragma warning( disable : 4267 4244 )
// IATHook.h: Protect sneaking my IAT
#include <Windows.h>

class H_IAT
{
	void* OriginalFunction;
	void* ObfFunc;
	
public:

	H_IAT(FARPROC fp);
	void Hook();
	void UnHook();

};

namespace IATHook {

}