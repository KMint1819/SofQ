#include <Windows.h>
#include <fstream>

INT APIENTRY DllMain(HMODULE hDLL, DWORD Reason, LPVOID Reserved)
{
	try {
		std::ofstream file("D:\\output.txt");
		file << "Hello!\n";
		file.close();
	}
	catch (...) {
		std::ofstream file("D:\\error.txt");
		file << "Hello!\n";
		file.close();
	}
}
