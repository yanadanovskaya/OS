#include <iostream>
#include <vector>
#include <windows.h>
#include <algorithm>

HANDLE hMinMaxThread;
HANDLE hAverageThread;

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

DWORD WINAPI averageThread(LPVOID lpParam) {
    std::vector<int>& numbers = *static_cast<std::vector<int>*>(lpParam);

    double sum = 0.0;

    for (const auto& num : numbers) {
        sum += num;
        Sleep(12);
    }

    double average = sum / numbers.size();
    std::cout << "\nСреднее значение: " << average << std::endl;

    return 0;
}

int main() {
    
    int arraySize;
    setlocale(LC_ALL, "Rus");
    std::cout << "Введите размерность массива: ";
    std::cin >> arraySize;
    

    std::vector<int> numbers(arraySize);
    for (int i = 0; i < arraySize; ++i) {
        std::cout << "Введите элементы массива: " << i << ": ";
        std::cin >> numbers[i];
    }

    hMinMaxThread = CreateThread(NULL, 0, minMaxThread, &numbers, 0, NULL);
    hAverageThread = CreateThread(NULL, 0, averageThread, &numbers, 0, NULL);

    WaitForSingleObject(hMinMaxThread, INFINITE);
    WaitForSingleObject(hAverageThread, INFINITE);

    auto minMaxIt = std::minmax_element(numbers.begin(), numbers.end());
    double averageVal = (static_cast<double>(*minMaxIt.first) + *minMaxIt.second) / 2;
    *minMaxIt.first = static_cast<int>(averageVal);
    *minMaxIt.second = static_cast<int>(averageVal);

    std::cout << "\nВидоизменённый массив: ";
    for (const auto& num : numbers) {
        std::cout << num << " ";
    }
    std::cout << std::endl;

    CloseHandle(hMinMaxThread);
    CloseHandle(hAverageThread);

    return 0;
}

