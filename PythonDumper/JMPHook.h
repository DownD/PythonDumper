#pragma once
#include "Hook.h"
#include <Windows.h>
#include <unordered_map>
#include "Util.h"

#define JMP_SIZE 5

template<class Return, class ClassPointer, class... Args>
using _cdeclFunction = Return(__cdecl*)(ClassPointer, Args...);

template<class Return, class ClassPointer, class... Args>
using _thiscallFunction = Return(__thiscall*)(ClassPointer, Args...);


#define SAVE_EAX 0x1
#define SAVE_ECX 0x2
#define SAVE_EDX 0x4

//To be used in bitamp argument
#define CDCEL_CALL 0x00000
#define THIS_CALL SAVE_ECX
#define FAST_CALL SAVE_ECX | SAVE_EDX
#define SAVE_ALL_NEEDED SAVE_EAX | SAVE_ECX | SAVE_EDX
#define MID_FUNCTION SAVE_ALL_NEEDED

/*
//Creates a JUMP Hook with a trampoline function, AT LEAST 5 BYTES necessary,
The size of the hook must be more then 5 if some instruction get cutted in half using the 5 bytes.
Example:
PUSH EBP
MOV EBP, ESP
SUB EBP , 0x00000010

Here the HookSize should be 6 because using the minimal 5 would break the SUB instruction.
//The last argument, the bit map, can be used to tell the object to generate a function that save specific registers
//This can be usefull for diferent call conventions without performance hit, some macros are defined for some calling convetions
//0x00000001 -> saves EAX
//0x00000010 -> saves ECX
//0x00000100 -> saves EDX
Only this registers are available because this are the fewer that the calle might not restore, the rest should be
caller safe for most of the calling conventions
Is important to note that for value on the bit map, THE 1ST VALUE ON THE STACK WILL BE POPPED and restored after
redirection function has been called, so do not try to read the FIRST VALUE ON STACK, the reason for this is just
because it is intended to be used at the start of any function. But i might update it in the future.

The function where the flow is to be redirected shall be passed in 2nd argument, that function will have control
over all the registers and arguments passed in stack, except for the original function return address (FIRST VALUE ON STACK,
discussed above)
*/


class JMPHook : public Hook
{
public:
	JMPHook(void* addressToHook, void* redirection, BYTE sizeHook, BYTE bitMap_SavedRegisters);
	virtual ~JMPHook();


	// Inherited via Hook
	virtual bool HookFunction() override;
	virtual bool UnHookFunction() override;

private:
	//Needs to have permission to access data
	bool setTrampolineFunction();
	DWORD getJMPOffset(void* jmpInstruction, void* function);

	BYTE* generateShellCode();
	BYTE  sizeRegisterToSave();

private:
	void* addressToHook;
	void* redirection;
	bool isHooked;

	BYTE* trampolineFunction;
	BYTE* oldCode;
	BYTE sizeHook;
	BYTE bit_map;

	DWORD storeEAX;
	DWORD storeEDX;
	DWORD storeECX;
	DWORD storeReturn;
};
