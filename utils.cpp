#include "header.h"
int sendrequest(int socket_fd, request req)
{
    int len = MENSAJE_MAXIMO + 15;
    int n;
    n = send(socket_fd, &req, len, 0);
     if ((n = socket (PF_INET, SOCK_STREAM, 0))== -1)
    {
        perror("enviando");
        exit(1);
    }

    return 0;
}

int recvrequest(int socket_fd, request &req)
{
    int len = MENSAJE_MAXIMO + 15;
    int n;
    n = recv(socket_fd, &req, len, 0);
     if ((n = socket (PF_INET, SOCK_STREAM, 0))== -1)
    {
        perror("reciviendo");
        exit(1);
    }

    return 0;
}

