#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>  
#include <string>

#pragma comment(lib, "ws2_32.lib")

const int PORT = 77777;
const std::string SERVER_ADDRESS = "127.0.0.1";

int main() {
    WSADATA wsData;
    if (WSAStartup(MAKEWORD(2, 2), &wsData) != 0) {
        std::cerr << "Failed to initialize Winsock" << std::endl;
        return 1;
    }

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create socket" << std::endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    if (inet_pton(AF_INET, SERVER_ADDRESS.c_str(), &(serverAddress.sin_addr)) <= 0) {
        std::cerr << "An error occurred while converting the server address" << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    if (connect(clientSocket, (sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        std::cerr << "Failed to connect to server" << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Connected to the server" << std::endl;

    std::string input_payload;

    std::cout << "Enter array size: ";
    std::getline(std::cin, input_payload);


    // Відправляємо рядки на сервер
    send(clientSocket, input_payload.c_str(), input_payload.size(), 0);

    // Отримуємо результат від сервера
    char buffer[1024];
    int bytesReceived = recv(clientSocket, buffer, 1024, 0);
    if (bytesReceived == SOCKET_ERROR) {
        std::cerr << "Error in receiving data from server" << std::endl;
    }
    else {
        buffer[bytesReceived] = '\0';
        std::cout << "Result from server: " << buffer << std::endl;
    }

    closesocket(clientSocket);
    WSACleanup();

    return 0;
}