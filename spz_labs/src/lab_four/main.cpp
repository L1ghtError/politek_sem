#include <stdio.h>
#include <windows.h>
#include <locale.h>

BOOL fileExists(const char* path) {
    DWORD attr = GetFileAttributesA(path);
    return (attr != INVALID_FILE_ATTRIBUTES && !(attr & FILE_ATTRIBUTE_DIRECTORY));
}

void deleteFile(const char* path) {
    if (!fileExists(path)) {
        printf("File doesnt exist: %s\n", path);
        return;
    }
    if (DeleteFileA(path)) {
        printf("File deleted: %s\n", path);
    }
    else {
        printf("Error in file deletion.\n");
    }
}

void checkIfHidden(const char* path) {
    if (!fileExists(path)) {
        printf("File doesnt exist: %s\n", path);
        return;
    }

    DWORD attributes = GetFileAttributesA(path);
    printf("File %s is hidden: %s\n", path, (attributes & FILE_ATTRIBUTE_HIDDEN) ? "Yes" : "No");
}

void printCreationTime(const char* directoryPath) {
    HANDLE hDir = CreateFileA(
        directoryPath,
        GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS,
        NULL
    );

    if (hDir == INVALID_HANDLE_VALUE) {
        printf("Cannot open file.\n");
        return;
    }

    FILETIME creationTime;
    if (GetFileTime(hDir, &creationTime, NULL, NULL)) {
        SYSTEMTIME stUTC, stLocal;
        FileTimeToSystemTime(&creationTime, &stUTC);
        SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);

        printf("Creation time %s: %02d.%02d.%04d %02d:%02d:%02d\n",
            directoryPath,
            stLocal.wDay, stLocal.wMonth, stLocal.wYear,
            stLocal.wHour, stLocal.wMinute, stLocal.wSecond
        );
    }
    else {
        printf("Cannot update creation time.\n");
    }

    CloseHandle(hDir);
}

int main() {
    SetConsoleCP(1251); 
    SetConsoleOutputCP(1251);

    char filePath[MAX_PATH];
    char dirPath[MAX_PATH];
    int choice;

    do {
        printf("\n==== Menu ====\n");
        printf("1. Delete file\n");
        printf("2. Chek is file deleted\n");
        printf("3. Show creation time of the file\n");
        printf("0. Close program\n");
        printf("Your choice: ");
        scanf("%d", &choice);
        getchar();

        switch (choice) {
        case 1:
            printf("Provide new path for folder:\n");
            fgets(filePath, MAX_PATH, stdin);
            filePath[strcspn(filePath, "\n")] = '\0';
            deleteFile(filePath);
            break;
        case 2:
            printf("Provide full path for file to check:\n");
            fgets(filePath, MAX_PATH, stdin);
            filePath[strcspn(filePath, "\n")] = '\0';
            checkIfHidden(filePath);
            break;
        case 3:
            printf("Provide full path for directory:\n");
            fgets(dirPath, MAX_PATH, stdin);
            dirPath[strcspn(dirPath, "\n")] = '\0';
            printCreationTime(dirPath);
            break;
        case 0:
            printf("Exit the program...\n");
            break;
        default:
            printf("Wrong choice. try againx.\n");
        }

    } while (choice != 0);

    return 0;
}
