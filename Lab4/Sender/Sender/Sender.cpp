#include <fstream>
#include <windows.h>
#include <conio.h>
#include <iostream>

int main(int argc, char* argv[]) {
    //system("pause");
    std::string fileName = argv[1];
    std::fstream file;
    //HANDLE startEvent = OpenEvent(EVENT_MODIFY_STATE, FALSE, L"event"+argv[2]);

    HANDLE hSemaphore1 = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, L"Semaphore1");
    if (hSemaphore1 == NULL)
        return GetLastError();
    HANDLE hSemaphore2 = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, L"Semaphore2");
    if (hSemaphore2 == NULL)
        return GetLastError();
    HANDLE hMutex = OpenMutex(SYNCHRONIZE, FALSE, L"DemoMutex");
    //SetEvent(startEvent);

    std::cout << "The event has started.";
    std::cout << "Enter 1 to write a message\n Enter 0 to exit the process\n";

    //if (startEvent == NULL)
    //{
    //    std::cout << "Failed to open event.";
    //    std:: cin.get();
    //    return GetLastError();
    //}

    int j;
    std::cin >> j;

    while (true) {
        if (j == 1) {

            WaitForSingleObject(hMutex, INFINITE);
            file.open(fileName, std::ios::out | std::ios::app);
            std::string message;
            std::cout << "Enter message to add";
            std::cin >> message;
            char m[20];

            for (int i = 0; i < message.length(); i++)
            {
                m[i] = message[i];
            }
            for (int i = message.length(); i < 20; i++)
            {
                m[i] = '\0';
            }

            m[19] = '\n';
            ReleaseMutex(hMutex);
            ReleaseSemaphore(hSemaphore2, 1, NULL);

            if (ReleaseSemaphore(hSemaphore1, 1, NULL) != TRUE)
            {
                std::cout << "file is full";
                ReleaseSemaphore(hSemaphore2, 1, NULL);
                WaitForSingleObject(hSemaphore2, INFINITE);
                ReleaseSemaphore(hSemaphore2, 1, NULL);
                ReleaseSemaphore(hSemaphore1, 1, NULL);
                for (int i = 0; i < 20; i++)
                {
                    file << m[i];
                }

            }
            else {
                for (int i = 0; i < 20; i++)
                {
                    file << m[i];
                }
            }
            file.close();

            std::cout << "Enter 1 to write a message\n Enter 0 to exit the process\n";
            std::cin >> j;
        }
        if (j != 0 && j != 1) {
            std::cout << "\nIncorrect value!\n";
            std::cin >> j;
        }
        if (j == 0) {
            std::cout << "The process has completed.";
            break;
        }
    }

    return 0;
}