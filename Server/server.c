#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define IP_ADDR 
#define PORT 
#define BUF_SIZE 1024

char *runScript(void);

int main()
{
    int server_fd, client_fd;
    struct sockaddr_in sa;
    struct in_addr ia;
    int addrlen = sizeof(sa);
    char *message = "Hello!\nPlease enter a command:\n(1) Check for parking spots\n(2) Close connection";

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    ia.s_addr = inet_addr(IP_ADDR);
    sa.sin_family = AF_INET;
    sa.sin_addr = ia;
    sa.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&sa, sizeof(sa)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        if (listen(server_fd, 3) < 0)
        {
            perror("listen");
            exit(EXIT_FAILURE);
        }

        if ((client_fd = accept(server_fd, (struct sockaddr *)&sa, (socklen_t *)&addrlen)) < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        int bytes_sent = send(client_fd, message, strlen(message), 0);
        if (bytes_sent > 0)
        {
            printf("Welcome message sent to client\n");
        }

        char buf[BUF_SIZE];
        memset(buf, 0, BUF_SIZE);

        int bytes_recv = recv(client_fd, buf, BUF_SIZE, 0);
        if (bytes_recv > 0)
        {
            printf("Command received from client: %s\n", buf);
        }

        int command = atoi(buf);
        if (command == 1)
        {
            char *out = runScript();
            if (out != NULL)
            {
                printf("Script output: %s\n", out);
                int bytes_sent = send(client_fd, out, strlen(out), 0);
                if (bytes_sent > 0)
                {
                    printf("Message sent to client\n");
                }
                else
                {
                    perror("send");
                }
                free(out); // Free allocated memory
            }
            else
            {
                perror("runScript");
            }
        }

        if (command == 2)
        {
            close(client_fd);
        }
        else
        {
            close(client_fd);
        }
    }
    return 0;
}

char *runScript()
{
    char buffer[1024 * 5]; // Buffer to store the output : 5 KB

    FILE *pipe = popen("python3 ./ImageProcessing/main.py", "r");
    if (!pipe)
    {
        perror("popen");
        return NULL;
    }

    fgets(buffer, sizeof(buffer), pipe);

    pclose(pipe);

    return strdup(buffer);
}
