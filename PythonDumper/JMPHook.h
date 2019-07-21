#pragma once
#include "Hook.h"
#include <Windows.h>
#include <unordered_map>

//Creates a JUMP Hook
//at least 5 bytes are necessary
class JMPHook : public Hook
{
public:
	JMPHook(void* addressToHook, tFunction redirection, BYTE sizeHook);
	~JMPHook();


	// Inherited via Hook
	virtual bool HookFunction() override;
	virtual bool UnHookFunction() override;

private:
	//Needs to have permission to access data
	bool setTrampolineFunction();
	DWORD getJMPOffset(void* jmpInstruction, void* function);

private:
	void* addressToHook;
	tFunction redirection;
	bool isHooked;

	BYTE* trampolineFunction;
	BYTE* oldCode;
	BYTE sizeHook;

};

