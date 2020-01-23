#pragma once
#include "JMPHook.h"

class SleepFunctionHook
{
public:
	SleepFunctionHook(tFunction toBeRedirected);
	~SleepFunctionHook();


	static Hook* setupHook(tFunction toBeRedirected);
};

