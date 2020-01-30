#pragma once
#include <windows.h>
#include <Psapi.h>
#include <Shlwapi.h>
#include "Util.h"

#pragma comment(lib,"shlwapi.lib")
#pragma comment(lib,"psapi.lib")

int getModuleSize(void * baseAddress);
bool getModuleInfo(MODULEINFO* mInfo);
//NEEDS MORE TESTING
DWORD FindPattern(const char *pattern, const char *mask, void* start, void* end);



DWORD getMaxAddress();