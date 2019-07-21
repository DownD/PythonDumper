#include "JMPHook.h"

#define JMP_SIZE 5

JMPHook::JMPHook(void* addressToHook, tFunction redirection, BYTE sizeHook) : addressToHook(addressToHook), redirection(redirection), sizeHook(sizeHook)
{
	printf("value %x\n", addressToHook);
	isHooked = false;
	if (sizeHook < 5)
		throw std::exception("JMPHook needs 5 or more bytes");

}


JMPHook::~JMPHook()
{
	UnHookFunction();
	free(trampolineFunction);
	free(oldCode);
}

bool JMPHook::HookFunction()
{
	BYTE* tooHookLocation = (BYTE*)addressToHook;
	DWORD oldProtection = 0;
	if (!VirtualProtect(tooHookLocation, sizeHook, PAGE_EXECUTE_READWRITE, &oldProtection)) {
#ifdef _DEBUG
		printf("Fail Changing protection of Hooked Function\n!");
#endif
		return 0;
	}

	if (!setTrampolineFunction()) {
#ifdef _DEBUG
		printf("Fail Setting Trampoline Function\n!");
#endif
		return false;
	}

	//set Hook
	BYTE* HookBuffer = new BYTE[sizeHook];
	DWORD offset = getJMPOffset(tooHookLocation, trampolineFunction);
	HookBuffer[0] = 0xE9;
	memcpy(HookBuffer + 1, &offset, sizeof(offset));
	for (int i = 5; i<sizeHook; i++) {
		HookBuffer[i] = 0xCC;
	}
	memcpy(tooHookLocation, HookBuffer, sizeHook);
/*
#ifdef _DEBUG
	printf("Function Hooked\n!");
#endif*/

	if (!VirtualProtect(tooHookLocation, sizeHook, oldProtection,&oldProtection)) {
#ifdef _DEBUG
		printf("Fail Changing to old protection of Hooked Function %d\n!",GetLastError());
#endif
		UnHookFunction();
		return 0;
	}

	isHooked = true;
	return true;

}

bool JMPHook::UnHookFunction()
{
	if (!isHooked)
		return false;
	DWORD oldProtection = 0;
	if (!VirtualProtect(addressToHook, sizeHook, PAGE_EXECUTE_READWRITE, &oldProtection)) {
#ifdef _DEBUG
		printf("Fail Changing protection of Hooked Function\n!");
#endif
		return 0;
	}
	//Unset Hook
	memcpy(addressToHook, oldCode, sizeHook);

	if (!VirtualProtect(addressToHook, sizeHook, oldProtection, &oldProtection)) {
#ifdef _DEBUG
		printf("Fail Changing protection of Hooked Function\n!");
#endif
		return 0;
	}
	isHooked = false;
	/*
#ifdef _DEBUG
	printf("Function UnHooked!\n");
#endif*/
	return true;
}

bool JMPHook::setTrampolineFunction()
{
	BYTE callInstruction[] = { 0x60, 0xE8, 0x00, 0x00, 0x00, 0x00, 0x61 }; //PUSHA, CALL, POPA
	BYTE jumpInstruction[] = { 0xE9, 0x00, 0x00, 0x00, 0x00 }; //JMP

	//Allocate data
	int sizeOfFunction = sizeof(callInstruction) + sizeof(jumpInstruction) + sizeHook;
	trampolineFunction = (BYTE*)VirtualAlloc(NULL, sizeOfFunction, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if(!trampolineFunction){
#ifdef _DEBUG
		printf("Fail to allocate memory for trampoline function code: %d\n", GetLastError());
#endif 
		return 0;
	}
	oldCode = (BYTE*)malloc(sizeHook);

	if (!oldCode) {
#ifdef _DEBUG
		printf("Fail to allocate memory for old code\n");
#endif 
		return 0;
	}
	memset(trampolineFunction, 0xCC,sizeOfFunction);

	//Locations of CALL and JMP instructions
	void* jmpInstructionAddress = (void*)((DWORD)trampolineFunction + sizeHook+ sizeof(callInstruction));
	void* callInstructionAddress = (void*)((DWORD)trampolineFunction + 1);

	//Store oldCode
	memcpy(oldCode,addressToHook, sizeHook);

	//Change Addresses
	DWORD redirectionOffset = getJMPOffset(callInstructionAddress,redirection);
	DWORD originalFunctionOffset = getJMPOffset(jmpInstructionAddress,(void*)((DWORD)addressToHook + sizeHook));

	//CopyContent
	memcpy(callInstruction + 2, &redirectionOffset, 4);
	memcpy(jumpInstruction + 1, &originalFunctionOffset, 4);


	//Copy shellcode
	memcpy(trampolineFunction, callInstruction, sizeof(callInstruction));
	memcpy(trampolineFunction + sizeof(callInstruction), oldCode, sizeHook);
	memcpy(trampolineFunction + sizeof(callInstruction) + sizeHook, jumpInstruction, sizeof(jumpInstruction));

	/*
#ifdef _DEBUG
	printf("Trampoline Function Created at %#x!\nRaw Data: ", trampolineFunction);
	for (int i = 0; i < sizeOfFunction; i++) {
		printf("%x ",trampolineFunction[i]);
	}
	printf("\n");
#endif*/

}

DWORD JMPHook::getJMPOffset(void * jmpInstruction, void * function)
{
	return (DWORD)function - (DWORD)jmpInstruction - JMP_SIZE;
}
