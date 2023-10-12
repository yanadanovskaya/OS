#include <iostream>
#include <thread>
#include <vector>
#include <mutex>

std::vector<int> array;
std::mutex mtx;
bool continueExecution = true;

void markerThread(int threadNum) {
    std::srand(threadNum);
    while (continueExecution) {
        int randomIndex = std::rand() % array.size();
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        std::lock_guard<std::mutex> lock(mtx);
        if (array[randomIndex] == 0) {
            std::cout << "Marker thread " << threadNum << " marked element at index " << randomIndex << std::endl;
            array[randomIndex] = threadNum;
        }
        else {
            std::cout << "Marker thread " << threadNum << " found marked element at index " << randomIndex << std::endl;
            continueExecution = false;
        }
    }
}

int main() {
    int arraySize;
    std::cout << "Enter the size of the array: ";
    std::cin >> arraySize;
    array.resize(arraySize);

    int numThreads;
    std::cout << "Enter the number of marker threads: ";
    std::cin >> numThreads;

    std::vector<std::thread> threads;
    for (int i = 0; i < numThreads; i++) {
        threads.push_back(std::thread(markerThread, i));
    }

    std::cout << "Press any key to start the marker threads..." << std::endl;
    std::cin.ignore();
    std::cin.get();

    std::cout << "Marker threads started!" << std::endl;

    while (continueExecution) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::lock_guard<std::mutex> lock(mtx);
        std::cout << "Array contents: ";
        for (int num : array) {
            std::cout << num << " ";
        }
        std::cout << std::endl;

        int threadToTerminate;
        std::cout << "Enter the thread number to terminate: ";
        std::cin >> threadToTerminate;

        if (threadToTerminate >= 0 && threadToTerminate < numThreads) {
            continueExecution = false;
            threads[threadToTerminate].join();
            array.assign(arraySize, 0);
            continueExecution = true;
        }
    }

    std::cout << "Marker threads terminated!" << std::endl;

    for (std::thread& thread : threads) {
        thread.join();
    }

    return 0;
}
