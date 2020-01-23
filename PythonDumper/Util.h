#pragma once
#include <Windows.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <sstream> 



int split(char* str, char c, std::vector<std::string>* vec);


class Stack {
public:
	Stack(void* toHoldValues, int Capacity);
	~Stack();
	int getSize();

	bool pushBYTE(BYTE value);
	bool pushDWORD(DWORD value);
#ifdef _M_IX86
	bool pushPOINTER(void* value);
#endif
	bool pushWORD(WORD value);
	bool pushARRAY(BYTE* arr, int size);

	bool getCopy(BYTE* buffer, int copySize);

private:

	int capacity;
	int size;
	BYTE* arr;
};
