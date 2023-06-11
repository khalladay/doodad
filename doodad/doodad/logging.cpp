#include "logging.h"
#include <Windows.h>

#include <stdio.h>
#include "debug.h"
#include <cstdarg>
#include <shlwapi.h>
#pragma comment (lib, "Shlwapi.lib")


class LogFile
{
public:

	LogFile()
	{
		char path[512];
		HMODULE hModule = GetModuleHandle(NULL);
		GetModuleFileNameA(hModule, path, 512);
		PathRemoveFileSpecA(path);
		strcat_s(path, "\\log.txt");

		fopen_s(&file, path, "wb");
		check(file);
	}

	~LogFile()
	{
		fclose(file);
	}

	void Write(const char* fmt, va_list args)
	{
		vfprintf(file, fmt, args);
		fprintf(file, "\n");

		vfprintf(stdout, fmt, args);
		fprintf(stdout, "\n");
		fflush(file);
	}

	FILE* file;
};


LogFile GLog;

void LOG(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	GLog.Write(fmt, args);
	va_end(args);

}
