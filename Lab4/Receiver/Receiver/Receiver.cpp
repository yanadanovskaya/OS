#include <fstream>
#include <iostream>
#include <windows.h>
#include <conio.h>
#include <string>

int main() {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    LPWSTR lpwstr;
    
    std::string fileName;
    std::fstream file;
    int number_of_records;
    int number_of_senderpr;

    std::cout << "Enter the name of the binary file:\n";
    std::cin >> fileName;    
    std::cout << "Enter the number of records in the binary file:\n";
    std::cin >> number_of_records;
    file.open(fileName, std::ios::out);
    file.close();

    std::cout << "Enter the number of Sender processes:\n";
    std::cin >> number_of_senderpr;
    

    HANDLE hSemaphore1 = CreateSemaphore(NULL, 0, number_of_records, L"Semaphore1");
    if (hSemaphore1 == NULL)
        return GetLastError();
    HANDLE hSemaphore2 = CreateSemaphore(NULL, 0, number_of_records, L"Semaphore2");
    if (hSemaphore2 == NULL)
        return GetLastError();
    HANDLE hMutex = CreateMutex(NULL, TRUE, L"DemoMutex");
   /* HANDLE* hEventStarted = new HANDLE[number_of_senderpr];*/

    for (int i = 0; i < number_of_senderpr; ++i)
    {
        std::string senderName = "C:\\Users\\Yana\\source\\OP\\Lab4\\Receiver\\x64\\Debug\\Sender.exe " + fileName;
        std::wstring converting_to_lpwstr = std::wstring(senderName.begin(), senderName.end());
        lpwstr = &converting_to_lpwstr[0];
        ZeroMemory(&si, sizeof(STARTUPINFO));
        si.cb = sizeof(STARTUPINFO);
        if (!CreateProcess(NULL, lpwstr, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
            std::cout << "Sender process isn't started.\n";
            return GetLastError();
        }
       /* hEventStarted[i] = CreateEvent(NULL, FALSE, FALSE, L"event"+i);
        if (hEventStarted[i] == NULL) {
            return GetLastError();
            CloseHandle(pi.hProcess);
        }*/
    }

    /*WaitForMultipleObjects(number_of_senderpr, hEventStarted, TRUE, INFINITE);*/
    ReleaseMutex(hMutex);
    
    int counter = 0;
    file.open(fileName, std::ios::in);

    while (true) {
        std::cout << "\nEnter 1 to read the message\nEnter 0 to exit the process\n";
        int j;
       
        std::cin >> j;
        if (j == 1) {
            std::string msg;
            WaitForSingleObject(hSemaphore1, INFINITE);
            WaitForSingleObject(hMutex, INFINITE);
            std::getline(file, msg);
            std::cout << msg;
            ReleaseSemaphore(hSemaphore2, 1, NULL);
            ReleaseMutex(hMutex);
         
            /*std::cin >> j;
            std::cout << "\nEnter 1 to read the message\nEnter 0 to exit the process\n";           */ 
        }
        if (j != 0 && j != 1) {
            std::cin >> j;
            std::cout << "\nIncorrect value!\n";
        }
        if (j == 0) {
            std::cout << "The process has completed.";
            break;
        }
    }
    CloseHandle(hSemaphore1);
    CloseHandle(hSemaphore2);
    /*for (int i = 0; i < number_of_senderpr; i++)
    {
        CloseHandle(hEventStarted[i]);
    }*/

    return 0;
}