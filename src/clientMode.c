#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <pthread.h>
#include <pwd.h>

#define SOCK_PATH "/usr/local/etc/SOCKET"
#define MESSAGE_LEN 256

char *getUserName();
void *sendThread(void *);
void *receiveThread(void *);

struct communication { 
    char message[MESSAGE_LEN];
    int socket;
};

/****************************************************************************
 * #!clientMode                                                             *   
 ****************************************************************************/
void clientMode()
{
    int sockLen;
    struct sockaddr_un server;
    struct communication commData;
    pthread_t receive_th, send_th;

    // Create socket
    if ( (commData.socket = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    // Connect to server
    printf("Trying to connect...\n");

    server.sun_family = AF_UNIX;
    strcpy(server.sun_path, SOCK_PATH);
    sockLen = strlen(server.sun_path) + sizeof(server.sun_family);

    if (connect(commData.socket, (struct sockaddr *) &server, sockLen) == -1) {
        perror("connect");
        exit(1);
    }
    printf("Connected.\n");
 
    // Create threads for sending and receiving
    pthread_create(&send_th, NULL, sendThread, &commData);
    pthread_create(&receive_th, NULL, receiveThread, &commData);

    // Infinite loop until... I add graceful termination!  
    while(1); 
 
    close(commData.socket);
}

/****************************************************************************
 * #!sendThread                                                             *   
 ****************************************************************************/
void *sendThread(void *context)
{
    struct communication *commData = (struct communication *) context;
    
    char *userName = getUserName();
    char sendMessage[MESSAGE_LEN];

    while(fgets(commData->message, MESSAGE_LEN, stdin) != NULL) {
        sprintf(sendMessage, "[%s] %s", userName, commData->message); 
        

        if (send (commData->socket, sendMessage, \
                  strlen(sendMessage), 0) == -1) {
            perror("send");
            exit(1);
        }
     }

    return NULL;
}

/****************************************************************************
 * #!receiveThread                                                          *   
 ****************************************************************************/
void *receiveThread(void *context)
{
    struct communication *commData = (struct communication *) context; 
    int endIndex;

    while((endIndex=recv(commData->socket, commData->message, MESSAGE_LEN, 0))\
           > 0) {

        if (endIndex > 0) {
            commData->message[endIndex] = '\0';
            printf("\n%s", commData->message);
        } 
        else {
            if (endIndex < 0)
                perror("recv");
            else printf("Server closed connection\n");
                exit(1);
        }
    }

    return NULL;
}

/****************************************************************************
 * #!getUsername                                                            *   
 ****************************************************************************/

char *getUserName()
{
  register struct passwd *pw;
  register uid_t uid;

  uid = getuid ();
  pw = getpwuid (uid);

  return pw->pw_name;
}
