#include "MemoryManager.h"

// MemoryManager constructor initialises some of the repeated data used throughout the script.

MemoryManager::MemoryManager() {
	clientDLL = "client.dll";
	engineDLL = "engine.dll";
	processName = "csgo.exe";
	vguimatsurfaceDLL = "vguimatsurface.dll";
	dataWriter.open("debug.txt", std::ofstream::out | std::ofstream::app);
}

MemoryManager::~MemoryManager() {
	dataWriter.close();
}

// The FindProcessName() function will take a snapshot of all the processes in the system 
// then compare each one with the specified process. If the process is found,
// I close the  handle to the snapshot, then return the process ID. 

DWORD MemoryManager::FindProcess(const char *ProcessName, PROCESSENTRY32 *pEntry) {


	// Process structure that will contain various data about the process.
	PROCESSENTRY32 ProcessEntry;
	// Setting the size of the structure.
	ProcessEntry.dwSize = sizeof(PROCESSENTRY32);
	// Taking a snapshot of all the active system processes and returning a handle to the process.
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	// If the snapshot returns null or an invalid handle the terminate.
	if (hSnapshot == INVALID_HANDLE_VALUE) return 0;
	// If there is not a first process snapshot, close the handle and terminate.
	if (!Process32First(hSnapshot, &ProcessEntry)) {
		CloseHandle(hSnapshot);
		return 0;
	}

	// Check each processes executable file name against the specified string whilst there are
	// still processes in the structure - This is essentially process walking.
	do {
		// If the string compare between a process matches the specified process name.
		if (!_strcmpi(ProcessEntry.szExeFile, ProcessName)) {
			// Copies the process information gathered to the processEntry structure.
			memcpy((void *)pEntry, (void *)&ProcessEntry, sizeof(PROCESSENTRY32));
			CloseHandle(hSnapshot);
			Sleep(500);
			
			Sleep(1000);
			// Returns the process ID that matched the specified process name.
			processID = ProcessEntry.th32ProcessID;
			return ProcessEntry.th32ProcessID;
		}

		// Recieves information about the next process.
	} while (Process32Next(hSnapshot, &ProcessEntry));
	
	CloseHandle(hSnapshot);
	return 0;
}

// The GetThreadByProcess() function takes a snapshot of the all the threads in the system
// then walk through each and compare the threads process ID with the specified process ID.
// If any threads process ID matches the specified process ID, the handle to the snapshot is closed
// and the threadID is returned.

DWORD MemoryManager::GetProcessThreads(DWORD DwordProcessID) {
	// Thread structure that will contain various data about the processes threads.
	THREADENTRY32 ThreadEntry;
	// Set the structure size.
	ThreadEntry.dwSize = sizeof(THREADENTRY32);
	// Take a snapshot of all the threads in the system.
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	// If the snapshot is null or an invalid handle value, terminate.
	if (hSnapshot == INVALID_HANDLE_VALUE) return 0;
	// If there is not a first thread snapshot, close the handle and terminate.  
	if (!Thread32First(hSnapshot, &ThreadEntry)) { CloseHandle(hSnapshot); return 0; }

	// Whilst there are still threads to check in the thread structure.
	do {
		// If the process owner for the thread matches the specified process ID.
		if (ThreadEntry.th32OwnerProcessID == DwordProcessID) {
			CloseHandle(hSnapshot);
			// Return the thread ID that matched the process owner specified.
			return ThreadEntry.th32ThreadID;
		}
		// Receives information about the next thread.
	} while (Thread32Next(hSnapshot, &ThreadEntry));
	CloseHandle(hSnapshot);
	return 0;
}

// The GetModuleNamePointer() function takes a snapshot of all the modules within the 
// specified process then compares all the module names against the specified DLL strings,
// if any match, return the base address of the module.

DWORD MemoryManager::GetModuleNameAddr(LPSTR LPSTRModuleName, DWORD DwordProcessId) {
	// Describes an entry from a list of the modules belonging to the specified process.
	MODULEENTRY32 lpModuleEntry = { 0 };
	// Take a snapshot of all modules in the specified process ID.
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, DwordProcessId);
	// If the snapshot is invalid, return null.
	if (!hSnapShot)
		return NULL;
	// Set the size of the module structure.
	lpModuleEntry.dwSize = sizeof(lpModuleEntry);
	// Module32First:: If the first module was sent to the buffer, returns true.
	BOOL RunModule = Module32First(hSnapShot, &lpModuleEntry);
	// While there are still modules to copy.
	while (RunModule) {
		// If a module doesn't match the specified module names, close handle.
		if (!strcmp(lpModuleEntry.szModule, LPSTRModuleName)) {
			CloseHandle(hSnapShot);
			Sleep(500);
			// If the module matches the specified names, return the 
			// base address to the module.
			dwSize = lpModuleEntry.dwSize;
			return (DWORD)lpModuleEntry.modBaseAddr;
		}
		// Recieves information about the next module.
		RunModule = Module32Next(hSnapShot, &lpModuleEntry);
		// Display the loaded modules
		cout << "\n  LOADING MODULE: " << lpModuleEntry.szModule;
		dataWriter << "\n  LOADING MODULE: " << lpModuleEntry.szModule;

	}
	CloseHandle(hSnapShot);
	return NULL;
}

// The RunSetDebugPrivs() function adjusts a processes access token privileges to give the process the 
// ability to perform low level actions such as modifying memory. 

void MemoryManager::SetTokenPrivileges() {
	// Creates a handle to the current processes access token.
	HANDLE HandleProcess = GetCurrentProcess(), HandleToken;
	// The TOKEN_PRIVILEGES structure contains information about a set of privileges for an access token.
	TOKEN_PRIVILEGES priv;
	// (Locally Unique Identifier) is a 64-bit value guaranteed to be unique only 
	// on the system on which it was generated.
	LUID LUID;
	// Opens the access token associated with the specified process where the system will check the 
	// requested access rights against the DACL in the tokens security descriptor.
	// This call will also allow the access tokens privileges to be adjusted.
	OpenProcessToken(HandleProcess, TOKEN_ADJUST_PRIVILEGES, &HandleToken);
	// LookupPrivilegeValue function retrieves the LUID used on a specified system to locally
	// represent the specified privilege name.
	LookupPrivilegeValue(0, "seDebugPrivilege", &LUID);
	// Specifies the number of privileges in the privilege set.
	priv.PrivilegeCount = 1;
	// Specifies the LUID value.
	priv.Privileges[0].Luid = LUID;
	// Enables the privilege at index [0]
	priv.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	// Enables or disables privileges in the specified access token. This is possible as the 
	// access token has been given token adjust access.
	AdjustTokenPrivileges(HandleToken, false, &priv, 0, 0, 0);
	// Close the handle to the token and the process.
	CloseHandle(HandleToken);
	CloseHandle(HandleProcess);
}

bool MemoryManager::RunAllFuctions() {
	SetTokenPrivileges();

	if (Client != 0x0 && VGui != 0x0) { //  ENGINE ALREADY LOADED - RETURN FALSE
		return true;
	}
	while (!FindProcess(processName, &gameProcess)) Sleep(12);
	while (!(GetProcessThreads(gameProcess.th32ProcessID))) Sleep(12);
	HandleProcess = OpenProcess(PROCESS_ALL_ACCESS, false, gameProcess.th32ProcessID);
	while (Client == 0x0) Client = GetModuleNameAddr(clientDLL, gameProcess.th32ProcessID);
	//while (Engine == 0x0) Engine = GetModuleNameAddr(engineDLL, gameProcess.th32ProcessID);
	while (VGui == 0x0) VGui = GetModuleNameAddr(vguimatsurfaceDLL, gameProcess.th32ProcessID);
	system("CLS");	
		return false;
}

// The GetTime() function gets the system date/time to display before any console outputs.
// ***Add colour***

string MemoryManager::GetTime() {
	time_t t;
	time(&t);

	string p = ctime(&t);
	p = p.substr(0, p.length() - 1);

	return p;
}

DWORD MemoryManager::GetProcessID() {return processID;}

DWORD MemoryManager::GetDwSize() { return dwSize; }

// GetHandleProcess() function returns the handle to a specified process.
HANDLE& MemoryManager::GetHandleProcess() { return HandleProcess; }

// GetClient() function returns the address to the client DLL.
DWORD& MemoryManager::GetClient() { return Client; }
