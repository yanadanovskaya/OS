
#include <iostream>
#include <vector>
#include <windows.h>
#include <algorithm>

// Глобальные переменные
HANDLE hMinMaxThread;
HANDLE hAverageThread;

// Функция потока min max
DWORD WINAPI minMaxThread(LPVOID lpParam) {
    std::vector<int>& numbers = *static_cast<std::vector<int>*>(lpParam);

    int minVal = numbers[0];
    int maxVal = numbers[0];

    for (const auto& num : numbers) {
        if (num < minVal) {
            minVal = num;
        }
        Sleep(7);
        if (num > maxVal) {
            maxVal = num;
        }

        Sleep(7);
    }
    
    std::cout << "\nМинимальное значение: " << minVal << std::endl;
    std::cout << "Максимальное значение: " << maxVal << std::endl;

    return 0;
}

// Функция потока average
DWORD WINAPI averageThread(LPVOID lpParam) {
    std::vector<int>& numbers = *static_cast<std::vector<int>*>(lpParam);

    double sum = 0.0;

    for (const auto& num : numbers) {//конст ссылка
        sum += num;
        Sleep(12);
    }

    double average = sum / numbers.size();
    std::cout << "\nСреднее значение: " << average << std::endl;

    return 0;
}

int main() {
    // Ввод размерности массива
    int arraySize;
    setlocale(LC_ALL, "Rus");
    std::cout << "Введите размерность массива: ";
    std::cin >> arraySize;
    

    // Ввод элементов массива
    std::vector<int> numbers(arraySize);
    for (int i = 0; i < arraySize; ++i) {
        std::cout << "Введите элементы массива: " << i << ": ";
        std::cin >> numbers[i];
    }

    // Создание потоков min max и average
    hMinMaxThread = CreateThread(NULL, 0, minMaxThread, &numbers, 0, NULL);
    hAverageThread = CreateThread(NULL, 0, averageThread, &numbers, 0, NULL);

    // Ожидание завершения потоков min max и average
    WaitForSingleObject(hMinMaxThread, INFINITE);
    WaitForSingleObject(hAverageThread, INFINITE);

    // Замена максимального и минимального элементов на среднее значение
    auto minMaxIt = std::minmax_element(numbers.begin(), numbers.end());
    double averageVal = (static_cast<double>(*minMaxIt.first) + *minMaxIt.second) / 2;
    *minMaxIt.first = static_cast<int>(averageVal);
    *minMaxIt.second = static_cast<int>(averageVal);

    // Вывод результатов
    std::cout << "\nВидоизменённый массив: ";
    for (const auto& num : numbers) {
        std::cout << num << " ";
    }
    std::cout << std::endl;

    // Закрытие дескрипторов потоков
    CloseHandle(hMinMaxThread);
    CloseHandle(hAverageThread);

    return 0;
}

