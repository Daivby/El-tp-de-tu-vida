#include "header.h"
#include <sys/types.h>
#include <sys/socket.h>

void aceptar_conecciones(int s, int cantidad, vector <int> &conecciones)
{
    
    for (int i = 0; i < cantidad; i++)
    {
        int s1;
        struct sockaddr_in remote;
        int t;

        if((s1 = accept(s, (struct sockaddr *) &remote, (socklen_t *) &t))== -1)
        {
            perror("aceptando la conexion entrante");
            exit(1);
        }

        conecciones.push_back(s1);
    }   
}

void P2P (vector <int> ports, vector <int> &nuevos_clientes)
{   
    int socket_fd; 
    struct sockaddr_in remote;
    
    for (int i = 0; i < ports.size(); i++)
    {
        if((socket_fd = socket (PF_INET, SOCK_STREAM, 0))== -1)
        {
            perror("creando socket");
            exit(1);
        }    

        remote.sin_family = AF_INET;
        remote.sin_port = htons(ports [i]);
        inet_pton(AF_INET, "127.0.0.1", &(remote.sin_addr));

        int result = connect(socket_fd, (struct sockaddr *)&remote, sizeof(remote));
        if (result == -1)
        {
            perror ("conectandose");
            exit(1);
        } 

        nuevos_clientes.push_back(socket_fd);
    }
    
    
}


int main()
{
    request req; 
    int socket_fd;
    int s, s1;

    struct sockaddr_in remote;
    struct in_addr addr;
    struct sockaddr_in local;

    vector <int> direcciones;
    vector <int> conexiones_aceptadas;
    vector <int> conexiones_establecidas;

    srand(getpid());
    bool state = rand() % 2 == 1; 

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
    

    struct pollfd pfds[1];
    pfds[0].fd = socket_fd;
    pfds[0].events = POLLIN;

    while (true)
    {

        int num_events = poll(pfds, 1, -1);
        int pollin_happened = pfds[0].revents & POLLIN;

        if (pollin_happened) 
        {
           recvrequest(socket_fd, req); 

            string type_req(req.type);
            
            if (type_req == "NEW_CLIENTS")
            {
                string j = "";  
                int i;  
                int puertos;
                vector <int> ports;
                
                for(i = 0; i < MENSAJE_MAXIMO; i++)  
                {    
                    if(req.msg[i] == '\0')
                    {
                        break;
                    }  

                    if(req.msg[i] == 'k')
                    {
                        int puertos = stoi(j);
                        j = "";
                        ports.push_back(puertos);
                    } 
                    else       
                    {
                        j += req.msg[i];
                    }        
                } 

                thread acept_conect = thread (aceptar_conecciones, s, ports.size(), ref(conexiones_aceptadas));
                P2P(ports, ref(conexiones_establecidas));
                acept_conect.join();

                strncpy(req.type, "CLIENT_READY", 15);
                sendrequest(socket_fd, req);
            }

            if (type_req == "TICK")
            {
                strncpy(req.type, "ESTADO", 15);
                
                if (state == true)
                {
                    strncpy(req.msg, "VIVO", MENSAJE_MAXIMO);
                }
                else
                {
                    strncpy(req.msg, "MUERTO", MENSAJE_MAXIMO);
                }

                thread broadcasting;
                broadcasting = thread(broadcast, conexiones_establecidas, req);

                int counter = 0;

                for (int i = 0; i < conexiones_aceptadas.size(); i++)
                {

                    recvrequest(conexiones_aceptadas[i], req);
                    string msg_req(req.msg);

                    if (msg_req == "VIVO")
                    {
                        counter++;
                    }

                }

                broadcasting.join();

                cout << state << endl;
                cout << counter << endl;

                if (counter == 3 && state == false)
                {
                    state = true;
                }
                else if (counter == 2 && state == true || counter == 3 && state == true)
                {
                    state = true;
                }
                else
                {
                    state = false;
                }

                if (state == true)
                {
                    strncpy(req.msg, "VIVO", MENSAJE_MAXIMO);
                }
                else
                {
                    strncpy(req.msg, "MUERTO", MENSAJE_MAXIMO);
                }

                cout << state << endl;
                
                strncpy(req.type, "CLIENT_STATE", 15);
                sendrequest(socket_fd, req); 
            }                
        }            
    }  

    close(socket_fd);

    return 0;
    
}