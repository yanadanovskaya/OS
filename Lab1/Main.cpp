﻿#include <iostream>
#include <fstream>
#include <windows.h>
#include <conio.h>
#include "Employee.h"
#include <vector>
#include <string>

int main() {
	std::string bin_name;
	std::string txt_name;
	std::string salary;
	std::string empl_number;
	std::ifstream fileInput;
	std::ifstream fileOutput;
	std::vector<employee> emps;
	employee empl;

	std::cout << "Input binary file name: ";
	std::cin >> bin_name;
	std::cout << "Input employee number: ";
	std::cin >> empl_number;

	std::string creator = "Creator.exe " + bin_name + " " + empl_number;
	std::wstring converting_to_lpwstr = std::wstring(creator.begin(), creator.end());
	wchar_t* data0 = const_cast<wchar_t*>(converting_to_lpwstr.c_str());
	LPWSTR CreatorProcessCommand = data0;//
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	std::cout << creator << "\n";

	if (!CreateProcess(NULL, CreatorProcessCommand, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
		std::cout << "The Creator process not activated.";
		return GetLastError();
	}
	WaitForSingleObject(pi.hProcess, INFINITE);

	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);

	fileInput.open(bin_name, std::ios::binary);
	if (fileInput.is_open()) {
		while (!fileInput.eof()) {
			fileInput.read((char*)&empl, sizeof(employee));
			emps.push_back(empl);
		}
		emps.pop_back();
	}
	std::cout << "Binary file: \nname\tnumber\thours\n";
	for (size_t i = 0; i < emps.size(); i++)
	{
		std::cout << emps[i].num;
		std::cout << emps[i].name;
		std::cout << emps[i].hours;
	}


	std::cout << "Input txt file name:\n"; std::cin >> txt_name;
	std::cout << "Input payment for working hours:\n"; std::cin >> salary;
	std::wstring reporter = L"Reporter.exe " + std::wstring(bin_name.begin(), bin_name.end()) + L" "
		+ std::wstring(txt_name.begin(), txt_name.end()) + L" " + std::wstring(salary.begin(), salary.end());

	//std::wstring converting_to_lpwstr = std::wstring(reporter.begin(), reporter.end());
	wchar_t* data = const_cast<wchar_t*>(reporter.c_str());
	LPWSTR ReporterProcessCommand = data;

	if (!CreateProcess(NULL, ReporterProcessCommand, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
		std::cout << "The Reporter  process not activated.";
		return GetLastError();
	}
	WaitForSingleObject(pi.hProcess, INFINITE);

	std::cout << "Reporter process was terminated.";
	fileOutput.open(txt_name);
	if (fileOutput.is_open()) {
		while (!fileOutput.eof()) {
			std::string t;
			getline(fileOutput, t);
			std::cout << t;
		}
	}
	fileOutput.close();
	_getch();
	TerminateProcess(pi.hProcess, 1);
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
	std::cout << "Main process was terminated.\n";
	_getch();
	return 0;
}