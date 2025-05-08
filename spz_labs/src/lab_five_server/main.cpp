#include <iostream>
#include <string>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

const int PORT = 77777;

int findMax(int *arr, int size) {
    int max = arr[0];
    for (int i = 1; i < size; i++) {
        if (max < arr[i])
            max = arr[i];
    }
    return max;
}

bool isInteger(const std::string &str) {
    for (char c : str) {
        if (!isdigit(c) && c != '+' && c != '-') {
            return false;
        }
    }
    return true;
}

int main() {
    WSADATA wsData;
    if (WSAStartup(MAKEWORD(2, 2), &wsData) != 0) {
        std::cerr << "Failed to initialize Winsock" << std::endl;
        return 1;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create socket" << std::endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (sockaddr *)&serverAddress, sizeof(serverAddress)) ==
        SOCKET_ERROR) {
        std::cerr << "Failed to bind socket to port" << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr
            << "An error occurred when setting standby mode for connections"
            << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "The server is waiting for connections..." << std::endl;

    while (true) {
        SOCKET clientSocket = accept(serverSocket, NULL, NULL);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Error receiving connection" << std::endl;
            closesocket(serverSocket);
            WSACleanup();
            return 1;
        }

        std::cout << "Connected to client" << std::endl;

        char buffer[1024];
        std::string str1;

        // Отримуємо рядки від клієнта
        int bytesReceived = recv(clientSocket, buffer, 1024, 0);
        if (bytesReceived == SOCKET_ERROR) {
            std::cerr << "Error in receiving data from client" << std::endl;
        } else {
            buffer[bytesReceived] = '\0';
            str1 = buffer;
        }

        std::cout << "Received strings from client: " << str1 << std::endl;

        std::string result;
        if (isInteger(str1) || std::stoi(str1) < 0) {
            int size = std::stoi(str1);
            int half = size;
            size *= size;
            int *dynamicArray = (int *)malloc(size * sizeof(int));
            std::cout << "array is:\n";
            for (int i = 0; i < size; i++) {
                dynamicArray[i] = rand() % 100;
                std::cout << dynamicArray[i] << " ";
                if (i % half == half - 1)
                    std::cout << "\n";
            }
            int max = findMax(dynamicArray, size);
            std::cout << "\nmax is: " << max << "\n";
            result = std::to_string(max);
        } else {
            result = "please, provide valid number, that greater than 0";
        }

        send(clientSocket, result.c_str(), result.size(), 0);

        closesocket(clientSocket);
        std::cout << "The connection with the client is closed" << std::endl;
    }

    closesocket(serverSocket);
    WSACleanup();

    return 0;
}