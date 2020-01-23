#pragma once
#include <Windows.h>
#include <unordered_map>

typedef void(*tFunction)(void);
typedef void(__thiscall *tClassFunction)(DWORD classPointer);

class Hook
{
public:
	virtual bool HookFunction() = 0;
	virtual bool UnHookFunction() = 0;

	virtual ~Hook() {};
};

