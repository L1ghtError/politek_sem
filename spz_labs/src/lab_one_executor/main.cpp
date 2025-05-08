
#include <stdio.h>
#include <windows.h>
int main() {
    const char* lpApplicationName = "./lab_one.exe";
    PROCESS_INFORMATION pi;
    STARTUPINFO si;
    int status = 0;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    // Встановлюємо прапорець CREATE_NEW_CONSOLE, щоб створити нове консольне
    // вікно
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_SHOWDEFAULT;
    if (!CreateProcess(lpApplicationName, NULL, NULL, NULL, FALSE,
                       CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
        fprintf(stderr, "CreateProcess failed (%lu).\n", GetLastError());
        return 1;
    }
    printf("Child process started...\n");
    // Виведення інформації про новий процес та батьківський процес
    printf("\nChild process ID: %lu\n", pi.dwProcessId);
    printf("Child thread ID: %lu\n", pi.dwThreadId);
    printf("Child process Handle: %p\n", pi.hProcess);
    printf("Child thread Handle: %p\n", pi.hThread);
    // Виведення інформації про батьківський процес
    printf("\nParent process ID: %lu\n", GetCurrentProcessId());
    printf("Parent thread ID: %lu\n", GetCurrentThreadId());
    printf("Parent process Handle: %p\n", GetCurrentProcess());
    printf("Parent thread Handle: %p\n", GetCurrentThread());

    // Очікування завершення дочірнього процесу
    WaitForSingleObject(pi.hProcess, INFINITE);
    // Отримання статусу завершення процесу
    GetExitCodeProcess(pi.hProcess, (LPDWORD)&status);
    printf("\nChild process finished with status %d\n", status);

    // Закриття дескрипторів, які більше не потрібні
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return 0;
}