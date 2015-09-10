/* $Id: serverMode.c,v 1.5 2015/03/23 23:17:02 sentient Exp $
 *
 *
 *
 *     REVISON HISTORY
 *
 *         $Log: serverMode.c,v $
 *         Revision 1.5  2015/03/23 23:17:02  sentient
 *         Workins with multiple connections; can handle disconnects some
 *
 *         Revision 1.4  2015/03/23 00:18:37  sentient
 *         Multiple people can chat
 *
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <pthread.h>
#include <poll.h>
#include <sys/stat.h>

#define SOCK_PATH "/usr/local/etc/SOCKET"
#define MESSAGE_LEN 256
#define MAX_CONNECTIONS 16

struct client {
  int socket;
  int isConnected;
};

struct client clients[MAX_CONNECTIONS]; // Consider different data structure
int numConnected = 0;

void *serverReceive(void *);

/****************************************************************************
 * #!serverMode                                                             *   
 ****************************************************************************/
void serverMode()
{
    int serverSocket;
    int serverSocketLen;
    int sockAddrSize;
    int s;
    int i = 0;
    struct sockaddr_un clientAddr[MAX_CONNECTIONS];
    struct sockaddr_un server;
    struct stat socketStat;
    pthread_t receive_th[MAX_CONNECTIONS];

    // Create server socket
    if ((serverSocket = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    // Bind server socket to address
    server.sun_family = AF_UNIX;
    strcpy(server.sun_path, SOCK_PATH);
    unlink(server.sun_path);
    serverSocketLen = strlen(server.sun_path) + sizeof(server.sun_family);
    if (bind(serverSocket, (struct sockaddr *)&server, serverSocketLen) == -1) {
        perror("bind");
        exit(1);
    }
    
    // Allow anyone to read and write to socket... Dangerous?
    stat(SOCK_PATH, &socketStat);
    chmod(SOCK_PATH, socketStat.st_mode | S_IWOTH | S_IROTH);

    printf("Listening...\n");
  
    while (numConnected < MAX_CONNECTIONS) { 

        // Listen for incoming connections
        if (listen(serverSocket, MAX_CONNECTIONS) == -1) {
            perror("listen");
            exit(1);
        }
        
        // Accept connection to a client ... Vulernable to disconnects!
        sockAddrSize = sizeof(clientAddr[i]);
        if ((s = accept(serverSocket, (struct sockaddr *)(clientAddr+i),\
                                      (socklen_t *)&sockAddrSize)) == -1) {
            perror("accept");
            exit(1);
        }

        clients[i].socket = s;
        clients[i].isConnected = 1;
        ++numConnected;

        // Start Thread for particular client
        pthread_create(receive_th + i, NULL, serverReceive, clients + i);
     
        printf("Connected client: %d\n", i+1);
        ++i;
    }

} 

/****************************************************************************
 * #!serverReceive                                                          *   
 ****************************************************************************/
void *serverReceive(void *messenger)
{
    struct client *sender = (struct client *) messenger; 
    int i, r;
    char message[MESSAGE_LEN];

    while (sender->isConnected) {
        r = recv(sender->socket, message, MESSAGE_LEN - 1, 0);
 
        if (r > 0) { 
            message[r] = '\0';

            // Send Message to all clients except sender
            for(i=0; i < MAX_CONNECTIONS; i++) {
           
                if (clients[i].isConnected && clients[i].socket != sender->socket) {
                    if ( send(clients[i].socket, message, strlen(message), 0) == -1) {
                       perror("send");
                       exit(1);
                    }
                }
            }   
        }
        else { 
            sender->isConnected = 0;
            --numConnected;
            printf("A client has disconnected.");
        }
    }
    
    return NULL;
}
