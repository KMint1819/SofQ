#undef UNICODE
#include <vector>
#include <string>
#include <iostream>
#include <windows.h>
#include <Tlhelp32.h>

using std::vector;
using std::string;

inline bool fileExists(const std::string& name) {
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}

int main(void)
{
	if (fileExists("D:\\output.txt"))
	{
		printf("Removing...\n");
		remove("D:\\output.txt");
	}
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);
	HANDLE hTool32 = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (Process32First(hTool32, &pe32))
	{
		while ((Process32Next(hTool32, &pe32)) == TRUE) {
			char exeName[] = "Calculator.exe";
			//char exeName[] = "notepad.exe";
			if (strcmp(pe32.szExeFile, exeName) == 0)
			{
				printf("Found %s at %d\n", exeName, pe32.th32ProcessID);
				char* DirPath = new char[MAX_PATH];
				char* FullPath = new char[MAX_PATH];
				GetCurrentDirectory(MAX_PATH, DirPath);
				sprintf_s(FullPath, MAX_PATH, "%s\\..\\x64\\Debug\\TestDLL.dll", DirPath);
				printf("%s File exists: %d\n", FullPath, fileExists(FullPath));
				HANDLE hProcess = OpenProcess(
					PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, FALSE, pe32.th32ProcessID);
				if (hProcess == NULL)
				{
					auto err = GetLastError();
					printf("OpenProcess failed! code %d\n", err);
				}
				LPVOID LoadLibraryAddr = (LPVOID)GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");
				if (LoadLibraryAddr == NULL)
				{
					auto err = GetLastError();
					printf("LoadLib failed! code %d\n", err);
				}
				LPVOID LLParam = (LPVOID)VirtualAllocEx(hProcess, NULL, strlen(FullPath),
					MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
				if (LLParam == NULL)
				{
					auto err = GetLastError();
					printf("Alloc failed! code %d\n", err);
				}
				BOOL status = WriteProcessMemory(hProcess, LLParam, FullPath, strlen(FullPath), NULL);
				if (status == 0)
				{
					auto err = GetLastError();
					printf("WriteProcessMemory failed! code %d\n", err);
				}
				auto handle = CreateRemoteThread(hProcess, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibraryAddr,
					LLParam, NULL, NULL);
				if (handle == NULL)
				{
					auto err = GetLastError();
					printf("Injection failed! code %d\n", err);
				}
				else
					printf("Succeed!\n");
				CloseHandle(hProcess);
				delete[] DirPath;
				delete[] FullPath;
				std::cin.get();
			}

		}
	}
	CloseHandle(hTool32);
	return 0;
}