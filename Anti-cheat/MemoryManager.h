#pragma once

#include <Windows.h>
#include <iostream>
#include <ostream>
#include <time.h>
#include <tlhelp32.h>
#include <vector>
#include <string>
#include <fstream>

using namespace std;

#define FL_ONGROUND 257
#define SPACE_BAR 0x20
#define F6_Key 0x75
#define TAB_Key 0x09
#define READABLE (PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY | PAGE_READONLY | PAGE_READWRITE | PAGE_WRITECOPY)

class MemoryManager {
private:
	PROCESSENTRY32 gameProcess;
	HANDLE HandleProcess;
	HWND HWNDCss;
	DWORD Client;
	DWORD Engine;
	DWORD Overlay;
	DWORD VGui;
	DWORD LibCef;
	DWORD Steam;
	LPSTR clientDLL;
	LPSTR engineDLL;
	LPSTR vguimatsurfaceDLL;
	const char *processName;
	DWORD processID;
	DWORD dwSize;
	std::ofstream dataWriter;
public:
	MemoryManager();
	~MemoryManager();
	DWORD FindProcess(const char *ProcessName, PROCESSENTRY32 *pEntry);
	DWORD GetProcessThreads(DWORD DwordProcess);
	DWORD GetModuleNameAddr(LPSTR LPSTRModuleName, DWORD DwordProcessId);
	HANDLE& GetHandleProcess();
	DWORD& GetClient();
	void SetTokenPrivileges();
	bool RunAllFuctions();
	string GetTime();
	DWORD GetProcessID();
	DWORD GetDwSize();
};


