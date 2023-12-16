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
int number_of_employees;
int number_of_clients;
std::string file_name;

DWORD WINAPI operations(LPVOID pipe) {
	HANDLE hChannel = (HANDLE)pipe;
	DWORD dwBytesRead; 
	DWORD dwBytesWrite; 

	int message;
	while (true) {
		if (!ReadFile(
			hChannel,
			&message,
			sizeof(message),
			&dwBytesRead,
			NULL))
		{ }
		else {
			int ID = message / 10;
			int chosen_option = message % 10;
			if (chosen_option == 1) {
				for (int i = 0; i < number_of_clients; i++)
				{
					WaitForSingleObject(hSemaphore[ID - 1], INFINITE);
				}
				employee* emp_to_push = new employee();
				emp_to_push->num = empl[ID - 1].num;
				emp_to_push->hours = empl[ID - 1].hours;
				strcpy_s(emp_to_push->name, empl[ID - 1].name);
				bool checker = WriteFile(hChannel, emp_to_push, sizeof(employee), &dwBytesWrite, NULL);
				if (checker) {
					std::cout << "Data to modify was sent.\n";
				}
				else {
					std::cout << "Data to modify wasn't sent.\n";
				}
				ReadFile(hChannel, emp_to_push, sizeof(employee), &dwBytesWrite, NULL);
				empl[ID - 1].hours = emp_to_push->hours;
				strcpy_s(empl[ID - 1].name, emp_to_push->name);
				std::ofstream file;
				file.open(file_name);
				for (int i = 0; i < number_of_employees; i++)
					file << empl[i].num << " " << empl[i].name << " " << empl[i].hours << "\n";
				file.close();
				int msg;
				ReadFile(hChannel, &msg, sizeof(msg), &dwBytesWrite, NULL);
				if (msg == 1) {
					for (int i = 0; i < number_of_clients; i++)
					{
						ReleaseSemaphore(hSemaphore[ID - 1], 1, NULL);
					}
				}
			}
			if (chosen_option == 2) {

				WaitForSingleObject(hSemaphore[ID - 1], INFINITE);
				employee* emp_to_push = new employee();
				emp_to_push->num = empl[ID - 1].num;
				emp_to_push->hours = empl[ID - 1].hours;
				strcpy_s(emp_to_push->name, empl[ID - 1].name);
				bool checker = WriteFile(hChannel, emp_to_push, sizeof(employee), &dwBytesWrite, NULL);
				if (checker) {
					std::cout << "Data to read was sent.\n";
				}
				else {
					std::cout << "Data to read wasn't sent.\n";
				}
				int msg;
				ReadFile(hChannel, &msg, sizeof(msg), &dwBytesWrite, NULL);
				if (msg == 1)
					ReleaseSemaphore(hSemaphore[ID - 1], 1, NULL);
			}
		}
	}
	DisconnectNamedPipe(hChannel);
	CloseHandle(hChannel);

}

int main() {
	std::cout << "Input file name:\n";
	//std::cin >> file_name;
	file_name = "emps.txt";
	std::cout << "Input number of employees:\n";
	//std::cin >> number_of_employees;
	number_of_employees = 3;
	std::ofstream file;
	empl = new employee[number_of_employees];
	for (int i = 0; i < number_of_employees; i++)
	{
		std::cout << "Input " << i + 1 << " employee id:\n";
		std::cin >> empl[i].num;
		std::cout << "Input employee name:\n";
		std::string tmp;
		std::cin >> empl[i].name;
		std::cout << "Input employee hours:\n";
		std::cin >> empl[i].hours;
	}
	file.open(file_name);
	for (int i = 0; i < number_of_employees; i++)
		file << empl[i].num << " " << empl[i].name << " " << empl[i].hours << "\n";
	file.close();
	std::ifstream file_input;
	file_input.open(file_name);
	for (int i = 0; i < number_of_employees; i++)
	{
		int id;
		char name[10];
		double hours;
		file_input >> id >> name >> hours;
		std::cout << "\nID of employee: " << id << ".\nName of employee: " << name << ".\nHours of employee: " << hours << ".\n";

	}
	file_input.close();

	std::cout << "Input number of clients:\n";
	std::cin >> number_of_clients;
	hStarted = new HANDLE[number_of_clients];
	hSemaphore = new HANDLE[number_of_employees];
	for (int i = 0; i < number_of_employees; i++)
	{
		hSemaphore[i] = CreateSemaphore(NULL, number_of_clients, number_of_clients, L"hSemahpore");
	}
	for (int i = 0; i < number_of_clients; ++i)
	{
		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		std::string cmd = "Client.exe";
		std::wstring str_to_wstr = std::wstring(cmd.begin(), cmd.end());
		LPWSTR clientCmdLine = &str_to_wstr[0];
		ZeroMemory(&si, sizeof(STARTUPINFO));
		si.cb = sizeof(STARTUPINFO);
		CreateProcess(NULL, clientCmdLine, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
		hStarted[i] = CreateEvent(NULL, FALSE, FALSE, L"Process Started");
		CloseHandle(pi.hProcess);
	}
	WaitForMultipleObjects(number_of_clients, hStarted, TRUE, INFINITE);
	hChannel = new HANDLE[number_of_clients];
	hThreads = new HANDLE[number_of_clients];
	for (int i = 0; i < number_of_clients; i++)
	{
		hChannel[i] = CreateNamedPipe(L"\\\\.\\pipe\\pipe_name", PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES, 0, 0, INFINITE, NULL);
		if (hChannel == INVALID_HANDLE_VALUE)
		{
			std::cout << "Creation of the named pipe failed.\n The last error code: " << GetLastError() << "\n";
			std::cout << "Press any char to finish server: ";
			_getch();
			return (0);
		}
		if (!ConnectNamedPipe(hChannel[i], (LPOVERLAPPED)NULL))
		{
			std::cout << "The connection failed.\nThe last error code: " << GetLastError() << "\n";
			CloseHandle(hChannel[i]);
			std::cout << "Press any char to finish the server: ";
			_getch();
			return (0);
		}
		hThreads[i] = CreateThread(NULL, 0, operations, static_cast<LPVOID>(hChannel[i]), 0, NULL);

	}
	WaitForMultipleObjects(number_of_clients, hThreads, TRUE, INFINITE);
	std::cout << "All clients has ended their work.";
	file_input.open(file_name);
	for (int i = 0; i < number_of_employees; i++)
	{
		int id;
		char name[10];
		double hours;
		file_input >> id >> name >> hours;
		std::cout << "\nID of employee: " << id << ".\nName of employee: " << name << ".\nHours of employee: " << hours << ".\n";

	}
	file_input.close();
	std::cout << "Press any key to exit...\n";
	_getch();
	return 0;
}