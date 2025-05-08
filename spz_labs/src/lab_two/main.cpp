#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
// Структура для передачі аргументів у функцію потоку
struct ThreadArgs {
    int array_size; // Розмір масиву
};
// Функція потоку
DWORD WINAPI multMatrix(LPVOID lpParam) {
    struct ThreadArgs *threadArgs = (struct ThreadArgs *)lpParam;
    int size = threadArgs->array_size;
    // Генерування динамічного масиву з випадковими значеннями
    char *a = (char *)malloc(size * sizeof(*a));
    for (int i = 0; i < size; i++) {
        a[i] = rand() % 10;
    }
    int max_index = -1;
    if (size > 0)
        max_index = 0;

    for (int i = 0; i < size; i++) {
        if (a[i] > a[max_index])
            max_index = i;
    }

    free(a);

    return 0;
}
// Функція для друку результатів
void printRes(HANDLE *hThread) {
    printf("-------------------------------------------------------------------"
           "----------------------------------------------------\n");
    printf(
        "|  Thread  | Kernel Time, | User Time, | Execution Time, | Thread | "
        "Thread | Thread Cycle |\n");
    printf("| Priority | milliseconds | milliseconds | milliseconds | creation "
           "time | end time | Time, tacts |\n");
    printf("-------------------------------------------------------------------"
           "----------------------------------------------------\n");
    for (int i = 0; i < 7; i++) {
        int p = GetThreadPriority(hThread[i]);
        printf("| %3d |", p);
        // Визначення часу виконання потоку за допомогою GetThreadTimes()
        FILETIME creationTime, exitTime, kernelTime, userTime;
        FILETIME creationLocalTime, exitLocalTime;
        if (GetThreadTimes(hThread[i], &creationTime, &exitTime, &kernelTime,
                           &userTime)) {
            ULARGE_INTEGER kernelTimeInt, userTimeInt;
            kernelTimeInt.LowPart = kernelTime.dwLowDateTime;
            kernelTimeInt.HighPart = kernelTime.dwHighDateTime;
            userTimeInt.LowPart = userTime.dwLowDateTime;
            userTimeInt.HighPart = userTime.dwHighDateTime;
            printf(" %13.5f |", kernelTimeInt.QuadPart * 1e-4);
            printf(" %15.5f |", userTimeInt.QuadPart * 1e-4);
            ULARGE_INTEGER creationTimeInt, exitTimeInt;
            creationTimeInt.LowPart = creationTime.dwLowDateTime;
            creationTimeInt.HighPart = creationTime.dwHighDateTime;
            exitTimeInt.LowPart = exitTime.dwLowDateTime;
            exitTimeInt.HighPart = exitTime.dwHighDateTime;
            double exeTime =
                (exitTimeInt.QuadPart - creationTimeInt.QuadPart) * 1e-4;
            printf(" %16.5f |", exeTime);
            SYSTEMTIME systemTime;
            FileTimeToLocalFileTime(&creationTime, &creationLocalTime);
            FileTimeToLocalFileTime(&exitTime, &exitLocalTime);
            FileTimeToSystemTime(&creationLocalTime, &systemTime); //
            printf(" %02u:%02u:%02u:%03u |", systemTime.wHour,
                   systemTime.wMinute, systemTime.wSecond,
                   systemTime.wMilliseconds);
            FileTimeToSystemTime(&exitLocalTime, &systemTime); // Перетворення
            printf(" %02u:%02u:%02u:%03u |", systemTime.wHour,
                   systemTime.wMinute, systemTime.wSecond,
                   systemTime.wMilliseconds);
            ULONGLONG ThreadCycleTime;
            QueryThreadCycleTime(hThread[i], &ThreadCycleTime);
            printf("%14llu |\n", ThreadCycleTime);
            printf(
                "--------------------------------------------------------------"
                "---------------------------------------------------------\n");
        }
    }
}
void ParallelThreadExecution(int n) {
    struct ThreadArgs threadArgs;
    threadArgs.array_size = n;
    HANDLE hThread[7];
    DWORD dwThreadId[7];
    int i;
    // Створення потоків
    for (i = 0; i < 7; i++) {
        hThread[i] =
            CreateThread(NULL,             // default security attributes
                         0,                // use default stack size
                         multMatrix,       // thread function
                         &threadArgs,      // argument to thread function
                         CREATE_SUSPENDED, // create suspended initially
                         &dwThreadId[0]);  // returns the thread identifier
        if (hThread[0] == NULL) {
            fprintf(stderr, "Error creating thread (%lu).\n", GetLastError());
            exit(1);
        }
    }
    // Задання рівня пріоритету потоків
    if (!SetThreadPriority(hThread[0], THREAD_PRIORITY_IDLE))
        printf("\nSet Thread[0] Priority %d is failed !!!\n",
               THREAD_PRIORITY_IDLE);

    for (int i = 1; i < 6; i++) {
        if (!SetThreadPriority(hThread[i], i - 3))
            printf("\nSet Thread[%d] Priority %d is failed !!!\n", i, i - 3);
    }
    if (!SetThreadPriority(hThread[6], THREAD_PRIORITY_TIME_CRITICAL))
        printf("\nSet Thread[6] Priority %d is failed !!!\n",
               THREAD_PRIORITY_TIME_CRITICAL);
    for (i = 0; i < 7; i++)
        SetThreadPriorityBoost(hThread[i], TRUE);
    // Запускаємо потоки у виконання
    for (i = 0; i < 7; i++) {
        ResumeThread(hThread[i]);
    }
    // Чекаємо поки потоки завершаться
    WaitForMultipleObjects(7, hThread, TRUE, INFINITE);
    // Виводимо результати
    printRes(hThread);
    // Закриття дескрипторів потоків
    for (int i = 0; i < 7; i++)
        CloseHandle(hThread[i]);
}
void SequentialThreadExecution(int n) {
    struct ThreadArgs threadArgs;
    threadArgs.array_size = n;
    int i;
    HANDLE hThread[7];
    DWORD dwThreadId[7];
    // Cтворюємо, запускаємо і очікуємо завершення 0-го потоку
    hThread[0] = CreateThread(NULL,             // default security attributes
                              0,                // use default stack size
                              multMatrix,       // thread function
                              &threadArgs,      // argument to thread function
                              CREATE_SUSPENDED, // create suspended initially
                              &dwThreadId[0]);  // returns the thread identifier
    if (hThread[0] == NULL) {
        fprintf(stderr, "Error creating thread (%lu).\n", GetLastError());
        exit(1);
    }
    BOOL success = SetThreadPriority(hThread[0], THREAD_PRIORITY_IDLE);
    if (!success)
        printf("\nSet Thread[0] Priority %d is failed !!!\n",
               THREAD_PRIORITY_IDLE);
    SetThreadPriorityBoost(hThread[0], TRUE);
    ResumeThread(hThread[0]);
    WaitForSingleObject(hThread[0], INFINITE);
    // Cтворюємо, запускаємо і очікуємо завершення потоків від 1 до 5
    for (i = 1; i < 6; i++) {
        hThread[i] =
            CreateThread(NULL,             // default security attributes
                         0,                // use default stack size
                         multMatrix,       // thread function
                         &threadArgs,      // argument to thread function
                         CREATE_SUSPENDED, // create suspended initially
                         &dwThreadId[i]);  // returns the thread identifier
        if (hThread[i] == NULL) {
            fprintf(stderr, "Error creating thread (%lu).\n", GetLastError());
            exit(1);
        }
        success = SetThreadPriority(hThread[i], i - 3);
        if (!success)
            printf("\nSet Thread[%d] Priority %d is failed !!!\n", i, i - 3);
        SetThreadPriorityBoost(hThread[i], TRUE);
        ResumeThread(hThread[i]);
        WaitForSingleObject(hThread[i], INFINITE);
    }
    // Cтворюємо, запускаємо і очікуємо завершення 6-го потоку
    hThread[6] = CreateThread(NULL,             // default security attributes
                              0,                // use default stack size
                              multMatrix,       // thread function
                              &threadArgs,      // argument to thread function
                              CREATE_SUSPENDED, // create suspended initially
                              &dwThreadId[6]);  // returns the thread identifier
    if (hThread[6] == NULL) {
        fprintf(stderr, "Error creating thread (%lu).\n", GetLastError());
        exit(1);
    }
    success = SetThreadPriority(hThread[6], THREAD_PRIORITY_TIME_CRITICAL);
    if (!success)
        printf("\nSet Thread[7] Priority %d is failed !!!\n",
               THREAD_PRIORITY_TIME_CRITICAL);
    SetThreadPriorityBoost(hThread[6], TRUE);
    ResumeThread(hThread[6]);
    WaitForSingleObject(hThread[6], INFINITE);
    printRes(hThread);
    // Закриття дескрипторів
    for (int i = 0; i < 7; i++)
        CloseHandle(hThread[i]);
}
int main() {
    // Отримання дескриптора поточного процесу
    HANDLE hProcess = GetCurrentProcess();
    // Отримання пріоритету класу для поточного процесу
    int priorityClass = GetPriorityClass(hProcess);
    printf("------------------------------------------------------\n");
    printf("\nPriority class for the current process is : %#010x\n",
           priorityClass);
    int k;
    printf("\nIf you want to set the priority class to IDLE_PRIORITY_CLASS "
           "press 1, to HIGH_PRIORITY_CLASS press 2\n");
    printf("\else, press any other key : ");
    scanf_s("%d", &k);
    if (k == 1) {
        if (SetPriorityClass(hProcess, IDLE_PRIORITY_CLASS)) {
            printf("\nSet Priority Class is success !!!\n");
            priorityClass = GetPriorityClass(hProcess);
            printf("Priority class for the current process is : %#010x\n",
                   priorityClass);
        } else
            printf("\nSet Priority Class is failed !!!\n");
    } else if (k == 2) {
        if (SetPriorityClass(hProcess, HIGH_PRIORITY_CLASS)) {
            printf("\nSet Priority Class is success !!!\n");
            priorityClass = GetPriorityClass(hProcess);
            printf("Priority class for the current process is : %#010x\n",
                   priorityClass);
        } else
            printf("\nSet Priority Class is failed !!!\n");
    }
    int n;
    printf("\n------------------------------------------------------\n");
    printf("Enter the size of the array: ");
    scanf_s("%d", &n);
    printf("\n------------------------------------------------------\n");
    printf("Results parallel execution of threads\n");
    ParallelThreadExecution(n);
    printf("\n------------------------------------------------------\n");
    printf("Results sequential execution of threads\n");
    SequentialThreadExecution(n);
    return 0;
}
