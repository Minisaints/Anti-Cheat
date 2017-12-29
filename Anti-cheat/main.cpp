#include "MemoryManager.h"
#include "Anticheat.h"
#include <limits>

using namespace std;

MemoryManager* memory = new MemoryManager();
Anticheat* anti = new Anticheat();

void RunEngine();
void ShutdownEngine();

int main() {

	RunEngine();
	ShutdownEngine();
	return 0;
}

void RunEngine() {

	int userChoice;
	bool Quit = false;

	while (!Quit) {
			cout << "Anti cheat Engine\n" << endl;
			cout << "1. Run Anti-cheat engine" << endl;
			cout << "2. Repair engine modules/hooks" << endl;
			cout << "3. Clear debug text file" << endl;
			cout << "4. Scan CSGO memory" << endl;
			cout << "5. Shutdown Engine\n" << endl;

			cin >> userChoice;
			system("CLS");

			switch (userChoice) {
			case 1:
				cout << "Engine loading..\n" << endl;
				Sleep(1000);
				
				/*if (!memory->RunAllFuctions()) {
					cout << "\nEngine successfully loaded.." << endl;
				} else {
					Sleep(500);
					system("CLS");
					cout << "\nEngine already loaded.." << endl;
				}*/

				//while (true) {
					//anti->OutputToFile("Craig", "ESP");
					//anti->Detection(memory);

				anti->Detection();
				//}
				
				Sleep(1500);
				system("CLS");

				break;
			case 2:
				cout << "\nRepairing engine hooks and modules..";
				if (!memory->RunAllFuctions()) {
					cout << "\nRepair successful.." << endl;
				} else {
					Sleep(500);
					system("CLS");
					cout << "\nEngine hooks and modules already valid.." << endl;
				}
				Sleep(1500);
				system("CLS");
				break;
			case 3:
				anti->ClearFile();
				cout << "\nClearing debug file..";
				Sleep(1500);
				system("CLS");
				cout << "\nDebug file cleared..";
				Sleep(1500);
				system("CLS");
				break;
			case 4:
				anti->ScanMemory();
				break;
			case 5:
				Quit = true;
				break;
			default:
				break;
			}
	}
}

void ShutdownEngine() {

	cout << "\nShutting down engine.." << endl;
	Sleep(1000);

	if(memory) delete memory;
	if(anti) delete anti;
}