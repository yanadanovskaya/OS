#include <iostream>
#include <string>
#include <windows.h>

const int MAX_MESSAGE_LENGTH = 20;

void senderProcess(const std::string& messageFile) {
   
    HANDLE fileHandle = CreateFile(messageFile.c_str(), GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (fileHandle == INVALID_HANDLE_VALUE) {
        std::cerr << "Ошибка открытия файла" << std::endl;
        exit(1);
    }

    // Открываем событие для сигнализации готовности
    HANDLE eventHandle = OpenEvent(EVENT_MODIFY_STATE, FALSE, L"MessageEvent");
    if (eventHandle == NULL) {
        std::cerr << "Ошибка открытия события" << std::endl;
        CloseHandle(fileHandle);
        exit(1);
    }

    bool running = true;
    while (running) {
        char command;
        std::cout << "Введите команду (m - отправить сообщение, q - завершить работу): ";
        std::cin >> command;

        if (command == 'm') {
            // Ожидаем событие готовности от Receiver
            WaitForSingleObject(eventHandle, INFINITE);

            std::string message;
            std::cout << "Введите сообщение для отправки (максимум " << MAX_MESSAGE_LENGTH << " символов): ";
            std::cin.ignore();
            std::getline(std::cin, message);
            if (message.length() > MAX_MESSAGE_LENGTH) {
                message.resize(MAX_MESSAGE_LENGTH);
            }

            // Записываем сообщение в бинарный файл
            DWORD bytesWritten;
            WriteFile(fileHandle, message.c_str(), MAX_MESSAGE_LENGTH, &bytesWritten, NULL);
            SetEndOfFile(fileHandle);

            // Сигнализируем готовность Receiver
            SetEvent(eventHandle);
        } else if (command == 'q') {
            running = false;
        } else {
            std::cout << "Неверная команда. Повторите попытку." << std::endl;
        }
    }

    CloseHandle(eventHandle);
    CloseHandle(fileHandle);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Необходимо указать имя бинарного файла" << std::endl;
        return 1;
    }

    std::string messageFile = argv[1];

    senderProcess(messageFile);

    return 0;
}
