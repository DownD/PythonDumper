#include "SleepFunctionHook.h"
#define COLOR_RED "\x1B[31m"
#define COLOR_GREEN "\x1B[32m"
#define COLOR_DEFAULT "\033[0m"

#define DEBUG_INFO(color,...); {printf(__VA_ARGS__); printf("\n");}


SleepFunctionHook::SleepFunctionHook(tFunction toBeRedirected) 
{
}


SleepFunctionHook::~SleepFunctionHook()
{
}

Hook * SleepFunctionHook::setupHook(tFunction toBeRedirected)
{
	HMODULE mod = GetModuleHandle(L"KERNELBASE");
	if (!mod) {
		DEBUG_INFO(COLOR_RED , "Error finding KERNELBASE dll for hooking %d", GetLastError()); return NULL;
	}

	void* sleepFunction = GetProcAddress(mod, "Sleep");
	if (!sleepFunction) { DEBUG_INFO(COLOR_RED, "Error finding Sleep Function for Hooking Error Code: %d", GetLastError()); return NULL; }

	return new JMPHook(sleepFunction, toBeRedirected, 5,THIS_CALL);
}

