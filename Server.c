#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>
#include <direct.h>

#pragma comment(lib, "ws2_32.lib") // Winsock kütüphanesi

#define PORT 8080
#define BUFFER_SIZE 1024

void handle_client(SOCKET client_socket);

int main() {
    WSADATA wsa;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server, client;
    int c;

    // Winsock başlatılıyor
    printf("Initializing Winsock...\n");
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Failed. Error Code: %d\n", WSAGetLastError());
        return 1;
    }
    printf("Winsock initialized.\n");

    // Sunucu soketi oluşturuluyor
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Could not create socket: %d\n", WSAGetLastError());
        return 1;
    }
    printf("Socket created.\n");

    // Sunucu ayarları
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    // Soket bağlama
    if (bind(server_socket, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("Bind failed. Error Code: %d\n", WSAGetLastError());
        return 1;
    }
    printf("Bind done.\n");

    // Dinlemeye başlama
    listen(server_socket, 3);
    printf("Waiting for incoming connections...\n");

    c = sizeof(struct sockaddr_in);
    while ((client_socket = accept(server_socket, (struct sockaddr *)&client, &c)) != INVALID_SOCKET) {
        printf("Connection accepted.\n");

        // Her istemci için bir thread oluşturuluyor
        CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)handle_client, (void *)client_socket, 0, NULL);
    }

    if (client_socket == INVALID_SOCKET) {
        printf("Accept failed. Error Code: %d\n", WSAGetLastError());
        return 1;
    }

    closesocket(server_socket);
    WSACleanup();

    return 0;
}

void handle_client(SOCKET client_socket) {
    char buffer[BUFFER_SIZE];
    char command[BUFFER_SIZE];
    char filename[BUFFER_SIZE];

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int recv_size = recv(client_socket, buffer, BUFFER_SIZE, 0);

        if (recv_size == SOCKET_ERROR || recv_size == 0) {
            printf("Client disconnected.\n");
            closesocket(client_socket);
            return;
        }

        sscanf(buffer, "%s %s", command, filename);

        if (strcmp(command, "CREATE") == 0) {
            FILE *file = fopen(filename, "w");
            if (file) {
                fclose(file);
                send(client_socket, "File created successfully.\n", BUFFER_SIZE, 0);
            } else {
                send(client_socket, "Failed to create file.\n", BUFFER_SIZE, 0);
            }
        } else if (strcmp(command, "DELETE") == 0) {
            if (remove(filename) == 0) {
                send(client_socket, "File deleted successfully.\n", BUFFER_SIZE, 0);
            } else {
                send(client_socket, "Failed to delete file.\n", BUFFER_SIZE, 0);
            }
        } else if (strcmp(command, "LIST") == 0) {
            system("dir > files.txt");
            FILE *file = fopen("files.txt", "r");
            if (file) {
                char file_list[BUFFER_SIZE] = "";
                char line[BUFFER_SIZE];
                while (fgets(line, BUFFER_SIZE, file)) {
                    strcat(file_list, line);
                }
                fclose(file);
                send(client_socket, file_list, BUFFER_SIZE, 0);
                remove("files.txt");
            }
        } else {
            send(client_socket, "Invalid command.\n", BUFFER_SIZE, 0);
        }
    }
}
