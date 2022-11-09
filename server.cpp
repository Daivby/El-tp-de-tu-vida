#include "header.h"
#include <sys/types.h>
#include <sys/socket.h>
using namespace std;
sem_t minimal;

void aceptar_cliente( vector<int> &direcciones, int s, vector<int> &listpuerto)
{
    int puerto = PORT + 1;
    int amount;
    while(true)
    { 
    
        int s1;
        struct sockaddr_in remote;
        int t;
        request req;
        strncpy(req.type, "PUERTO", 15);
        string puertotemp_str = to_string(puerto);
        char const* puerto2 = puertotemp_str.c_str();
        strncpy(req.msg, puerto2, MENSAJE_MAXIMO);
        if((s1 = accept(s, (struct sockaddr *) &remote, (socklen_t *) &t))== -1)
        {
            perror("aceptando la conexion entrante");
            exit(1);
        }
        listpuerto.push_back(puerto);    
        direcciones.push_back(s1);
        cout << "conexion establecida" << endl;
        sendrequest(s1, req);
        puerto++;
    
        amount++;
        if(amount == 9)
        {   
            sem_post(&minimal);
        }
    
    }
}



int main()

{
    sem_init(&minimal, 0, 0);
    request req;
    strncpy(req.type, "Clientes", 15);
    int s, s1;
    vector <int> direcciones; 
    struct sockaddr_in local;
    struct sockaddr_in remote;
    thread threads;
    vector <int> listpuerto;
    
    

    vector <vector <int>> Portmatrix;
    vector <vector <int>> Sockmatrix; 
    


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
    threads = thread(aceptar_cliente, ref(direcciones), s);
    sem_wait(&minimal);

    for (int a = 0; a < 3; a++)
    {   vector <int> rowsockets;
        vector <int> rowports;
        
        for (int j = 0; j < 3 ; j++)
        {

            rowports.push_back(listpuerto[j + 3 * a]);
            rowsockets.push_back(direcciones[j + 3 * a]);

        }
        
        Portmatrix.push_back(rowports);
        Sockmatrix.push_back(rowsockets);
    }

    

    threads.join();
    return 0;
}
