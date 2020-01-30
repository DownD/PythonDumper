#include "MemorySearch.h"



int getModuleSize(void * baseAddress)
{
	PIMAGE_DOS_HEADER     pDosH = (PIMAGE_DOS_HEADER)baseAddress;
	if (pDosH->e_magic == (WORD)0x5A4D) {
		PIMAGE_NT_HEADERS     pNtH = (PIMAGE_NT_HEADERS)((int)baseAddress + pDosH->e_lfanew);
		if (pNtH->Signature == (DWORD)0x4550) {
			if (pNtH->OptionalHeader.Magic == (WORD)0x010B) {
				return pNtH->OptionalHeader.SizeOfImage;

			}
		}

	}
	return 0;
}


bool getModuleInfo(MODULEINFO* mInfo)
{
	HANDLE psHandle = GetCurrentProcess();

	TCHAR buffer[MAX_PATH];
	int path_size = GetProcessImageFileName(psHandle, buffer, MAX_PATH);


	if (!path_size) {
		DEBUG_INFO("Fail to Get Module FileName");
		return false;
	}


	PathStripPath(buffer);
	HMODULE hModule = GetModuleHandle(buffer);
	if (hModule == 0)
		return false;

	printf("Scanning Module: %S\n", buffer);
	GetModuleInformation(psHandle, hModule, mInfo, sizeof(MODULEINFO));
	mInfo->SizeOfImage = getModuleSize(mInfo->lpBaseOfDll);
	return true;
}



DWORD FindPattern(const char *pattern, const char *mask, void* start, void* end)
{
	int len = strlen(mask);
	printf("Pattern: ");
	for (int i = 0; i < len; i++) {
		printf("%#x ", (BYTE)pattern[i]);
	}
	printf("\n\n");


	DWORD patternLength = (DWORD)strlen(mask);
	MEMORY_BASIC_INFORMATION PermInfo;


	DWORD startModule = (DWORD)start;
	DWORD endModule = (DWORD)end;
	int pageEndAddr = startModule;

	for (DWORD indexAddr = startModule; indexAddr + patternLength <= endModule; indexAddr++)
	{
		if (pageEndAddr <= indexAddr)
		{
			if (VirtualQuery((LPCVOID*)(indexAddr), &PermInfo, sizeof(PermInfo)))
			{
				if ((PermInfo.Protect == PAGE_EXECUTE || PermInfo.Protect == PAGE_EXECUTE_READ || PermInfo.Protect == PAGE_EXECUTE_READWRITE || PermInfo.Protect == PAGE_EXECUTE_READ || PermInfo.Protect == PAGE_EXECUTE_WRITECOPY)&& PermInfo.State == MEM_COMMIT) {
					indexAddr = (DWORD)PermInfo.BaseAddress;
					pageEndAddr = indexAddr + PermInfo.RegionSize;
				}
				else {
					pageEndAddr += PermInfo.RegionSize;
					indexAddr = pageEndAddr;
					continue;
				}
			}
			else {
				//DEBUG_INFO("Error Querying memory at %#x", indexAddr);
				continue;
			}
		}

		if (indexAddr + patternLength > pageEndAddr) {
			indexAddr = pageEndAddr;
			continue;
		}
		//printf("Scanning Address %#x\n",indexAddr);
		bool found = true;
		for (DWORD j = 0; j < patternLength && found; j++)
		{
			//if we have a ? in our mask then we have true by default, 
			//or if the bytes match then we keep searching until finding it or not
			found &= mask[j] == '?' || pattern[j] == *(char*)(indexAddr + j);
		}

		//found = true, our entire pattern was found
		//return the memory addy so we can write to it
		if (found) {
			return indexAddr;
		}
	}
	return NULL;
}


DWORD getMaxAddress() {
	return 0x7FFFFFFF;
}
