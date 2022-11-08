#include "header.h"
#include <sys/types.h>
#include <sys/socket.h>

int main()
{
    request req; 
    int socket_fd;
    struct sockaddr_in remote;
    struct in_addr addr;
    char buf[MENSAJE_MAXIMO];

    if((socket_fd = socket (PF_INET, SOCK_STREAM, 0))== -1)
    {
        perror("creando socket");
        exit(1);
    }    

    remote.sin_family = AF_INET;
    remote.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &(remote.sin_addr));

    int s = connect(socket_fd, (struct sockaddr *)&remote, sizeof(remote));
    if (s == -1)
    {
        perror ("conectandose");
        exit(1);
    }

    recvrequest(socket_fd, req);
    cout << req.type << endl;
    close(socket_fd);

    return 0;
    
}