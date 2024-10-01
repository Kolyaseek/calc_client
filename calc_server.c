#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    int num1, num2;
    char operation;
    double result;

    while (1) {
        // Получаем данные от клиента
        memset(buffer, 0, BUFFER_SIZE);
        int read_size = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (read_size <= 0) {
            break;
        }

        // Обрабатываем запрос
        sscanf(buffer, "%d %c %d", &num1, &operation, &num2);

        switch (operation) {
        case '+': result = num1 + num2; break;
        case '-': result = num1 - num2; break;
        case '*': result = num1 * num2; break;
        case '/':
            if (num2 == 0) {
                sprintf(buffer, "Error: Division by zero.\n");
                send(client_socket, buffer, strlen(buffer), 0);
                continue;
            }
            result = (double)num1 / num2;
            break;
        default:
            sprintf(buffer, "Error: Unsupported operation.\n");
            send(client_socket, buffer, strlen(buffer), 0);
            continue;
        }

        // Отправляем результат обратно клиенту
        sprintf(buffer, "Result: %.2f\n", result);
        send(client_socket, buffer, strlen(buffer), 0);
    }

    close(client_socket);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    // Создаем сокет
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Could not create socket");
        exit(EXIT_FAILURE);
    }

    // Задаем параметры адреса сервера
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Привязываем сокет к порту
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // Запускаем прослушивание порта
    if (listen(server_socket, 3) == -1) {
        perror("Listen failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d...\n", PORT);

    // Ждем подключения клиентов
    while (1) {
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_len);
        if (client_socket == -1) {
            perror("Accept failed");
            close(server_socket);
            exit(EXIT_FAILURE);
        }

        printf("Client connected.\n");

        // Обрабатываем запрос клиента
        handle_client(client_socket);
    }

    close(server_socket);
    return 0;
}
