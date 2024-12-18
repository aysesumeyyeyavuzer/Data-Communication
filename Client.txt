#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib") // Winsock kütüphanesi

#define SERVER_IP "127.0.0.1"
#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    WSADATA wsa;
    SOCKET client_socket;
    struct sockaddr_in server;
    char buffer[BUFFER_SIZE];
    char message[BUFFER_SIZE];

    // Winsock başlatılıyor
    printf("Initializing Winsock...\n");
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Failed. Error Code: %d\n", WSAGetLastError());
        return 1;
    }
    printf("Winsock initialized.\n");

    // Soket oluşturuluyor
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Could not create socket: %d\n", WSAGetLastError());
        return 1;
    }
    printf("Socket created.\n");

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(SERVER_IP);
    server.sin_port = htons(PORT);

    // Sunucuya bağlanılıyor
    if (connect(client_socket, (struct sockaddr *)&server, sizeof(server)) < 0) {
        printf("Connect failed. Error Code: %d\n", WSAGetLastError());
        return 1;
    }
    printf("Connected to server.\n");

    while (1) {
        printf("Enter command (CREATE, DELETE, LIST): ");
        fgets(message, BUFFER_SIZE, stdin);
        message[strcspn(message, "\n")] = 0; // Yeni satırı kaldır

        if (send(client_socket, message, strlen(message), 0) < 0) {
            printf("Send failed.\n");
            return 1;
        }

        memset(buffer, 0, BUFFER_SIZE);
        int recv_size = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (recv_size == SOCKET_ERROR) {
            printf("Recv failed.\n");
            break;
        }

        printf("Server reply:\n%s\n", buffer);
    }

    closesocket(client_socket);
    WSACleanup();

    return 0;
}
