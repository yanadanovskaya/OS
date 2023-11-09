#include <iostream>
#include <string>
#include <vector>
#include <windows.h>

const int MAX_MESSAGE_LENGTH = 20;

void senderProcess(const std::string& messageFile) {
    std::string message;
    std::cout << "Введите сообщение для передачи (максимум " << MAX_MESSAGE_LENGTH << " символов): ";
    std::cin.ignore();
    std::getline(std::cin, message);

    // Открываем событие для сигнализации готовности
    HANDLE eventHandle = OpenEvent(EVENT_MODIFY_STATE, FALSE, L"MessageEvent");
    if (eventHandle == NULL) {
        std::cerr << "Ошибка открытия события" << std::endl;
        exit(1);
    }

    int messageLength = message.length();
    if (messageLength > MAX_MESSAGE_LENGTH) {
        messageLength = MAX_MESSAGE_LENGTH;
    }
    message.resize(MAX_MESSAGE_LENGTH, ' ');
    HANDLE fileHandle = CreateFile(messageFile.c_str(), GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (fileHandle == INVALID_HANDLE_VALUE) {
        std::cerr << "Ошибка создания/открытия файла" << std::endl;
        CloseHandle(eventHandle);
        exit(1);
    }
    DWORD bytesWritten;
    WriteFile(fileHandle, message.c_str(), MAX_MESSAGE_LENGTH, &bytesWritten, NULL);
    CloseHandle(fileHandle);

    // Сигнализируем о готовности
    SetEvent(eventHandle);

    CloseHandle(eventHandle);
}

void receiverProcess(const std::string& messageFile, int messageCount) {

    HANDLE fileHandle = CreateFile(messageFile.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (fileHandle == INVALID_HANDLE_VALUE) {
        std::cerr << "Ошибка создания файла" << std::endl;
        exit(1);
    }
    DWORD bytesWritten;
    std::string emptyMessage(MAX_MESSAGE_LENGTH, ' ');
    for (int i = 0; i < messageCount; ++i) {
        WriteFile(fileHandle, emptyMessage.c_str(), MAX_MESSAGE_LENGTH, &bytesWritten, NULL);
    }
    CloseHandle(fileHandle);

    // событие для сигнализации готовности
    HANDLE eventHandle = CreateEvent(NULL, TRUE, FALSE, L"MessageEvent");
    if (eventHandle == NULL) {
        std::cerr << "Ошибка создания события" << std::endl;
        exit(1);
    }

    while (true) {
        char command;
        std::cout << "Введите команду (r - чтение сообщения, q - завершение работы): ";
        std::cin >> command;

        if (command == 'r') {
            // Ожидаем событие готовности
            WaitForSingleObject(eventHandle, INFINITE);

            fileHandle = CreateFile(messageFile.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            if (fileHandle == INVALID_HANDLE_VALUE) {
                std::cerr << "Ошибка открытия файла" << std::endl;
                CloseHandle(eventHandle);
                exit(1);
            }
            std::string message;
            DWORD bytesRead;
            for (int i = 0; i < messageCount; ++i) {
                ReadFile(fileHandle, &message[0], MAX_MESSAGE_LENGTH, &bytesRead, NULL);
                if (bytesRead > 0) {
                    std::cout << "Прочитано сообщение: " << message << std::endl;
                }
                else {
                    std::cout << "Бинарный файл пуст." << std::endl;
                    break;
                }
            }
            CloseHandle(fileHandle);
        }
        else if (command == 'q') {
   
            CloseHandle(eventHandle);
            break;
        }
        else {
            std::cout << "Неверная команда. Повторите попытку.";
        }
    }
}

int main() {
    std::string messageFile;
    int messageCount;
    int senderCount;

    std::cout << "Введите имя бинарного файла: ";
    std::cin >> messageFile;

    std::cout << "Введите количество записей в бинарном файле: ";
    std::cin >> messageCount;

    std::cout << "Введите количество процессов Sender: ";
    std::cin >> senderCount;

    // Запускаем процессы Sender
    std::vector<PROCESS_INFORMATION> senderProcesses(senderCount);
    for (int i = 0; i < senderCount; ++i) {
        std::string commandLine = "SenderProcess.exe " + messageFile;
        STARTUPINFO startupInfo;
        ZeroMemory(&startupInfo, sizeof(STARTUPINFO));
        startupInfo.cb = sizeof(STARTUPINFO);
        if (!CreateProcess(NULL, &commandLine[0], NULL, NULL, FALSE, 0, NULL, NULL, &startupInfo, &senderProcesses[i])) {
            std::cerr << "Ошибка создания процесса Sender" << std::endl;
            exit(1);
        }
    }

    // Запускаем процесс Receiver
    receiverProcess(messageFile, messageCount);

    // Ожидаем завершения процессов Sender
    for (int i = 0; i < senderCount; ++i) {
        WaitForSingleObject(senderProcesses[i].hProcess, INFINITE);
        CloseHandle(senderProcesses[i].hProcess);
        CloseHandle(senderProcesses[i].hThread);
    }

    return 0;
}
