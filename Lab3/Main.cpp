#include <iostream>
#include <vector>
#include <windows.h>
#include <mutex>

using namespace std;
HANDLE hMutex;
struct MarkerThreadData
{
    int index;     
    int* arr;    
    int size;      
   
    bool flag = false;
    HANDLE startEvent;     
    HANDLE continueEvent;  
    HANDLE stopEvent;      
};


DWORD WINAPI MarkerThread(LPVOID lpParam)
{
    MarkerThreadData* data = (MarkerThreadData*)lpParam;
    int index = data->index;
    int* array = data->arr;
    int size = data->size;
    HANDLE startEvent = data->startEvent;
    HANDLE continueEvent = data->continueEvent;
    HANDLE stopEvent = data->stopEvent;
    int a=0;
    //mutex mtx;

 
    WaitForSingleObject(startEvent, INFINITE);

    srand(index); 

    while (true)
    {
        int randomNum = rand();
        int indexToMark = randomNum % size;
        //mutex enter;
        WaitForSingleObject(hMutex, INFINITE);
        if (array[indexToMark] == 0)
        {
            Sleep(5);
            array[indexToMark] = index;
            a++;
            Sleep(5);
            ReleaseMutex(hMutex);
           
        }
        else
        {
            //mtx.unlock();
            ReleaseMutex(hMutex);
            cout << "Поток marker " << index << " пометил " <<a<< array[index] << " элементов." << endl;
            cout << "Невозможно пометить элемент с индексом " << indexToMark << "." << endl;

            // Cигнал потоку main о невозможности продолжения работы
            SetEvent(stopEvent);

            // Ожидание ответного сигнала от потока main для продолжения или завершения работы
            WaitForSingleObject(continueEvent, INFINITE);

            if (data->flag == true) // Получен сигнал на завершение работы
            {
                // Заполнение нулями помеченных элементов в массиве
                for (int i = 0; i < size; i++)
                {
                    if (array[i] == index)
                    {
                        array[i] = 0;
                    }
                }

                break; // Завершение работы потока marker
            }
        }
        //mtx.unlock();
    }

    return 0;
}

int main()
{
    setlocale(LC_ALL, "Russian");
    int size;
    cout << "Введите размерность массива: ";
    cin >> size;
    hMutex = OpenMutex(SYNCHRONIZE, FALSE, L"DemoMutex");
    // Захват памяти под массив целых чисел и инициализация элементов нулями
    int* array = new int[size]();

    int numMarkers;
    cout << "Введите количество потоков marker: ";
    cin >> numMarkers;

    // Создание событий для управления потоками marker
    HANDLE* startEvents = new HANDLE[numMarkers];
    HANDLE* continueEvents = new HANDLE[numMarkers];
    HANDLE* stopEvents = new HANDLE[numMarkers];

    // Создание и запуск потоков marker
    vector<HANDLE> markerThreads(numMarkers);
    vector<MarkerThreadData> markerThreadData(numMarkers);

    for (int i = 0; i < numMarkers; i++)
    {
        // Создание событий для каждого потока marker
        startEvents[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
        continueEvents[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
        stopEvents[i] = CreateEvent(NULL, FALSE, FALSE, NULL);

        // Подготовка данных для каждого потока marker
        markerThreadData[i].index = i;
        markerThreadData[i].arr = array;
        markerThreadData[i].size = size;
        markerThreadData[i].startEvent = startEvents[i];
        markerThreadData[i].continueEvent = continueEvents[i];
        markerThreadData[i].stopEvent = stopEvents[i];

        // Запуск потока marker
        markerThreads[i] = CreateThread(NULL, 0, MarkerThread, &markerThreadData[i], 0, NULL);
    }
        // Дать сигнал на начало работы всем потокам marker
    for (int i = 0; i < numMarkers; i++)
    {
        SetEvent(startEvents[i]);
    }

    while (true)
    {
        for (int i = 0; i < numMarkers; i++)
            if (markerThreadData[i].flag == true) SetEvent(stopEvents[i]);
        // Ждать, пока все потоки marker не подадут сигналы о невозможности продолжения своей работы
        DWORD result = WaitForMultipleObjects(numMarkers, stopEvents, TRUE, INFINITE);

        // Вывести содержимое массива на консоль
        for (int i = 0; i < size; i++)
        {
            cout << array[i] << " ";
        }
        cout << endl;
            
        int markerIndex;
        cout << "Введите порядковый номер потока marker, которому будет подан сигнал на завершение: ";
        cin >> markerIndex;

        // Подать потоку marker, номер которого получен, сигнал на завершение работы
        markerThreadData[markerIndex].flag = true;
        SetEvent(continueEvents[markerIndex]);

        // Ждать завершение работы потока marker
        WaitForSingleObject(markerThreads[markerIndex], INFINITE);

        // Вывести содержимое массива на консоль
        for (int i = 0; i < size; i++)
        {
            cout << array[i] << " ";
        }
        cout << endl;

        // Подать сигнал на продолжение работы оставшимся потокам marker
        for (int i = 0; i < numMarkers; i++)
        {
            if (i != markerIndex)
            {
                SetEvent(continueEvents[i]);
            }
        }

        // Проверить, завершились ли все потоки marker
        DWORD exitCode;
        bool allThreadsExited = true;

        for (int i = 0; i < numMarkers; i++)
        {
            GetExitCodeThread(markerThreads[i], &exitCode);
            if (exitCode == STILL_ACTIVE)
            {
                allThreadsExited = false;
                break;
            }
        }

        if (allThreadsExited)
        {
            break; // Завершение работы потока main
        }
    }

    // Освобождение ресурсов
    delete[] array;
    delete[] startEvents;
    delete[] continueEvents;
    delete[] stopEvents;
    return 0;
}
    
