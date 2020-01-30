#include "SleepFunctionHook.h"

#define DEBUG_INFO(...); {printf(__VA_ARGS__); printf("\n");}


SleepFunctionHook::SleepFunctionHook(tFunction toBeRedirected) 
{
}


SleepFunctionHook::~SleepFunctionHook()
{
}

Hook * SleepFunctionHook::setupUpdateHook(tFunction toBeRedirect)
{
	MODULEINFO mod;
	if (!getModuleInfo(&mod)) { return 0; }
	DWORD addr = FindPattern(UPDATE_PATTERN, UPDATE_MASK, 0x0,(void*)getMaxAddress());
	if (addr == 0) {
		printf("Pattern not found\n");
		return 0;
	}

	return new JMPHook((void*)addr, toBeRedirect, 6, THIS_CALL);
}

Hook * SleepFunctionHook::setupHook(tFunction toBeRedirected)
{
	HMODULE mod = GetModuleHandle(L"KERNELBASE");
	if (!mod) {
		DEBUG_INFO("Error finding KERNELBASE dll for hooking %d", GetLastError()); return NULL;
	}

	void* sleepFunction = GetProcAddress(mod, "Sleep");
	if (!sleepFunction) { DEBUG_INFO("Error finding Sleep Function for Hooking Error Code: %d", GetLastError()); return NULL; }

	return new JMPHook(sleepFunction, toBeRedirected, 5,THIS_CALL);
}

