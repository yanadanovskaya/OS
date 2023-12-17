#include <iostream>
#include <fstream>
#include <vector>
#include <windows.h>
#include <conio.h>
#include <algorithm>

HANDLE* hStarted;

struct employee
{
	int num;
	char name[10];
	double hours;
};

HANDLE* hChannel;
HANDLE* hThreads;
HANDLE* hSemaphore;
employee* empl;
int quantity_empl;
int quantity_clients;
std::string fileName;

DWORD WINAPI operations(LPVOID channel) {
	HANDLE hChannel = (HANDLE)channel;
	DWORD bytesRead;
	DWORD bytesWrite;

	int m;
	while (true) {
		if (!ReadFile(
			hChannel,
			&m,
			sizeof(m),
			&bytesRead,
			NULL))
		{
		}
		else {
			int ID = m / 10;
			int selection = m % 10;
			if (selection == 1) {
				for (int i = 0; i < quantity_clients; i++)
				{
					WaitForSingleObject(hSemaphore[ID - 1], INFINITE);
				}
				employee* emp = new employee();
				emp->num = empl[ID - 1].num;
				emp->hours = empl[ID - 1].hours;
				strcpy_s(emp->name, empl[ID - 1].name);
				bool checker = WriteFile(hChannel, emp, sizeof(employee), &bytesWrite, NULL);
				if (checker) {
					std::cout << "Data to change has been sent\n";
				}
				else {
					std::cout << "Change data was not sent\n";
				}
				ReadFile(hChannel, emp, sizeof(employee), &bytesWrite, NULL);

				empl[ID - 1].hours = emp->hours;
				strcpy_s(empl[ID - 1].name, emp->name);
				std::ofstream file;
				file.open(fileName);
				for (int i = 0; i < quantity_empl; i++)
					file << empl[i].num << " " << empl[i].name << " " << empl[i].hours << "\n";
				file.close();
				int msg;
				ReadFile(hChannel, &msg, sizeof(msg), &bytesWrite, NULL);
				if (msg == 1) {
					for (int i = 0; i < quantity_clients; i++)
					{
						ReleaseSemaphore(hSemaphore[ID - 1], 1, NULL);
					}
				}
			}
			if (selection == 2) {

				WaitForSingleObject(hSemaphore[ID - 1], INFINITE);
				employee* emp = new employee();
				emp->num = empl[ID - 1].num;
				emp->hours = empl[ID - 1].hours;
				strcpy_s(emp->name, empl[ID - 1].name);
				bool checker = WriteFile(hChannel, emp, sizeof(employee), &bytesWrite, NULL);
				if (checker) {
					std::cout << "Read data has been sent\n";
				}
				else {
					std::cout << "Read data was not sent\n";
				}
				int msg;
				ReadFile(hChannel, &msg, sizeof(msg), &bytesWrite, NULL);
				if (msg == 1)
					ReleaseSemaphore(hSemaphore[ID - 1], 1, NULL);
			}
		}
	}
	DisconnectNamedPipe(hChannel);
	CloseHandle(hChannel);

}

int main() {
	std::cout << "Enter file name:\n";
	std::cin >> fileName;
	fileName = "employees.txt";
	std::cout << "Enter the number of employees:\n";
	std::cin >> quantity_empl;
	quantity_empl = 3;
	std::ofstream file;
	empl = new employee[quantity_empl];
	for (int i = 0; i < quantity_empl; i++)
	{
		std::cout << "Enter " << i + 1 << " employee ID:\n";
		std::cin >> empl[i].num;
		std::cout << "Enter employee name:\n";
		std::string tmp;
		std::cin >> empl[i].name;
		std::cout << "Enter employee hours:\n";
		std::cin >> empl[i].hours;
	}
	file.open(fileName);
	for (int i = 0; i < quantity_empl; i++)
		file << empl[i].num << " " << empl[i].name << " " << empl[i].hours << "\n";
	file.close();
	std::ifstream file_input;
	file_input.open(fileName);
	for (int i = 0; i < quantity_empl; i++)
	{
		int id;
		char name[10];
		double hours;
		file_input >> id >> name >> hours;
		std::cout << "\nEmployee ID: " << id << "\nEmployee name: " << name << "\nEmployee working hours: " << hours << "\n";

	}
	file_input.close();

	std::cout << "Enetr number of clients:\n";
	std::cin >> quantity_clients;

	hStarted = new HANDLE[quantity_clients];
	hSemaphore = new HANDLE[quantity_empl];
	for (int i = 0; i < quantity_empl; i++)
	{
		hSemaphore[i] = CreateSemaphore(NULL, quantity_clients, quantity_clients, L"hSemahpore");
	}
	for (int i = 0; i < quantity_clients; ++i)
	{
		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		std::string cmd = "Client.exe";
		std::wstring str_to_wstr = std::wstring(cmd.begin(), cmd.end());
		LPWSTR clientCmdLine = &str_to_wstr[0];
		ZeroMemory(&si, sizeof(STARTUPINFO));
		si.cb = sizeof(STARTUPINFO);
		CreateProcess(NULL, clientCmdLine, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
		hStarted[i] = CreateEvent(NULL, FALSE, FALSE, L"Event");
		CloseHandle(pi.hProcess);
	}
	WaitForMultipleObjects(quantity_clients, hStarted, TRUE, INFINITE);
	hChannel = new HANDLE[quantity_clients];
	hThreads = new HANDLE[quantity_clients];

	for (int i = 0; i < quantity_clients; i++)
	{
		hChannel[i] = CreateNamedPipe(L"\\\\.\\channel\\channelName", PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES, 0, 0, INFINITE, NULL);
		if (hChannel == INVALID_HANDLE_VALUE)
		{
			std::cout << "Failed to create named channel.\nLast error code: " << GetLastError() << "\n";
			std::cout << "Click any symbol to shut down the server: ";
			_getch();
			return (0);
		}
		if (!ConnectNamedPipe(hChannel[i], (LPOVERLAPPED)NULL))
		{
			std::cout << "Connection failed.\nLast error code: " << GetLastError() << "\n";
			CloseHandle(hChannel[i]);
			std::cout << "Click any symbol to shut down the server: ";
			_getch();
			return (0);
		}
		hThreads[i] = CreateThread(NULL, 0, operations, static_cast<LPVOID>(hChannel[i]), 0, NULL);

	}
	WaitForMultipleObjects(quantity_clients, hThreads, TRUE, INFINITE);

	std::cout << "All clients have completed their work.";
	file_input.open(fileName);
	for (int i = 0; i < quantity_empl; i++)
	{
		int id;
		char name[10];
		double hours;
		file_input >> id >> name >> hours;
		std::cout << "\nEmployee ID: " << id << "\nEmployee name: " << name << "\nEmployee working hours: " << hours << "\n";

	}
	file_input.close();
	std::cout << "Press any key to exit...\n";
	_getch();
	return 0;
}