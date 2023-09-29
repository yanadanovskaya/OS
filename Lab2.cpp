#include <iostream>
#include <thread>
#include <chrono>

using namespace std;

int arraySize;
int* array;

void findMinMax();
void findAverage();

int main() {

    cout << "Enter array size: ";
    cin >> arraySize;

    array = new int[arraySize];

    for (int i = 0; i < arraySize; i++) {
        cout << "Enter element " << i << ": ";
        cin >> array[i];
    }

    thread minMaxThread(findMinMax);
    thread averageThread(findAverage);

    minMaxThread.join();
    averageThread.join();

    int min = 0;
    int max = 0;
    double average = 0;

    // get results from threads

    array[min] = average;
    array[max] = average;

    cout << "Modified array: ";
    for (int i = 0; i < arraySize; i++) {
        cout << array[i] << " ";
    }
    cout << endl;

    return 0;
}

void findMinMax() {

    int min = array[0];
    int max = array[0];

    for (int i = 1; i < arraySize; i++) {

        if (array[i] < min) {
            min = array[i];
        }
        if (array[i] > max) {
            max = array[i];
        }

        this_thread::sleep_for(chrono::milliseconds(500));
    }

    cout << "Min: " << min << endl;
    cout << "Max: " << max << endl;

}

void findAverage() {

    int sum = 0;

    for (int i = 0; i < arraySize; i++) {

        sum += array[i];

        this_thread::sleep_for(chrono::milliseconds(12));
    }

    double average = static_cast<double>(sum) / arraySize;

    cout << "Average: " << average << endl;

}
