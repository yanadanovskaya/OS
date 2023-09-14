#include "Employee.h"
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <conio.h>
#include <windows.h>

int main(int argc, char** argv) {
	employee empl;
	std::string inputName = argv[1];
	std::string outputName = argv[2];
	int salary = (int)argv[3];
	std::ifstream input(inputName, std::ios::binary);
	std::ofstream output(outputName);;
	if (output.is_open()) {
		output << "Report to the file " << inputName << "\n";

		output << "NUMBER\tNAME\tHOURS\tSALARY\n";
		
	}
	else {
		return -1;
	}
	if (input.is_open()) {
		while (!input.eof()) {
			input.read((char*)&empl, sizeof(empl));
			output << empl.num << "\t" << empl.name << "\t" << empl.hours
				<< "\t" << salary * empl.hours << "\n";
		}
	}
	input.close();

	output.close();
	Sleep(100);
	return 0;
}