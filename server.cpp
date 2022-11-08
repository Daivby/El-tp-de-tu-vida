#include "header.h"
#include <sys/types.h>
#include <sys/socket.h>

using namespace std;

int aceptar_cliente( vector<int> &direcciones, int s)
{
thread threads[1024];
threads[0] = thread(aceptar_cliente, s, &direcciones);
threads[0].join();
while(true)
    {
     int s1;
     struct sockaddr_in remote;
     int t;

        if((s1 = accept(s, (struct sockaddr *) &remote, (socklen_t *) &t))== -1)
            {
                perror("aceptando la conexion entrante");
                exit(1);
            }
        
        direcciones.push_back(s1);
        cout << "conexion establecida" << endl;
    }

}



int main()

{
    request req;
    strncpy(req.type, "Clientes", 15);
    int s, s1;
    vector <int> direcciones; 
    struct sockaddr_in local;
    struct sockaddr_in remote;
    thread threads;

    if ((s = socket (PF_INET, SOCK_STREAM, 0))== -1)
    {
        perror("socket");
        exit(1);
    }

    local.sin_family = AF_INET;
    local.sin_port = htons(PORT);
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

    int t = sizeof(remote);
    int i = 0;  
    
    aceptar_cliente(direcciones, s);
    sendrequest(s1, req);
    close(s1);
    return 0;
}
