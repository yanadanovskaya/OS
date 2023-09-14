#include "Employee.h"
#include <string>
#include <fstream>
#include <iostream>

int main(int argc, char** argv) {
	employee empl;
	//std::string filename = argv[1];
	std::cout << "Passed " << argc << " args\n";

	int n = std::atoi(argv[2]);//переход к инту
	std::string filename = "binary.bin";
	std::ofstream file("binary.bin", std::ios::binary);//поток для записи
	//file.open("binary.bin", std::ios::binary);//открываем файл для записи
	if (file.is_open())
	{
		for (int i = 0; i < n; i++) {
			std::cout << "Enter employee" << std::endl;
			std::cin >> empl.num >> empl.name >> empl.hours;
			file.write((char*)&empl,n);
		}
	}
	file.close();
	std::cout << "File has been written" << std::endl;

}