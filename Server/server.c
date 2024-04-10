#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

#define BUF_SIZE (100 * 4) // 100 bytes

char *buf = NULL;         /* buffer */
int cl_sockfd;            /* Socket descriptor for accepted client */
struct sockaddr_in cl_sa; /* Socket address of the client, once connected */
int listen_sockfd = -1;   /* Socket [file] descriptor */
unsigned short int port;  /* Port number */

/* Function Prototypes */
void cleanup(void); /* cleaning up and closing the server gracefully */
void SIGINT_handler(int sig); 
void listeningSocketSetup(int argc, char *argv[]); /* creating the listening socket */
void runServer(); /* running the server */
int runScript(void); /* running the python script */

/* RUNNING THE SERVER */
int main(int argc, char *argv[])
{
    int val = 1; /* Setting for one of the socket options */
    /* ------ Create a new socket { socket() } ------ */
    struct in_addr ia;     /* For the Internet address */
    struct sockaddr_in sa; /* For the socket address */
    const char *port_str;  /* For the port number */

    socklen_t cl_sa_size = sizeof(cl_sa);

    /* check if the Port Number was provided */
    if (!(port_str = argv[1]))
    {
        fprintf(stderr, "server: USAGE: server <listen_Port>\n");
        cleanup();
        exit(EXIT_FAILURE);
    }
    port = (unsigned short int)atoi(port_str);
    ia.s_addr = inet_addr("192.168.2.37");
    sa.sin_family = AF_INET;
    sa.sin_addr = ia;
    sa.sin_port = htons(port);

    /* check if we can use the given port */
    if (port >= 0 && port <= 1023)
    {
        fprintf(stderr, "server: ERROR: Port number is priviledged.\n");
        cleanup();
        exit(EXIT_FAILURE);
    }

    /* Create a new TCP socket */
    listen_sockfd = socket(AF_INET, SOCK_STREAM, 0);

    /* Check that it succeeded */
    if (listen_sockfd < 0)
    {
        fprintf(stderr, "server: ERROR: Unable to create a new socket.\n");
        cleanup();
        exit(EXIT_FAILURE);
    }

    if (setsockopt(listen_sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&val, sizeof(int)) != 0)
    {
        fprintf(stderr, "server: ERROR: setsockopt() failed.\n");
        cleanup();
        exit(EXIT_FAILURE);
    }

    /* ----- Bind the socket { bind() } ------ */
    /* Bind our listening socket to the socket address */
    if (bind(listen_sockfd, (struct sockaddr *)&sa, sizeof(sa)) != 0)
    {
        fprintf(stderr, "server: ERROR: Socket binding failed.\n");
        cleanup();
        exit(EXIT_FAILURE);
    }

    /* ------ Listen on the socket for a client { listen() } ------ */
    /*  Turn the socket that we have already made into a listening socket
     *  so that it can accept requests from clients */
    if (listen(listen_sockfd, 32) != 0)
    {
        fprintf(stderr, "server: ERROR: listen(): Failed.\n");
        cleanup();
        exit(EXIT_FAILURE);
    }

    /* ----- Accept the connection to the client { accept() } ----- */
    /* Clear all the bytes of this structure to zero */
    memset((void *)&cl_sa, 0, sizeof(cl_sa));

    printf("server: Awaiting TCP connection over Port %s...\n", port_str);

    signal(SIGINT, SIGINT_handler); /* signal */

    /* sit and wait for the client to attempt a connection */
    while (1)
    {
        cl_sockfd = accept(listen_sockfd, (struct sockaddr *)&cl_sa, &cl_sa_size);

        /* Check we have a valid socket */
        if (cl_sockfd > 0)
        {
            printf("server: Connection accepted! (file desc = %d)\n", cl_sockfd);
            /* Now we need to recieve the data and write it to a buffer */
            buf = (void *)malloc(BUF_SIZE); /* For storing incoming data using 'recv()' */

            /* Check to see if the memory was allocated properly */
            if (buf == NULL)
            {
                fprintf(stderr, "server: ERROR: Failed to allocate memory.\n");
                cleanup();
                exit(EXIT_FAILURE);
            }

            /* Attempt to read data from a socket */
            printf("server: Receiving command... \n");
            int command = recv(cl_sockfd, (void *)buf, BUF_SIZE, MSG_WAITALL); 

            /* Close the client socket after we get what we need */
            if (cl_sockfd > -1)
            {
                printf("server: Connection closed.\n");
                close(cl_sockfd);
                cl_sockfd = -1;
            }

        }

        /* There was a connection error */
        else
        {
            fprintf(stderr, "server: ERROR: accept(): Connection request failed.\n");
            cleanup();
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}

/* CREATING THE SOCKET AND MAKING IT A LISTENING SOCKET */
void listeningSocketSetup(int argc, char *argv[])
{
    int val = 1; /* Setting for one of the socket options */
    /* ------ Create a new socket { socket() } ------ */
    struct in_addr ia;     /* For the Internet address */
    struct sockaddr_in sa; /* For the socket address */
    const char *port_str;  /* For the port number */

    socklen_t cl_sa_size = sizeof(cl_sa);

    /* check if the Port Number was provided */
    if (!(port_str = argv[1]))
    {
        fprintf(stderr, "server: USAGE: server <listen_Port>\n");
        cleanup();
        exit(EXIT_FAILURE);
    }
    port = (unsigned short int)atoi(port_str);
    ia.s_addr = inet_addr("192.168.2.37");
    sa.sin_family = AF_INET;
    sa.sin_addr = ia;
    sa.sin_port = htons(port);

    /* check if we can use the given port */
    if (port >= 0 && port <= 1023)
    {
        fprintf(stderr, "server: ERROR: Port number is priviledged.\n");
        cleanup();
        exit(EXIT_FAILURE);
    }

    /* Create a new TCP socket */
    listen_sockfd = socket(AF_INET, SOCK_STREAM, 0);

    /* Check that it succeeded */
    if (listen_sockfd < 0)
    {
        fprintf(stderr, "server: ERROR: Unable to create a new socket.\n");
        cleanup();
        exit(EXIT_FAILURE);
    }

    if (setsockopt(listen_sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&val, sizeof(int)) != 0)
    {
        fprintf(stderr, "server: ERROR: setsockopt() failed.\n");
        cleanup();
        exit(EXIT_FAILURE);
    }

    /* ----- Bind the socket { bind() } ------ */
    /* Bind our listening socket to the socket address */
    if (bind(listen_sockfd, (struct sockaddr *)&sa, sizeof(sa)) != 0)
    {
        fprintf(stderr, "server: ERROR: Socket binding failed.\n");
        cleanup();
        exit(EXIT_FAILURE);
    }

    /* ------ Listen on the socket for a client { listen() } ------ */
    /*  Turn the socket that we have already made into a listening socket
     *  so that it can accept requests from clients */
    if (listen(listen_sockfd, 32) != 0)
    {
        fprintf(stderr, "server: ERROR: listen(): Failed.\n");
        cleanup();
        exit(EXIT_FAILURE);
    }

    /* ----- Accept the connection to the client { accept() } ----- */
    /* Clear all the bytes of this structure to zero */
    memset((void *)&cl_sa, 0, sizeof(cl_sa));

    printf("server: Awaiting TCP connection over Port %s...\n", port_str);

    signal(SIGINT, SIGINT_handler); /* signal */
}

void runServer(){
    /* sit and wait for the client to attempt a connection */
    while (1)
    {
        // cl_sockfd = accept(listen_sockfd, (struct sockaddr *)&cl_sa, &cl_sa_size);

        /* Check we have a valid socket */
        if (cl_sockfd > 0)
        {
            printf("server: Connection accepted! (file desc = %d)\n", cl_sockfd);
            /* Now we need to recieve the data and write it to a buffer */
            buf = (void *)malloc(BUF_SIZE); /* For storing incoming data using 'recv()' */

            /* Check to see if the memory was allocated properly */
            if (buf == NULL)
            {
                fprintf(stderr, "server: ERROR: Failed to allocate memory.\n");
                cleanup();
                exit(EXIT_FAILURE);
            }

            /* Attempt to read data from a socket */
            printf("server: Receiving command... \n");
            int command = recv(cl_sockfd, (void *)buf, BUF_SIZE, MSG_WAITALL); 

            /* Close the client socket after we get what we need */
            if (cl_sockfd > -1)
            {
                printf("server: Connection closed.\n");
                close(cl_sockfd);
                cl_sockfd = -1;
            }


           
        }

        /* There was a connection error */
        else
        {
            fprintf(stderr, "server: ERROR: accept(): Connection request failed.\n");
            cleanup();
            exit(EXIT_FAILURE);
        }
    }
}

int runScript()
{
    char buffer[1024]; // Buffer to store the output

    // Open a pipe to the Python interpreter and the script
    FILE *pipe = popen("python3 ../ImageProcessing/main.py", "r");
    if (!pipe)
    {
        perror("popen");
        return 1;
    }

    // Read the output of the script into the buffer
    fgets(buffer, sizeof(buffer), pipe);

    // Close the pipe
    pclose(pipe);

    // Print the output
    printf("Output: %s\n", buffer);

    return 0;
}

void cleanup()
{

    if (buf != NULL)
    {               /* if the buffer is not empty */
        free(buf);  /* free all of the memory in the buffer */
        buf = NULL; /* empty buffer */
    }

    if (cl_sockfd > -1)
    {                     /* if the client socket is still in use */
        close(cl_sockfd); /* close the socket */
        cl_sockfd = -1;   /* Mark it as unused */
    }

    if (listen_sockfd > -1)
    {                         /* if the listening socket is still in use */
        close(listen_sockfd); /* close the socket */
        listen_sockfd = -1;   /* Mark it as unused */
    }
}

void SIGINT_handler(int sig)
{
    fprintf(stderr, "server: Server interrupted. Shutting down.\n"); /* alert message */

    cleanup(); /* cleanup */

    exit(EXIT_FAILURE); /* exit with a failure */
}