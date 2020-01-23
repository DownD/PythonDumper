#include "Util.h"

int split(char * str, char c, std::vector<std::string>* vec)
{
	std::istringstream f(str);
	std::string s;
	for (int i = 0; std::getline(f, s, c); i++) {
		vec->push_back(s);
	}
	return vec->size();
}

Stack::Stack(void * toHoldValues, int Capacity)
{
	arr = (BYTE*)toHoldValues;
	size = 0;
	capacity = Capacity;
}

Stack::~Stack()
{
}


int Stack::getSize()
{
	return size;
}

bool Stack::pushBYTE(BYTE value)
{
	if (size < capacity) {
		arr[size++] = value;
		return true;
	}

	return false;
}

bool Stack::pushDWORD(DWORD value)
{
	if (size + 3 < capacity) {
		DWORD* val = (DWORD*)((int)arr + size);
		*val = value;
		size += 4;
		return true;
	}
	return false;
}

bool Stack::pushPOINTER(void * value)
{
	return pushDWORD((DWORD)value);
}

bool Stack::pushWORD(WORD value)
{
	if (size + 1 < capacity) {
		WORD* val = (WORD*)((int)arr + size);
		*val = value;
		size += 2;
		return true;
	}
	return false;
}

bool Stack::pushARRAY(BYTE * arr, int size)
{
	if (this->size + size - 1 < capacity) {
		memcpy((void*)((int)this->arr + this->size), arr, size);
		this->size += size;
		return true;
	}

	return false;
}

bool Stack::getCopy(BYTE * buffer, int copySize)
{
	if (copySize >= size) {
		memcpy(buffer, arr, size);
		return true;
	}

	return false;
}


