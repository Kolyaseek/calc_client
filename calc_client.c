#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int main(int argc, char* argv[]) {
    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    char operation;
    int num1, num2;

    if (argc != 3) {
        printf("Usage: %s <IP-адрес> <номер_порта>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Создаем сокет
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Could not create socket");
        exit(EXIT_FAILURE);
    }

    // Настраиваем адрес сервера
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));
    if (inet_pton(AF_INET, argv[1], &server_addr.sin_addr) <= 0) {
        perror("Invalid address");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    // Подключаемся к серверу
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection failed");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    printf("Connected to the server.\n");

    // Основной цикл ввода/вывода
    while (1) {
        // Ввод данных пользователя
        printf("Enter operation (format: num1 operator num2, e.g., 3 + 4): ");
        scanf("%d %c %d", &num1, &operation, &num2);

        // Формируем запрос к серверу
        sprintf(buffer, "%d %c %d", num1, operation, num2);

        // Отправляем запрос на сервер
        if (send(client_socket, buffer, strlen(buffer), 0) == -1) {
            perror("Send failed");
            close(client_socket);
            exit(EXIT_FAILURE);
        }

        // Получаем ответ от сервера
        memset(buffer, 0, BUFFER_SIZE);
        if (recv(client_socket, buffer, BUFFER_SIZE, 0) == -1) {
            perror("Receive failed");
            close(client_socket);
            exit(EXIT_FAILURE);
        }

        // Выводим результат
        printf("Server response: %s", buffer);

        // Выход по ключевому слову
        printf("Continue? (yes/no): ");
        char choice[4];
        scanf("%s", choice);
        if (strcmp(choice, "no") == 0) {
            break;
        }
    }

    // Закрываем сокет
    close(client_socket);
    return 0;
}
