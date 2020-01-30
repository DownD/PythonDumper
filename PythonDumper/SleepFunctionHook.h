#pragma once
#include "JMPHook.h"
#include "MemorySearch.h"

#define UPDATE_PATTERN "\x81\xec\x00\x00\x00\x00\x56\x8b\xf1\x8b\x46\x00\x83\xF8\xFF"
#define UPDATE_MASK "xx????xxxxx?xxx"


class SleepFunctionHook
{
public:
	SleepFunctionHook(tFunction toBeRedirected);
	~SleepFunctionHook();


	static Hook* setupUpdateHook(tFunction toBeRedirect);
	static Hook* setupHook(tFunction toBeRedirected);
};

