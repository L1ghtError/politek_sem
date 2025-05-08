
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
// Структура для передачі аргументів у функцію потоку
struct ThreadArgs {
    int *array;     // Динамічний масив
    int array_size; // Розмір масиву
    int max;
};
// Функція потоку для обертання масиву
DWORD WINAPI reverseArray(LPVOID lpParam) {
    struct ThreadArgs *threadArgs = (struct ThreadArgs *)lpParam;
    int *array = threadArgs->array;
    int size = threadArgs->array_size;
    int max_index = -1;
    if (size > 0)
        max_index = 0;

    for (int i = 0; i < size; i++) {
        if (array[i] > array[max_index])
            max_index = i;
    }
    threadArgs->max = array[max_index];
    return 0;
}
int main() {
    int n;
    printf("Enter the size of the maxrix: ");
    scanf_s("%d", &n);
    int half = n;
    n *= n;
    // Генерування динамічного масиву з випадковими значеннями
    int *dynamicArray = (int *)malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) {
        dynamicArray[i] = rand() % 100; // Випадкові значення від 0 до 99
    }
    // Виведення початкового масиву
    printf("\nOriginal matrix:\n");
    for (int i = 0; i < n; i++) {
        printf("%d ", dynamicArray[i]);
        if (i % half == half - 1)
            printf("\n");
    }
    printf("\n");
    // Створення структури з аргументами для функції потоку
    struct ThreadArgs threadArgs;
    threadArgs.array = dynamicArray;
    threadArgs.array_size = n;
    HANDLE hThread;
    DWORD dwThreadId;
    // Створення потоку для обертання масиву
    hThread = CreateThread(NULL,         // default security attributes
                           0,            // use default stack size
                           reverseArray, // thread function
                           &threadArgs,  // argument to thread function
                           0,            // use default creation flags
                           &dwThreadId); // returns the thread identifier
    if (hThread == NULL) {
        fprintf(stderr, "Error creating thread (%lu).\n", GetLastError());
        return 1;
    }
    // Чекаємо завершення потоку
    WaitForSingleObject(hThread, INFINITE);
    // Виведення обернутого масиву
    printf("\Max elemnet is:%d\n", threadArgs.max);
    printf("\n");
    // Виведення інформації про потік та основний процес
    printf("\nThread ID: %lu\n", dwThreadId);
    printf("Thread Handle: %p\n\n", hThread);
    printf("Main process ID: %lu\n", GetCurrentProcessId());
    printf("Main thread ID: %lu\n", GetCurrentThreadId());
    printf("Main process Handle: %p\n", GetCurrentProcess());
    printf("Main thread Handle: %p\n", GetCurrentThread());
    // Закриття дескрипторів, які більше не потрібні
    CloseHandle(hThread);
    free(dynamicArray);
    getchar();
    getchar();
    return 0;
}