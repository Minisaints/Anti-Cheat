#pragma once

#include "MemoryManager.h"
#include <fstream>
#include <vector>
#include <string>

using namespace std;

class Anticheat {

private:
	std::ofstream dataWriter, logWriter;
	std::ifstream dataReader, cheatReader;
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); // COLOR
	vector<string> blacklist;
	WIN32_FIND_DATA file;
	std::string folderNameWithString;
	bool PotentialCheatFound;
	MemoryManager mm;
	//// CHEAT SIGNATURE TEST ////

	//////////////////////////////
public:
	Anticheat();
	~Anticheat();
	bool OutputToFile(string playerName, string hack, MemoryManager* memory);
	bool ClearFile();
	void Detection();
	void FileDescription();
	void FullSystemScan(string DirectoryName);
	void AnalyzeFile();
	//// CHEAT SIGNATURE TEST ////
	void ScanMemory();


	//////////////////////////////

};