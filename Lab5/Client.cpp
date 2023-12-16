#include <conio.h>
#include <windows.h>
#include <iostream>

struct employee
{
    int num; 
    char name[10]; 
    double hours; 
};

int main(int argc, char* argv) {
    HANDLE hStartEvent = OpenEvent(EVENT_MODIFY_STATE, FALSE, L"Process");
    if (hStartEvent == NULL)
    {
        std::cout << "Failed to open event. \nPress any key to exit.\n";
        std::cin.get();
        return GetLastError();
    }
    SetEvent(hStartEvent);

    HANDLE hChannel = CreateFile(L"\\\\.\\chanel\\channel_name", GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (hChannel == INVALID_HANDLE_VALUE)
    {
        std::cout << "Failed to create named channel.\nLast error code: " << GetLastError() << "\n";
        std::cout << "Press any key to shut down the server : ";
        _getch();
        return (0);
    }
    while (true) {
        int selection = 0;
        std::cout << "Select an option:\n 1. Modify file record;\n 2. Read record;\n 3. Exit.\n";
        std::cin >> selection;
        if (selection == 3) {
            break;
        }
        else if (selection == 1) {
            DWORD record;
            DWORD reading;
            int ID;
            std::cout << "Enter employee ID:\n";
            std::cin >> ID;
            int j = ID * 10 + selection;
            bool n = WriteFile(
                hChannel, 
                &j, 
                sizeof(j), 
                &record,
                NULL);
            if (n) {
                std::cout << "Message sent.\n";
            }
            else {
                std::cout << "Message not sent.\n";
            }
            employee* empl = new employee();
            ReadFile(
                hChannel, 
                empl, 
                sizeof(employee), 
                &reading,
                NULL);

            std::cout << "Employee ID: " << empl->num << "\nEmployee name: " << empl->name << "\nEmployee working hours: " << empl->hours << "\n";
            std::cout << "Enter a new name:\n";
            std::cin >> empl->name;
            std::cout << "Enter new hours:\n";
            std::cin >> empl->hours;
            n = WriteFile(
                hChannel, 
                empl, 
                sizeof(employee), 
                &record,
                NULL);
            if (n) {
                std::cout << "Message sent.\n";
            }
            else {
                std::cout << "Message not sent.\n";
            }
            std::cout << "Press any key to confirm your selection\n";
            _getch();
            j = 1;
            WriteFile(hChannel, &j, sizeof(j), &record, NULL);
        }
        else if (selection == 2) {
            DWORD record;
            DWORD reading;
            int ID;
            std::cout << "Input an ID of employee:\n";
            std::cin >> ID;
            int j = ID * 10 + selection;
            bool checker = WriteFile(
                hChannel, 
                &j, 
                sizeof(j), 
                &record, 
                NULL);
            if (checker) {
                std::cout << "Message sent.\n";
            }
            else {
                std::cout << "Message not sent.\n";
            }
            employee* empl = new employee();
            ReadFile(
                hChannel, 
                empl, 
                sizeof(employee), 
                &reading, 
                NULL);

            std::cout << "Employee ID: " << empl->num << "\nEmployee name: " << empl->name << "\nEmployee working hours: " << empl->hours << "\n";
            std::cout << "Press any key to confirm your selection\n";
            _getch();
            j = 1;
            WriteFile(hChannel, &j, sizeof(j), &record, NULL);
        }

    }



    return 0;
}