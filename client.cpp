#include "header.h"
#include <sys/types.h>
#include <sys/socket.h>

int main()
{
    request req; 
    int socket_fd;
    int s, s1;
    struct sockaddr_in remote;
    struct in_addr addr;
    struct sockaddr_in local;
    char buf[MENSAJE_MAXIMO];


    strncpy(req.type, "Clientes", 15);
    vector <int> direcciones; 

    if ((s = socket (PF_INET, SOCK_STREAM, 0))== -1)
    {
        perror("socket");
        exit(1);
    }

    local.sin_family = AF_INET;
    local.sin_port = htons(1059);
    local.sin_addr.s_addr = INADDR_ANY; 

    if (bind(s, (struct sockaddr *)&local, sizeof(local)) < 0)
    {
        perror("bind");
        exit(1);  
    }

    if (listen(s, 10) == -1) 
    {
        perror("listen");
        exit(1);
    }
    
    
    if((socket_fd = socket (PF_INET, SOCK_STREAM, 0))== -1)
    {
        perror("creando socket");
        exit(1);
    }    

    remote.sin_family = AF_INET;
    remote.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &(remote.sin_addr));

    int result = connect(socket_fd, (struct sockaddr *)&remote, sizeof(remote));
    if (result == -1)
    {
        perror ("conectandose");
        exit(1);
    }

    recvrequest(socket_fd, req);
    int puertoCl = atoi(req.msg);
    cout << req.type << endl;
    cout << puertoCl << endl;
    
    
    if ((s = socket (PF_INET, SOCK_STREAM, 0))== -1)
    {
        perror("socket");
        exit(1);
    }

    local.sin_family = AF_INET;
    local.sin_port = htons(puertoCl);
    local.sin_addr.s_addr = INADDR_ANY; 

    if (bind(s, (struct sockaddr *)&local, sizeof(local)) < 0)
    {
        perror("bind");
        exit(1);  
    }

    if (listen(s, 10) == -1) 
    {
        perror("listen");
        exit(1);
    }
    
    close(socket_fd);

    return 0;
    
}