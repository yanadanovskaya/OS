#include <iostream>
#include <thread>
#include <chrono>

using namespace std;

int arraySize;
int* arr;

int main();

void findMinMax();
void findAverage();

int main() {

    cout << "Enter array size: ";
    cin >> arraySize;

    arr = new int[arraySize];

    for (int i = 0; i < arraySize; i++) {
        cout << "Enter element " << i << ": ";
        cin >> arr[i];
    }

    thread minMaxThread(findMinMax);
    thread averageThread(findAverage);

    minMaxThread.join();
    averageThread.join();

    int min = 0;
    int max = 0;
    double average = 0;

    // get results from threads

    arr[min] = average;
    arr[max] = average;

    cout << "Modified array: ";
    for (int i = 0; i < arraySize; i++) {
        cout << arr[i] << " ";
    }
    cout << endl;

    return 0;
}

void findMinMax() {

    int min = arr[0];
    int max = arr[0];

    for (int i = 1; i < arraySize; i++) {

        if (arr[i] < min) {
            min = arr[i];
        }
        if (arr[i] > max) {
            max = arr[i];
        }

        this_thread::sleep_for(chrono::milliseconds(500));
    }

    cout << "Min: " << min << endl;
    cout << "Max: " << max << endl;

}

void findAverage() {

    int sum = 0;

    for (int i = 0; i < arraySize; i++) {

        sum += arr[i];

        this_thread::sleep_for(chrono::milliseconds(12));
    }

    double average = static_cast<double>(sum) / arraySize;

    cout << "Average: " << average << endl;

}
