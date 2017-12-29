#include "Anticheat.h"

Anticheat::Anticheat() {
	PotentialCheatFound = false;
}

Anticheat::~Anticheat() {
}

bool Anticheat::OutputToFile(string playerName, string hack, MemoryManager* memory) {

	dataWriter.open("CheatAttempts.txt", std::ofstream::out | std::ofstream::app);
	if(dataWriter.is_open()) {
		dataWriter << "Time/Date: " << memory->GetTime() << endl;
		dataWriter << "Playername: " << playerName << endl;
		dataWriter << "Hack Detected: " << hack << endl;
		dataWriter << endl;
		return true;
	}
	return false;
	dataWriter.close();
}

bool Anticheat::ClearFile() {
	if (dataReader) {
		dataReader.open("debug.txt", std::ofstream::out | std::ofstream::trunc);
		return true;
	} else {
		return false;
	}
}

void Anticheat::FileDescription() {

}

void Anticheat::Detection() {

	// LOAD IN BLACKLISTED KEYWORDS

	dataReader.open("blacklist.txt");
	logWriter.open("debug.txt");

	while (!dataReader.eof()) {
		for (std::string line; std::getline(dataReader, line); ) {
			blacklist.push_back(line);
		}
	}

	dataReader.close();

	// SCAN PLAYERS COMPUTER AND SAVE FOUND FILE NAMES

	system("CLS");
	dataWriter.open("potentialcheats.txt");
	cout << "Scanning computer..." << endl;
	logWriter << "\nScanning computer..." << endl;
	Sleep(2000);

	FullSystemScan("C:\\Users\\Craig\\Desktop\\Test\\");

	Sleep(2000);
	system("CLS");

	dataWriter.close();

	cout << "Scan completed..\n" << endl;
	logWriter << "\nScan completed." << endl;

	if (PotentialCheatFound) {
		cout << "**";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
		cout << " Suspicious files found during the scan, saved to logs ";
		logWriter << "\nSuspicious files found during the scan, saved to logs ";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
		cout << "**\n" << endl;
		system("PAUSE");
		system("CLS");
		cout << "Analysing suspicious files..\n" << endl;
		logWriter << "\nAnalysing suspicious files..\n" << endl;
		Sleep(1000);
		AnalyzeFile();
	}
	else {
		cout << "**";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
		cout << " No suspicious files were found during the scan ";
		logWriter << "\nNo suspicious files were found during the scan ";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15); 
		cout << "**\n" << endl;
	}

	logWriter.close();
	cout << endl;
	system("PAUSE");
}

void Anticheat::FullSystemScan(std::string DirectoryName) {

	folderNameWithString = DirectoryName + "*";
	HANDLE File = FindFirstFile(folderNameWithString.c_str(), &file);

	// Check that file is not an invalid handle value returned
	if (File != INVALID_HANDLE_VALUE) {
		do {
			// Check if the file matches the full stops in navigation the directory
			if (strcmp(file.cFileName, ".") != 0 && strcmp(file.cFileName, "..") != 0) {
				if (file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
					// Create a new path consisting of the directory path and filename
					std::string NewPath = DirectoryName + file.cFileName;
					// Appending 2 back slashes to let the compiler know it's not an escape sequence
					NewPath = NewPath + "\\";
					// Recursively call itself again with the new path
					FullSystemScan(NewPath);
				}
				else {
					for (unsigned int i = 0; i < blacklist.size(); i++) {
						string temp = file.cFileName;
						// Check if any files match any blacklisted keywords
						if (DirectoryName.find(blacklist[i]) != string::npos || temp.find(blacklist[i]) != string::npos) {
							dataWriter << DirectoryName << file.cFileName << std::endl;
							PotentialCheatFound = true;
						}
					}
				}
			}
			// Find the next file in the system
		} while (FindNextFile(File, &file));
	}
}

void Anticheat::AnalyzeFile() {

	dataReader.open("potentialcheats.txt");
	dataWriter.open("confirmedcheats.txt");

	// Keep looping until the end of the file is reached
	while (!dataReader.eof()) {
		// Get a line from the potentialcheats.txt file
		for (std::string line; std::getline(dataReader, line); ) {
		// Open up the file located at the data read above
		cheatReader.open(line, ios::binary);
		// Get the first line of the file and check against the blacklist
		for (std::string line1; std::getline(cheatReader, line1); ) {							
				for (unsigned int i = 0; i < blacklist.size(); i++) {
					// If any cheat data if found, execute the statements
					if (line1.find(blacklist[i]) != string::npos) {
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
						cout << "Cheat data found at location: ";
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
						cout << line << endl;
						// Write the file path with cheat data into confirmedcheats.txt
						dataWriter << line << endl;
						cheatReader.close();						
						Sleep(1000);
					}
				}
			}
		}
	}
	dataWriter.close();
	dataReader.close();
}

////////////////////////////// CHEAT SIGNATURE TEST ////////////////////////////////////
void Anticheat::ScanMemory() { 

	MEMORY_BASIC_INFORMATION mbi;
	LPSTR clientDLL = "client.dll";

	mm.RunAllFuctions();
	for (DWORD addr = mm.GetModuleNameAddr(clientDLL, mm.GetProcessID()); VirtualQueryEx(mm.GetHandleProcess(), (LPCVOID*)addr, &mbi, sizeof(mbi)); addr = reinterpret_cast<DWORD>(mbi.BaseAddress) + mbi.RegionSize) {
		if (mbi.State & MEM_COMMIT) {
			if (mbi.AllocationProtect & READABLE) {
				for (DWORD i = addr; i < addr + mbi.RegionSize; ++i) {
					std::cout << "0x" << reinterpret_cast<void*>(i);
					std::cout << "\t" << (i);
					std::cout << std::endl;
				}
			}
		}
	}
}
