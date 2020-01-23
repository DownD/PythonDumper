#include "JMPHook.h"

#define SHELL_CODE_DEFAULT_SIZE 0x16

JMPHook::JMPHook(void* addressToHook, void* redirection, BYTE sizeHook, BYTE bit_map) : addressToHook(addressToHook), redirection(redirection), sizeHook(sizeHook), bit_map(bit_map)
{
	isHooked = false;
	if (sizeHook < 5)
		throw std::exception("JMPHook needs 5 or more bytes");

	oldCode = (BYTE*)malloc(sizeHook);

}


JMPHook::~JMPHook()
{
	UnHookFunction();
	if(trampolineFunction)
		VirtualFree(trampolineFunction,0,MEM_RELEASE);

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

	//store old code
	memcpy(oldCode,tooHookLocation,sizeHook);


	//set Hook
	BYTE* HookBuffer = new BYTE[sizeHook];
	DWORD offset = getJMPOffset(tooHookLocation, trampolineFunction);
	HookBuffer[0] = 0xE9;
	memcpy((void*)((int)HookBuffer + 1), &offset, sizeof(offset));
	for (int i = 5; i<sizeHook; i++) {
		HookBuffer[i] = 0xCC;
	}
	memcpy(tooHookLocation, HookBuffer, sizeHook);
	delete[] HookBuffer;
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
	return true;
}

bool JMPHook::setTrampolineFunction()
{
	trampolineFunction = generateShellCode();
	return true;
}

DWORD JMPHook::getJMPOffset(void * jmpInstruction, void * function)
{
	return (DWORD)function - (DWORD)jmpInstruction - JMP_SIZE;
}

BYTE * JMPHook::generateShellCode()
{

	//ALOCATION
	BYTE sizeTrampoline = SHELL_CODE_DEFAULT_SIZE + sizeRegisterToSave()+ sizeHook;
	BYTE* trampolineFunction = (BYTE*)VirtualAlloc(NULL, sizeTrampoline, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	Stack trampoline_function(trampolineFunction,sizeTrampoline);

	// POP [storeReturn]
	trampoline_function.pushBYTE(0x8F);
	trampoline_function.pushBYTE(0x05);
	trampoline_function.pushPOINTER(&storeReturn);

	//mov [storeVariable], register
	if (bit_map & SAVE_EAX) {
		trampoline_function.pushBYTE(0xA3);
		trampoline_function.pushPOINTER(&storeEAX);

	}
	if (bit_map & SAVE_ECX) {
		trampoline_function.pushBYTE(0x89);
		trampoline_function.pushBYTE(0x0D);
		trampoline_function.pushPOINTER(&storeECX);
	}
	if (bit_map & SAVE_EDX) {
		trampoline_function.pushBYTE(0x89);
		trampoline_function.pushBYTE(0x15);
		trampoline_function.pushPOINTER(&storeEDX);
	}


	//CALL redirection
	trampoline_function.pushBYTE(0xE8);
	DWORD offset = getJMPOffset((void*)((int)trampolineFunction + trampoline_function.getSize() - 1), redirection);
	trampoline_function.pushDWORD(offset);

	// PUSH [storeReturn]
	trampoline_function.pushBYTE(0xFF);
	trampoline_function.pushBYTE(0x35);
	trampoline_function.pushPOINTER(&storeReturn);


	//mov register, [storeVariable]
	if (bit_map & SAVE_EAX) {
		trampoline_function.pushBYTE(0xA1);
		trampoline_function.pushPOINTER(&storeEAX);
	}
	if (bit_map & SAVE_ECX) {
		trampoline_function.pushBYTE(0x8B);
		trampoline_function.pushBYTE(0x0D);
		trampoline_function.pushPOINTER(&storeECX);
	}
	if (bit_map & SAVE_EDX) {
		trampoline_function.pushBYTE(0x8B);
		trampoline_function.pushBYTE(0x15);
		trampoline_function.pushPOINTER(&storeECX);
	}

	//Replaced Code
	trampoline_function.pushARRAY((BYTE*)addressToHook, sizeHook);

	//JMP originalFunction
	DWORD originalFunctionOffset = getJMPOffset((void*)((int)trampolineFunction + trampoline_function.getSize()), (void*)((DWORD)addressToHook + sizeHook));
	trampoline_function.pushBYTE(0xE9);
	trampoline_function.pushDWORD(originalFunctionOffset);

	return trampolineFunction;
}



BYTE JMPHook::sizeRegisterToSave()
{
	BYTE num = 0;
	if (bit_map & SAVE_EAX)
		num++;
	if (bit_map & SAVE_ECX)
		num++;
	if (bit_map & SAVE_EDX)
		num++;
	num *= 12; //mov [0xFFFFFF], esp, size 6 *2 because of the load
	return num;
}
